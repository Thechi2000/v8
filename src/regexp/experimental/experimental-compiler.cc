// Copyright 2020 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/regexp/experimental/experimental-compiler.h"

#include <optional>
#include <variant>

#include "src/base/logging.h"
#include "src/base/strings.h"
#include "src/regexp/experimental/experimental-bytecode.h"
#include "src/regexp/experimental/experimental.h"
#include "src/zone/zone-list-inl.h"
#include "src/zone/zone-list.h"

namespace v8 {
namespace internal {

namespace {

// TODO(mbid, v8:10765): Currently the experimental engine doesn't support
// UTF-16, but this shouldn't be too hard to implement.
constexpr base::uc32 kMaxSupportedCodepoint = 0xFFFFu;
#ifdef DEBUG
constexpr base::uc32 kMaxCodePoint = 0x10ffff;
#endif  // DEBUG

class CanBeHandledVisitor final : private RegExpVisitor {
  // Visitor to implement `ExperimentalRegExp::CanBeHandled`.
 public:
  static bool Check(RegExpTree* tree, RegExpFlags flags, int capture_count) {
    if (!AreSuitableFlags(flags)) return false;
    CanBeHandledVisitor visitor;
    tree->Accept(&visitor, nullptr);
    return visitor.result_;
  }

 private:
  CanBeHandledVisitor() = default;

  static bool AreSuitableFlags(RegExpFlags flags) {
    // TODO(mbid, v8:10765): We should be able to support all flags in the
    // future.
    static constexpr RegExpFlags kAllowedFlags =
        RegExpFlag::kGlobal | RegExpFlag::kSticky | RegExpFlag::kMultiline |
        RegExpFlag::kDotAll | RegExpFlag::kLinear;
    // We support Unicode iff kUnicode is among the supported flags.
    static_assert(ExperimentalRegExp::kSupportsUnicode ==
                  IsUnicode(kAllowedFlags));
    return (flags & ~kAllowedFlags) == 0;
  }

  void* VisitDisjunction(RegExpDisjunction* node, void*) override {
    for (RegExpTree* alt : *node->alternatives()) {
      alt->Accept(this, nullptr);
      if (!result_) {
        return nullptr;
      }
    }
    return nullptr;
  }

  void* VisitAlternative(RegExpAlternative* node, void*) override {
    for (RegExpTree* child : *node->nodes()) {
      child->Accept(this, nullptr);
      if (!result_) {
        return nullptr;
      }
    }
    return nullptr;
  }

  void* VisitClassRanges(RegExpClassRanges* node, void*) override {
    return nullptr;
  }

  void* VisitClassSetOperand(RegExpClassSetOperand* node, void*) override {
    result_ = !node->has_strings();
    return nullptr;
  }

  void* VisitClassSetExpression(RegExpClassSetExpression* node,
                                void*) override {
    result_ = false;
    return nullptr;
  }

  void* VisitAssertion(RegExpAssertion* node, void*) override {
    return nullptr;
  }

  void* VisitAtom(RegExpAtom* node, void*) override { return nullptr; }

  void* VisitText(RegExpText* node, void*) override {
    for (TextElement& el : *node->elements()) {
      el.tree()->Accept(this, nullptr);
      if (!result_) {
        return nullptr;
      }
    }
    return nullptr;
  }

  void* VisitQuantifier(RegExpQuantifier* node, void*) override {
    // Finite but large values of `min()` and `max()` are bad for the
    // breadth-first engine because finite (optional) repetition is dealt with
    // by replicating the bytecode of the body of the quantifier.  The number
    // of replicatons grows exponentially in how deeply quantifiers are nested.
    // `replication_factor_` keeps track of how often the current node will
    // have to be replicated in the generated bytecode, and we don't allow this
    // to exceed some small value.
    static constexpr int kMaxReplicationFactor = 16;

    // First we rule out values for min and max that are too big even before
    // taking into account the ambient replication_factor_.  This also guards
    // against overflows in `local_replication` or `replication_factor_`.
    if (node->min() > kMaxReplicationFactor ||
        (node->max() != RegExpTree::kInfinity &&
         node->max() > kMaxReplicationFactor)) {
      result_ = false;
      return nullptr;
    }

    // Save the current replication factor so that it can be restored if we
    // return with `result_ == true`.
    int before_replication_factor = replication_factor_;

    int local_replication;
    if (node->max() == RegExpTree::kInfinity) {
      if (node->min() > 0 && node->min_match() > 0) {
        // Quantifier can be reduced to a non nullable plus.
        local_replication = std::max(node->min(), 1);
      } else {
        local_replication = node->min() + 1;
      }
    } else {
      local_replication = node->max();
    }

    replication_factor_ *= local_replication;
    if (replication_factor_ > kMaxReplicationFactor) {
      result_ = false;
      return nullptr;
    }

    switch (node->quantifier_type()) {
      case RegExpQuantifier::GREEDY:
      case RegExpQuantifier::NON_GREEDY:
        break;
      case RegExpQuantifier::POSSESSIVE:
        // TODO(mbid, v8:10765): It's not clear to me whether this can be
        // supported in breadth-first mode. Re2 doesn't support it.
        result_ = false;
        return nullptr;
    }

    node->body()->Accept(this, nullptr);
    replication_factor_ = before_replication_factor;
    return nullptr;
  }

  void* VisitCapture(RegExpCapture* node, void*) override {
    if (inside_lookaround_) {
      result_ = false;
    } else {
      node->body()->Accept(this, nullptr);
    }
    return nullptr;
  }

  void* VisitGroup(RegExpGroup* node, void*) override {
    node->body()->Accept(this, nullptr);
    return nullptr;
  }

  void* VisitLookaround(RegExpLookaround* node, void*) override {
    // TODO(mbid, v8:10765): This will be hard to support, but not impossible I
    // think.  See product automata.
    auto temp = inside_lookaround_;
    inside_lookaround_ = true;

    if (node->type() == RegExpLookaround::Type::LOOKAHEAD) {
      result_ = false;
    } else {
      node->body()->Accept(this, nullptr);
    }

    inside_lookaround_ = temp;

    return nullptr;
  }

  void* VisitBackReference(RegExpBackReference* node, void*) override {
    // This can't be implemented without backtracking.
    result_ = false;
    return nullptr;
  }

  void* VisitEmpty(RegExpEmpty* node, void*) override { return nullptr; }

 private:
  // See comment in `VisitQuantifier`:
  int replication_factor_ = 1;
  bool inside_lookaround_ = false;

  bool result_ = true;
};

}  // namespace

bool ExperimentalRegExpCompiler::CanBeHandled(RegExpTree* tree,
                                              RegExpFlags flags,
                                              int capture_count) {
  return CanBeHandledVisitor::Check(tree, flags, capture_count);
}

namespace {

class BytecodeAssembler;

// A label in bytecode which starts with no known address. The address *must*
// be bound with `Bind` before the label goes out of scope.
// Implemented as a linked list through the `payload.pc` of FORK and JMP
// instructions.
struct Label {
 public:
  explicit Label(BytecodeAssembler& assembler);
  ~Label() = default;

 private:
  friend class BytecodeAssembler;

  int id_;
};

class BytecodeAssembler {
 private:
  struct Inst {
    RegExpInstruction inst;
    std::optional<Label> label;
  };
  typedef std::variant<Label, Inst> Instruction;

 public:
  // TODO(mbid,v8:10765): Use some upper bound for code_ capacity computed from
  // the `tree` size we're going to compile?
  explicit BytecodeAssembler(Zone* zone)
      : zone_(zone),
        code_(0, zone),
        code_stack_(zone),
        lookbehinds_(zone_),
        label_fresh_id_(0) {}

  ZoneList<RegExpInstruction> IntoCode() && {
    ZoneList<int> label_positions(label_fresh_id_, zone_);
    for (int i = 0; i < label_fresh_id_; ++i) {
      label_positions.Add(-1, zone_);
    }

    auto code(std::move(code_));
    for (auto it = lookbehinds_.begin(); it != lookbehinds_.end(); ++it) {
      code.AddAll(*it, zone_);
    }

    int instruction_count = 0;
    for (auto it : code) {
      if (it.index() == 0) {
        Label& label = std::get<Label>(it);
        DCHECK_LT(label.id_, label_fresh_id_);
        DCHECK_EQ(label_positions[label.id_], -1);

        label_positions[label.id_] = instruction_count + 1;
      } else {
        ++instruction_count;
      }
    }

    ZoneList<RegExpInstruction> out(0, zone_);
    for (auto it : code) {
      if (it.index() == 1) {
        Inst& inst = std::get<Inst>(it);
        RegExpInstruction instruction(inst.inst);

        if (inst.label.has_value()) {
          instruction.payload.pc = label_positions[inst.label->id_];
        }

        out.Add(instruction, zone_);
      }
    }

    return out;
  }

  int getLabelFreshId() { return label_fresh_id_++; }

  void Accept() { Add(RegExpInstruction::Accept()); }

  void Assertion(RegExpAssertion::Type t) {
    Add(RegExpInstruction::Assertion(t));
  }

  void ClearRegister(int32_t register_index) {
    Add(RegExpInstruction::ClearRegister(register_index));
  }

  void ConsumeRange(base::uc16 from, base::uc16 to) {
    Add(RegExpInstruction::ConsumeRange(from, to));
  }

  void ConsumeAnyChar() { Add(RegExpInstruction::ConsumeAnyChar()); }

  void Fork(Label& target) {
    LabelledInstrImpl(RegExpInstruction::Opcode::FORK, target);
  }

  void Jmp(Label& target) {
    LabelledInstrImpl(RegExpInstruction::Opcode::JMP, target);
  }

  void SetRegisterToCp(int32_t register_index) {
    Add(RegExpInstruction::SetRegisterToCp(register_index));
  }

  void BeginLoop() { Add(RegExpInstruction::BeginLoop()); }

  void EndLoop() { Add(RegExpInstruction::EndLoop()); }

  void Bind(Label target) { code_.Add(target, zone_); }

  void Fail() { Add(RegExpInstruction::Fail()); }

  void StartLookBehind() {
    code_stack_.push_front(std::move(code_));
    code_.DropAndClear();

    Label begin(*this);
    Bind(begin);
    ConsumeAnyChar();
    Fork(begin);
  }

  void EndLookBehind(int32_t index) {
    Add(RegExpInstruction::WriteLookTable(index));
    lookbehinds_.push_back(std::move(code_));
    code_.DropAndClear();

    code_.AddAll(code_stack_.front(), zone_);
    code_stack_.pop_front();

    Add(RegExpInstruction::ReadLookTable(index));
  }

 private:
  void Add(RegExpInstruction inst) {
    code_.Add(Inst{.inst = inst, .label = std::optional<Label>()}, zone_);
  }

  void LabelledInstrImpl(RegExpInstruction::Opcode op, Label& target) {
    code_.Add(Inst{.inst =
                       RegExpInstruction{
                           .opcode = op,
                           .payload = {.pc = -1},
                       },
                   .label = std::optional<Label>(target)},
              zone_);
  }

  Zone* zone_;
  ZoneList<Instruction> code_;
  ZoneLinkedList<ZoneList<Instruction>> code_stack_;
  ZoneLinkedList<ZoneList<Instruction>> lookbehinds_;
  int label_fresh_id_;
};

Label::Label(BytecodeAssembler& assembler) : id_(assembler.getLabelFreshId()) {}

class CompileVisitor : private RegExpVisitor {
 public:
  static ZoneList<RegExpInstruction> Compile(RegExpTree* tree,
                                             RegExpFlags flags, Zone* zone) {
    CompileVisitor compiler(zone);

    if (!IsSticky(flags) && !tree->IsAnchoredAtStart()) {
      // The match is not anchored, i.e. may start at any input position, so we
      // emit a preamble corresponding to /.*?/.  This skips an arbitrary
      // prefix in the input non-greedily.
      compiler.CompileNonGreedyStar(
          [&]() { compiler.assembler_.ConsumeAnyChar(); });
    }

    compiler.assembler_.SetRegisterToCp(0);
    tree->Accept(&compiler, nullptr);
    compiler.assembler_.SetRegisterToCp(1);
    compiler.assembler_.Accept();

    return std::move(compiler.assembler_).IntoCode();
  }

 private:
  explicit CompileVisitor(Zone* zone) : zone_(zone), assembler_(zone) {}

  // Generate a disjunction of code fragments compiled by a function `alt_gen`.
  // `alt_gen` is called repeatedly with argument `int i = 0, 1, ..., alt_num -
  // 1` and should build code corresponding to the ith alternative.
  template <class F>
  void CompileDisjunction(int alt_num, F&& gen_alt) {
    // An alternative a1 | ... | an is compiled into
    //
    //     FORK tail1
    //     <a1>
    //     JMP end
    //   tail1:
    //     FORK tail2
    //     <a2>
    //     JMP end
    //   tail2:
    //     ...
    //     ...
    //   tail{n -1}:
    //     <an>
    //   end:
    //
    // By the semantics of the FORK instruction (see above at definition and
    // semantics), a forked thread has lower priority than the thread that
    // spawned it.  This means that with the code we're generating here, the
    // thread matching the alternative a1 has indeed highest priority, followed
    // by the thread for a2 and so on.

    if (alt_num == 0) {
      // The empty disjunction.  This can never match.
      assembler_.Fail();
      return;
    }

    Label end(assembler_);

    for (int i = 0; i != alt_num - 1; ++i) {
      Label tail(assembler_);
      assembler_.Fork(tail);
      gen_alt(i);
      assembler_.Jmp(end);
      assembler_.Bind(tail);
    }

    gen_alt(alt_num - 1);

    assembler_.Bind(end);
  }

  void* VisitDisjunction(RegExpDisjunction* node, void*) override {
    ZoneList<RegExpTree*>& alts = *node->alternatives();
    CompileDisjunction(alts.length(),
                       [&](int i) { alts[i]->Accept(this, nullptr); });
    return nullptr;
  }

  void* VisitAlternative(RegExpAlternative* node, void*) override {
    for (RegExpTree* child : *node->nodes()) {
      child->Accept(this, nullptr);
    }
    return nullptr;
  }

  void* VisitAssertion(RegExpAssertion* node, void*) override {
    assembler_.Assertion(node->assertion_type());
    return nullptr;
  }

  void CompileCharacterRanges(ZoneList<CharacterRange>* ranges, bool negated) {
    // A character class is compiled as Disjunction over its `CharacterRange`s.
    CharacterRange::Canonicalize(ranges);
    if (negated) {
      // The complement of a disjoint, non-adjacent (i.e. `Canonicalize`d)
      // union of k intervals is a union of at most k + 1 intervals.
      ZoneList<CharacterRange>* negated =
          zone_->New<ZoneList<CharacterRange>>(ranges->length() + 1, zone_);
      CharacterRange::Negate(ranges, negated, zone_);
      DCHECK_LE(negated->length(), ranges->length() + 1);
      ranges = negated;
    }

    CompileDisjunction(ranges->length(), [&](int i) {
      // We don't support utf16 for now, so only ranges that can be specified
      // by (complements of) ranges with base::uc16 bounds.
      static_assert(kMaxSupportedCodepoint <=
                    std::numeric_limits<base::uc16>::max());

      base::uc32 from = (*ranges)[i].from();
      DCHECK_LE(from, kMaxSupportedCodepoint);
      base::uc16 from_uc16 = static_cast<base::uc16>(from);

      base::uc32 to = (*ranges)[i].to();
      DCHECK_IMPLIES(to > kMaxSupportedCodepoint, to == kMaxCodePoint);
      base::uc16 to_uc16 =
          static_cast<base::uc16>(std::min(to, kMaxSupportedCodepoint));

      assembler_.ConsumeRange(from_uc16, to_uc16);
    });
  }

  void* VisitClassRanges(RegExpClassRanges* node, void*) override {
    CompileCharacterRanges(node->ranges(zone_), node->is_negated());
    return nullptr;
  }

  void* VisitClassSetOperand(RegExpClassSetOperand* node, void*) override {
    // TODO(v8:11935): Support strings.
    DCHECK(!node->has_strings());
    CompileCharacterRanges(node->ranges(), false);
    return nullptr;
  }

  void* VisitClassSetExpression(RegExpClassSetExpression* node,
                                void*) override {
    // TODO(v8:11935): Add support.
    UNREACHABLE();
  }

  void* VisitAtom(RegExpAtom* node, void*) override {
    for (base::uc16 c : node->data()) {
      assembler_.ConsumeRange(c, c);
    }
    return nullptr;
  }

  void ClearRegisters(Interval indices) {
    if (indices.is_empty()) return;
    DCHECK_EQ(indices.from() % 2, 0);
    DCHECK_EQ(indices.to() % 2, 1);
    for (int i = indices.from(); i <= indices.to(); i += 2) {
      // It suffices to clear the register containing the `begin` of a capture
      // because this indicates that the capture is undefined, regardless of
      // the value in the `end` register.
      assembler_.ClearRegister(i);
    }
  }

  // Emit bytecode corresponding to /<emit_body>*/.
  template <class F>
  void CompileGreedyStar(F&& emit_body) {
    // This is compiled into
    //
    //   begin:
    //     FORK end
    //     BEGIN_LOOP
    //     <body>
    //     END_LOOP
    //     JMP begin
    //   end:
    //     ...
    //
    // This is greedy because a forked thread has lower priority than the
    // thread that spawned it.
    Label begin(assembler_);
    Label end(assembler_);

    assembler_.Bind(begin);
    assembler_.Fork(end);
    assembler_.BeginLoop();
    emit_body();
    assembler_.EndLoop();
    assembler_.Jmp(begin);

    assembler_.Bind(end);
  }

  // Emit bytecode corresponding to /<emit_body>*?/.
  template <class F>
  void CompileNonGreedyStar(F&& emit_body) {
    // This is compiled into
    //
    //     FORK body
    //     JMP end
    //   body:
    //     BEGIN_LOOP
    //     <body>
    //     END_LOOP
    //     FORK body
    //   end:
    //     ...

    Label body(assembler_);
    Label end(assembler_);

    assembler_.Fork(body);
    assembler_.Jmp(end);

    assembler_.Bind(body);
    assembler_.BeginLoop();
    emit_body();
    assembler_.EndLoop();
    assembler_.Fork(body);

    assembler_.Bind(end);
  }

  // Emit bytecode corresponding to /<emit_body>{0, max_repetition_num}/.
  template <class F>
  void CompileGreedyRepetition(F&& emit_body, int max_repetition_num) {
    // This is compiled into
    //
    //     FORK end
    //     BEGIN_LOOP
    //     <body>
    //     END_LOOP
    //     FORK end
    //     BEGIN_LOOP
    //     <body>
    //     END_LOOP
    //     ...
    //     ...
    //     FORK end
    //     <body>
    //   end:
    //     ...
    //
    // We add `BEGIN_LOOP` and `END_LOOP` instructions because these optional
    // repetitions of the body cannot match the empty string.

    Label end(assembler_);
    for (int i = 0; i != max_repetition_num; ++i) {
      assembler_.Fork(end);
      assembler_.BeginLoop();
      emit_body();
      assembler_.EndLoop();
    }
    assembler_.Bind(end);
  }

  // Emit bytecode corresponding to /<emit_body>{0, max_repetition_num}?/.
  template <class F>
  void CompileNonGreedyRepetition(F&& emit_body, int max_repetition_num) {
    // This is compiled into
    //
    //     FORK body0
    //     JMP end
    //   body0:
    //     BEGIN_LOOP
    //     <body>
    //     END_LOOP
    //
    //     FORK body1
    //     JMP end
    //   body1:
    //     BEGIN_LOOP
    //     <body>
    //     END_LOOP
    //     ...
    //     ...
    //   body{max_repetition_num - 1}:
    //     BEGIN_LOOP
    //     <body>
    //     END_LOOP
    //   end:
    //     ...
    //
    // We add `BEGIN_LOOP` and `END_LOOP` instructions because these optional
    // repetitions of the body cannot match the empty string.

    Label end(assembler_);
    for (int i = 0; i != max_repetition_num; ++i) {
      Label body(assembler_);
      assembler_.Fork(body);
      assembler_.Jmp(end);

      assembler_.Bind(body);
      assembler_.BeginLoop();
      emit_body();
      assembler_.EndLoop();
    }
    assembler_.Bind(end);
  }

  // In the general case, the first repetition of <body>+ is different
  // from the following ones as it is allowed to match the empty string. This is
  // compiled by repeating <body>, but it can result in a bytecode that grows
  // quadratically with the size of the regex when nesting pluses or repetition
  // upper-bounded with infinity.
  //
  // In the particular case where <body> cannot match the empty string, the
  // plus can be compiled without duplicating the bytecode of <body>, resulting
  // in a bytecode linear in the size of the regex in case of nested
  // non-nullable pluses.
  //
  // E.g. `/.+/` will compile `/./` once, while `/(?:.?)+/` will be compiled as
  // `/(?:.?)(?:.?)*/`, resulting in two repetitions of the body.

  // Emit bytecode corresponding to /<emit_body>+/, with <emit_body> not
  // nullable.
  template <class F>
  void CompileNonNullableGreedyPlus(F&& emit_body) {
    // This is compiled into
    //
    //   begin:
    //     <body>
    //
    //     FORK end
    //     JMP begin
    //   end:
    //     ...
    Label begin(assembler_), end(assembler_);

    assembler_.Bind(begin);
    emit_body();

    assembler_.Fork(end);
    assembler_.Jmp(begin);
    assembler_.Bind(end);
  }

  // Emit bytecode corresponding to /<emit_body>+?/, with <emit_body> not
  // nullable.
  template <class F>
  void CompileNonNullableNonGreedyPlus(F&& emit_body) {
    // This is compiled into
    //
    //   begin:
    //     <body>
    //
    //     FORK begin
    //     ...
    Label begin(assembler_);

    assembler_.Bind(begin);
    emit_body();

    assembler_.Fork(begin);
  }

  void* VisitQuantifier(RegExpQuantifier* node, void*) override {
    // Emit the body, but clear registers occuring in body first.
    //
    // TODO(mbid,v8:10765): It's not always necessary to a) capture registers
    // and b) clear them. For example, we don't have to capture anything for
    // the first 4 repetitions if node->min() >= 5, and then we don't have to
    // clear registers in the first node->min() repetitions.
    // Later, and if node->min() == 0, we don't have to clear registers before
    // the first optional repetition.
    Interval body_registers = node->body()->CaptureRegisters();
    auto emit_body = [&]() {
      ClearRegisters(body_registers);
      node->body()->Accept(this, nullptr);
    };

    bool can_be_reduced_to_non_nullable_plus =
        node->min() > 0 && node->max() == RegExpTree::kInfinity &&
        node->min_match() > 0;

    if (can_be_reduced_to_non_nullable_plus) {
      // Compile <body>+ with an optimization allowing linear sized bytecode in
      // the case of nested pluses. Repetitions with infinite upperbound like
      // <body>{n,}, with n != 0, are compiled into <body>{n-1}<body+>, avoiding
      // one repetition, compared to <body>{n}<body>*.

      // Compile the mandatory repetitions. We repeat `min() - 1` times, such
      // that the last repetition, compiled later, can be reused in a loop.
      for (int i = 0; i < node->min() - 1; ++i) {
        emit_body();
      }

      // Compile the optional repetitions, using an optimized plus when
      // possible.
      switch (node->quantifier_type()) {
        case RegExpQuantifier::POSSESSIVE:
          UNREACHABLE();
        case RegExpQuantifier::GREEDY: {
          // Compile both last mandatory repetition and optional ones.
          CompileNonNullableGreedyPlus(emit_body);
          break;
        }
        case RegExpQuantifier::NON_GREEDY: {
          // Compile both last mandatory repetition and optional ones.
          CompileNonNullableNonGreedyPlus(emit_body);
          break;
        }
      }
    } else {
      // Compile <body>+ into <body><body>*, and <body>{n,}, with n != 0, into
      // <body>{n}<body>*.

      // Compile the first `min()` repetitions.
      for (int i = 0; i < node->min(); ++i) {
        emit_body();
      }

      // Compile the optional repetitions, using stars or repetitions.
      switch (node->quantifier_type()) {
        case RegExpQuantifier::POSSESSIVE:
          UNREACHABLE();
        case RegExpQuantifier::GREEDY: {
          if (node->max() == RegExpTree::kInfinity) {
            CompileGreedyStar(emit_body);
          } else {
            DCHECK_NE(node->max(), RegExpTree::kInfinity);
            CompileGreedyRepetition(emit_body, node->max() - node->min());
          }
          break;
        }
        case RegExpQuantifier::NON_GREEDY: {
          if (node->max() == RegExpTree::kInfinity) {
            CompileNonGreedyStar(emit_body);
          } else {
            DCHECK_NE(node->max(), RegExpTree::kInfinity);
            CompileNonGreedyRepetition(emit_body, node->max() - node->min());
          }
          break;
        }
      }
    }

    return nullptr;
  }

  void* VisitCapture(RegExpCapture* node, void*) override {
    int index = node->index();
    int start_register = RegExpCapture::StartRegister(index);
    int end_register = RegExpCapture::EndRegister(index);
    assembler_.SetRegisterToCp(start_register);
    node->body()->Accept(this, nullptr);
    assembler_.SetRegisterToCp(end_register);
    return nullptr;
  }

  void* VisitGroup(RegExpGroup* node, void*) override {
    node->body()->Accept(this, nullptr);
    return nullptr;
  }

  void* VisitLookaround(RegExpLookaround* node, void*) override {
    // TODO(mbid,v8:10765): Support this case.
    if (node->type() == RegExpLookaround::Type::LOOKBEHIND) {
      assembler_.StartLookBehind();
      node->body()->Accept(this, nullptr);
      assembler_.EndLookBehind(node->index());
    } else {
      UNREACHABLE();
    }

    return nullptr;
  }

  void* VisitBackReference(RegExpBackReference* node, void*) override {
    UNREACHABLE();
  }

  void* VisitEmpty(RegExpEmpty* node, void*) override { return nullptr; }

  void* VisitText(RegExpText* node, void*) override {
    for (TextElement& text_el : *node->elements()) {
      text_el.tree()->Accept(this, nullptr);
    }
    return nullptr;
  }

 private:
  Zone* zone_;
  BytecodeAssembler assembler_;
};

}  // namespace

ZoneList<RegExpInstruction> ExperimentalRegExpCompiler::Compile(
    RegExpTree* tree, RegExpFlags flags, Zone* zone) {
  return CompileVisitor::Compile(tree, flags, zone);
}

}  // namespace internal
}  // namespace v8
