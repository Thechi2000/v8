// Copyright 2020 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/regexp/experimental/experimental-compiler.h"

#include "src/flags/flags.h"
#include "src/regexp/experimental/experimental.h"
#include "src/regexp/regexp-ast.h"
#include "src/zone/zone-containers.h"
#include "src/zone/zone-list-inl.h"

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
    CanBeHandledVisitor visitor{flags};
    tree->Accept(&visitor, nullptr);
    return visitor.result_;
  }

 private:
  explicit CanBeHandledVisitor(RegExpFlags flags) : flags_(flags) {}

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
    // of replications grows exponentially in how deeply quantifiers are nested.
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
    node->body()->Accept(this, nullptr);
    return nullptr;
  }

  void* VisitGroup(RegExpGroup* node, void*) override {
    if (flags() != node->flags()) {
      // Flags that aren't supported by the experimental engine at all, are not
      // supported via modifiers either.
      // TODO(pthier): Currently the only flag supported in modifiers and in
      // the experimental engine is multi-line, which is already handled in the
      // parser. If more flags are supported either by the experimental engine
      // or in modifiers we need to add general support for modifiers to the
      // experimental engine.
      if (!AreSuitableFlags(node->flags())) {
        result_ = false;
        return nullptr;
      }
    }
    node->body()->Accept(this, nullptr);
    return nullptr;
  }

  void* VisitLookaround(RegExpLookaround* node, void*) override {
    if (IsGlobal(flags()) || IsSticky(flags())) {
      result_ = false;
      return nullptr;
    }

    if (node->type() == RegExpLookaround::LOOKAHEAD ||
        node->capture_count() > 0) {
      if (!v8_flags.experimental_regexp_engine_capture_group_opt) {
        result_ = false;
        return nullptr;
      }

      node->body()->Accept(this, nullptr);
    }
    return nullptr;
  }

  void* VisitBackReference(RegExpBackReference* node, void*) override {
    // This can't be implemented without backtracking.
    result_ = false;
    return nullptr;
  }

  void* VisitEmpty(RegExpEmpty* node, void*) override { return nullptr; }

 private:
  RegExpFlags flags() const { return flags_; }

  // See comment in `VisitQuantifier`:
  int replication_factor_ = 1;

  bool result_ = true;
  RegExpFlags flags_;
};

}  // namespace

bool ExperimentalRegExpCompiler::CanBeHandled(RegExpTree* tree,
                                              RegExpFlags flags,
                                              int capture_count) {
  return CanBeHandledVisitor::Check(tree, flags, capture_count);
}

namespace {

// A label in bytecode which starts with no known address. The address *must*
// be bound with `Bind` before the label goes out of scope.
// Implemented as a linked list through the `payload.pc` of FORK and JMP
// instructions.
struct Label {
 public:
  Label() = default;
  ~Label() {
    DCHECK_EQ(state_, BOUND);
    DCHECK_GE(bound_index_, 0);
  }

  // Don't copy, don't move.  Moving could be implemented, but it's not
  // needed anywhere.
  Label(const Label&) = delete;
  Label& operator=(const Label&) = delete;

 private:
  friend class BytecodeAssembler;

  // UNBOUND implies unbound_patch_list_begin_.
  // BOUND implies bound_index_.
  enum { UNBOUND, BOUND } state_ = UNBOUND;
  union {
    int unbound_patch_list_begin_ = -1;
    int bound_index_;
  };
};

class BytecodeAssembler {
 public:
  // TODO(mbid,v8:10765): Use some upper bound for code_ capacity computed from
  // the `tree` size we're going to compile?
  explicit BytecodeAssembler(Zone* zone) : zone_(zone), code_(0, zone) {}

  ZoneList<RegExpInstruction> IntoCode() && { return std::move(code_); }

  void Accept() { code_.Add(RegExpInstruction::Accept(), zone_); }

  void Assertion(RegExpAssertion::Type t) {
    code_.Add(RegExpInstruction::Assertion(t), zone_);
  }

  void ClearRegister(int32_t register_index) {
    code_.Add(RegExpInstruction::ClearRegister(register_index), zone_);
  }

  void ConsumeRange(base::uc16 from, base::uc16 to) {
    code_.Add(RegExpInstruction::ConsumeRange(from, to), zone_);
  }

  void ConsumeAnyChar() {
    code_.Add(RegExpInstruction::ConsumeAnyChar(), zone_);
  }

  void Fork(Label& target) {
    LabelledInstrImpl(RegExpInstruction::Opcode::FORK, target);
  }

  void Jmp(Label& target) {
    LabelledInstrImpl(RegExpInstruction::Opcode::JMP, target);
  }

  void SetRegisterToCp(int32_t register_index) {
    code_.Add(RegExpInstruction::SetRegisterToCp(register_index), zone_);
  }

  void BeginLoop() { code_.Add(RegExpInstruction::BeginLoop(), zone_); }

  void EndLoop() { code_.Add(RegExpInstruction::EndLoop(), zone_); }

  void StartLookaround(int lookaround_index, bool is_ahead) {
    code_.Add(RegExpInstruction::StartLookaround(lookaround_index, is_ahead),
              zone_);
  }

  void EndLookaround() { code_.Add(RegExpInstruction::EndLookaround(), zone_); }

  void WriteLookaroundTable(int index) {
    code_.Add(RegExpInstruction::WriteLookTable(index), zone_);
  }

  void ReadLookaroundTable(int index, bool is_positive) {
    code_.Add(RegExpInstruction::ReadLookTable(index, is_positive), zone_);
  }

  void SetQuantifierToClock(int32_t quantifier_id) {
    code_.Add(RegExpInstruction::SetQuantifierToClock(quantifier_id), zone_);
  }

  void FilterQuantifier(int32_t quantifier_id) {
    code_.Add(RegExpInstruction::FilterQuantifier(quantifier_id), zone_);
  }

  void FilterGroup(int32_t group_id) {
    code_.Add(RegExpInstruction::FilterGroup(group_id), zone_);
  }

  void FilterLookaround(int32_t lookaround_id) {
    code_.Add(RegExpInstruction::FilterLookaround(lookaround_id), zone_);
  }

  void FilterChild(Label& target) {
    LabelledInstrImpl(RegExpInstruction::Opcode::FILTER_CHILD, target);
  }

  void Bind(Label& target) {
    DCHECK_EQ(target.state_, Label::UNBOUND);

    int index = code_.length();

    while (target.unbound_patch_list_begin_ != -1) {
      RegExpInstruction& inst = code_[target.unbound_patch_list_begin_];
      DCHECK(inst.opcode == RegExpInstruction::FORK ||
             inst.opcode == RegExpInstruction::JMP ||
             inst.opcode == RegExpInstruction::FILTER_CHILD);

      target.unbound_patch_list_begin_ = inst.payload.pc;
      inst.payload.pc = index;
    }

    target.state_ = Label::BOUND;
    target.bound_index_ = index;
  }

  void Fail() { code_.Add(RegExpInstruction::Fail(), zone_); }

 private:
  void LabelledInstrImpl(RegExpInstruction::Opcode op, Label& target) {
    RegExpInstruction result;
    result.opcode = op;

    if (target.state_ == Label::BOUND) {
      result.payload.pc = target.bound_index_;
    } else {
      DCHECK_EQ(target.state_, Label::UNBOUND);
      int new_list_begin = code_.length();
      DCHECK_GE(new_list_begin, 0);

      result.payload.pc = target.unbound_patch_list_begin_;

      target.unbound_patch_list_begin_ = new_list_begin;
    }

    code_.Add(result, zone_);
  }

  Zone* zone_;
  ZoneList<RegExpInstruction> code_;
};

class FilterGroupsCompileVisitor final : private RegExpVisitor {
 public:
  static void CompileFilter(Zone* zone, RegExpTree* tree,
                            BytecodeAssembler& assembler,
                            const ZoneMap<int, int>& quantifier_id_remapping) {
    /* To filter out groups that were not matched in the last iteration of a
     * quantifier, the regexp's AST is compiled using a special sets of
     * instructions: `FILTER_GROUP`, `FILTER_QUANTIFIER` and `FILTER_CHILD`.
     * They encode a simplified AST containing only the groups and quantifiers.
     * Each node is represented as either a `FILTER_GROUP` or a
     * `FILTER_QUANTIFIER` instruction, containing the index of the respective
     * group or quantifier, followed by a variable number of `FILTER_CHILD`
     * instructions each containing the index of their respective node in the
     * bytecode.
     *
     * The regexp's AST is traversed in breadth-first mode, compiling one node
     * at a time, while saving its children in a queue. */

    FilterGroupsCompileVisitor visitor(assembler, zone,
                                       quantifier_id_remapping);

    tree->Accept(&visitor, nullptr);

    while (!visitor.nodes_.empty()) {
      auto& entry = visitor.nodes_.front();

      visitor.assembler_.Bind(entry.label);
      visitor.can_compile_node_ = true;
      entry.node->Accept(&visitor, nullptr);

      visitor.nodes_.pop_front();
    }
  }

 private:
  FilterGroupsCompileVisitor(BytecodeAssembler& assembler, Zone* zone,
                             const ZoneMap<int, int>& quantifier_id_remapping)
      : zone_(zone),
        assembler_(assembler),
        nodes_(zone_),
        can_compile_node_(false),
        quantifier_id_remapping_(quantifier_id_remapping) {}

  void* VisitDisjunction(RegExpDisjunction* node, void*) override {
    for (RegExpTree* alt : *node->alternatives()) {
      alt->Accept(this, nullptr);
    }
    return nullptr;
  }

  void* VisitAlternative(RegExpAlternative* node, void*) override {
    for (RegExpTree* alt : *node->nodes()) {
      alt->Accept(this, nullptr);
    }
    return nullptr;
  }

  void* VisitClassRanges(RegExpClassRanges* node, void*) override {
    return nullptr;
  }

  void* VisitClassSetOperand(RegExpClassSetOperand* node, void*) override {
    return nullptr;
  }

  void* VisitClassSetExpression(RegExpClassSetExpression* node,
                                void*) override {
    return nullptr;
  }

  void* VisitAssertion(RegExpAssertion* node, void*) override {
    return nullptr;
  }

  void* VisitAtom(RegExpAtom* node, void*) override { return nullptr; }

  void* VisitText(RegExpText* node, void*) override { return nullptr; }

  void* VisitQuantifier(RegExpQuantifier* node, void*) override {
    if (can_compile_node_) {
      assembler_.FilterQuantifier(quantifier_id_remapping_.at(node->index()));
      can_compile_node_ = false;
      node->body()->Accept(this, nullptr);
    } else {
      if (node->CaptureRegisters().is_empty()) {
        return nullptr;
      }

      nodes_.emplace_back(node);
      assembler_.FilterChild(nodes_.back().label);
    }

    return nullptr;
  }

  void* VisitCapture(RegExpCapture* node, void*) override {
    if (can_compile_node_) {
      assembler_.FilterGroup(node->index());
      can_compile_node_ = false;
      node->body()->Accept(this, nullptr);
    } else {
      nodes_.emplace_back(node);
      assembler_.FilterChild(nodes_.back().label);
    }

    return nullptr;
  }

  void* VisitGroup(RegExpGroup* node, void*) override {
    node->body()->Accept(this, nullptr);
    return nullptr;
  }

  void* VisitLookaround(RegExpLookaround* node, void*) override {
    if (can_compile_node_) {
      assembler_.FilterLookaround(node->index());
      can_compile_node_ = false;
      node->body()->Accept(this, nullptr);
    } else {
      if (node->CaptureRegisters().is_empty()) {
        return nullptr;
      }

      nodes_.emplace_back(node);
      assembler_.FilterChild(nodes_.back().label);
    }

    return nullptr;
  }

  void* VisitBackReference(RegExpBackReference* node, void*) override {
    return nullptr;
  }

  void* VisitEmpty(RegExpEmpty* node, void*) override { return nullptr; }

 private:
  class BFEntry {
   public:
    explicit BFEntry(RegExpTree* node) : label(), node(node) {}

    Label label;
    RegExpTree* node;
  };

  Zone* zone_;

  BytecodeAssembler& assembler_;
  ZoneLinkedList<BFEntry> nodes_;

  // Whether we can compile a node or we should add it to `nodes_`. This is
  // set to true after popping an element from the queue, and false after
  // having compiled one.
  bool can_compile_node_;

  const ZoneMap<int, int>& quantifier_id_remapping_;
};

class CompileVisitor : private RegExpVisitor {
 public:
  static ZoneList<RegExpInstruction> Compile(RegExpTree* tree,
                                             RegExpFlags flags, Zone* zone) {
    CompileVisitor compiler(zone);

    if (!IsSticky(flags) && !tree->IsAnchoredAtStart()) {
      // The match is not anchored, i.e. may start at any input position, so
      // we emit a preamble corresponding to /.*?/.  This skips an arbitrary
      // prefix in the input non-greedily.
      compiler.CompileNonGreedyStar(
          [&]() { compiler.assembler_.ConsumeAnyChar(); });
    }

    compiler.assembler_.SetRegisterToCp(0);
    tree->Accept(&compiler, nullptr);
    compiler.assembler_.SetRegisterToCp(1);
    compiler.assembler_.Accept();

    if (v8_flags.experimental_regexp_engine_capture_group_opt) {
      FilterGroupsCompileVisitor::CompileFilter(
          zone, tree, compiler.assembler_,
          compiler.quantifier_id_remapping_.value());
    }

    // Each lookaround is compiled as two different bytecode sections: a
    // reverse, captureless automaton and a capturing one. They are then
    // added at the end of the bytecode, respecting the order in which they
    // were encountered. This ordering ensures that a lookaround can only
    // use others whose bytecode were already issued, and is used by the
    // interpreter to build a lookaround priority list.
    //
    // During the compilation of the main expression, lookarounds are not
    // immediately compiled, and are instead pushed at the back of a queue.
    // After compiling the filtering instructions, the queue is emptied and
    // each lookaround body is compiled into two sections:
    //
    // 1. A non-capturing automaton, which is reversed if it is a lookahead.
    // 2. A capturing automaton, reversed if it is a lookbehind. Since it is
    //    only used for capturing, it can be empty if the lookaround does
    //    not contain any captures.
    //
    // See {TODO} in experimental-interpreter.cc for detailed explanation
    // on the reversal's reasons. The sections are delimited by the
    // `START_LOOKAROUND` and `END_LOOKAROUND`, and the separation occurs
    // right after the `WRITE_LOOKAROUND_TABLE`.
    //
    // If another lookaround is encountered during this phase, it is added
    // at the back of the queue.
    //
    // {TODO still holds ?} This approach prevents the use of the sticky or
    // global flags. In both cases, when resuming the search, it starts at a
    // non null index, while the lookbehinds always need to start at the
    // beginning of the string. A future implementation for the global flag
    // may store the active lookbehind threads in the regexp to resume the
    // execution of the lookbehinds automata.
    while (!compiler.lookarounds_.empty()) {
      auto node = compiler.lookarounds_.front();
      compiler.CompileLookaround(node);
      compiler.lookarounds_.pop_front();
    }

    return std::move(compiler.assembler_).IntoCode();
  }

 private:
  explicit CompileVisitor(Zone* zone)
      : zone_(zone),
        lookarounds_(zone),
        quantifier_id_remapping_({}),
        assembler_(zone),
        reverse_(false),
        ignore_captures_(false),
        ignore_lookarounds_(false) {
    if (v8_flags.experimental_regexp_engine_capture_group_opt) {
      quantifier_id_remapping_.emplace(zone_);
    }
  }

  // Generate all the instructions to match and capture a lookaround.
  void CompileLookaround(RegExpLookaround* lookaround) {
    // Generate the first section, reversed in the case of a lookahead.
    // {TODO disable capture groups}
    assembler_.StartLookaround(
        lookaround->index(), lookaround->type() == RegExpLookaround::LOOKAHEAD);

    // If the lookaround is not anchored, we add a /.*/ at its start, such
    // that the resulting automaton will run over the whole input.
    if ((lookaround->type() == RegExpLookaround::LOOKAHEAD &&
         !lookaround->body()->IsAnchoredAtEnd()) ||
        (lookaround->type() == RegExpLookaround::LOOKBEHIND &&
         !lookaround->body()->IsAnchoredAtStart())) {
      CompileNonGreedyStar([&]() { assembler_.ConsumeAnyChar(); });
    }

    reverse_ = lookaround->type() == RegExpLookaround::LOOKAHEAD;

    ignore_captures_ = true;
    lookaround->body()->Accept(this, nullptr);
    ignore_captures_ = false;

    assembler_.WriteLookaroundTable(lookaround->index());

    // Generate the second sections, reversed in the case of a lookbehind.
    if (lookaround->capture_count() > 0 && lookaround->is_positive()) {
      reverse_ = lookaround->type() == RegExpLookaround::LOOKBEHIND;

      ignore_lookarounds_ = true;
      lookaround->body()->Accept(this, nullptr);
      ignore_lookarounds_ = false;
    }

    assembler_.EndLookaround();
  }

  // Generate a disjunction of code fragments compiled by a function
  // `alt_gen`. `alt_gen` is called repeatedly with argument `int i = 0, 1,
  // ..., alt_num - 1` and should build code corresponding to the ith
  // alternative.
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
    // thread matching the alternative a1 has indeed highest priority,
    // followed by the thread for a2 and so on.

    if (alt_num == 0) {
      // The empty disjunction.  This can never match.
      assembler_.Fail();
      return;
    }

    Label end;

    for (int i = 0; i != alt_num - 1; ++i) {
      Label tail;
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
    if (!reverse_) {
      for (RegExpTree* child : *node->nodes()) {
        child->Accept(this, nullptr);
      }
    } else {
      for (int i = node->nodes()->length() - 1; i >= 0; --i) {
        node->nodes()->at(i)->Accept(this, nullptr);
      }
    }
    return nullptr;
  }

  void* VisitAssertion(RegExpAssertion* node, void*) override {
    assembler_.Assertion(node->assertion_type());
    return nullptr;
  }

  void CompileCharacterRanges(ZoneList<CharacterRange>* ranges, bool negated) {
    // A character class is compiled as Disjunction over its
    // `CharacterRange`s.
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
      // We don't support utf16 for now, so only ranges that can be
      // specified by (complements of) ranges with base::uc16 bounds.
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
    if (!reverse_) {
      for (base::uc16 c : node->data()) {
        assembler_.ConsumeRange(c, c);
      }
    } else {
      for (int i = node->data().length() - 1; i >= 0; --i) {
        assembler_.ConsumeRange(node->data().at(i), node->data().at(i));
      }
    }
    return nullptr;
  }

  void ClearRegisters(Interval indices) {
    if (indices.is_empty()) return;
    DCHECK_EQ(indices.from() % 2, 0);
    DCHECK_EQ(indices.to() % 2, 1);
    for (int i = indices.from(); i <= indices.to(); i += 2) {
      // It suffices to clear the register containing the `begin` of a
      // capture because this indicates that the capture is undefined,
      // regardless of the value in the `end` register.
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
    Label begin;
    Label end;

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

    Label body;
    Label end;

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
    // We add `BEGIN_LOOP` and `END_LOOP` instructions because these
    // optional repetitions of the body cannot match the empty string.

    Label end;
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
    // We add `BEGIN_LOOP` and `END_LOOP` instructions because these
    // optional repetitions of the body cannot match the empty string.

    Label end;
    for (int i = 0; i != max_repetition_num; ++i) {
      Label body;
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
  // from the following ones as it is allowed to match the empty string.
  // This is compiled by repeating <body>, but it can result in a bytecode
  // that grows quadratically with the size of the regex when nesting pluses
  // or repetition upper-bounded with infinity.
  //
  // In the particular case where <body> cannot match the empty string, the
  // plus can be compiled without duplicating the bytecode of <body>,
  // resulting in a bytecode linear in the size of the regex in case of
  // nested non-nullable pluses.
  //
  // E.g. `/.+/` will compile `/./` once, while `/(?:.?)+/` will be compiled
  // as
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
    Label begin, end;

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
    Label begin;

    assembler_.Bind(begin);
    emit_body();

    assembler_.Fork(begin);
  }

  void* VisitQuantifier(RegExpQuantifier* node, void*) override {
    // If the quantifier must match nothing, we do not produce its body, but
    // still need the `SET_QUANTIFIER_TO_CLOCK` for the Nfa to be able to
    // correctly determine the number of quantifiers.
    if (v8_flags.experimental_regexp_engine_capture_group_opt &&
        node->max() == 0) {
      if (!node->CaptureRegisters().is_empty()) {
        assembler_.SetQuantifierToClock(RemapQuantifier(node->index()));
      }

      return nullptr;
    }

    // Emit the body, but clear registers occurring in body first.
    //
    // TODO(mbid,v8:10765): It's not always necessary to a) capture
    // registers and b) clear them. For example, we don't have to capture
    // anything for the first 4 repetitions if node->min() >= 5, and then we
    // don't have to clear registers in the first node->min() repetitions.
    // Later, and if node->min() == 0, we don't have to clear registers
    // before the first optional repetition.
    Interval body_registers = node->body()->CaptureRegisters();
    auto emit_body = [&]() {
      if (v8_flags.experimental_regexp_engine_capture_group_opt) {
        assembler_.SetQuantifierToClock(RemapQuantifier(node->index()));
      } else {
        ClearRegisters(body_registers);
      }

      node->body()->Accept(this, nullptr);
    };

    bool can_be_reduced_to_non_nullable_plus =
        node->min() > 0 && node->max() == RegExpTree::kInfinity &&
        node->min_match() > 0;

    if (can_be_reduced_to_non_nullable_plus) {
      // Compile <body>+ with an optimization allowing linear sized bytecode
      // in the case of nested pluses. Repetitions with infinite upperbound
      // like <body>{n,}, with n != 0, are compiled into <body>{n-1}<body+>,
      // avoiding one repetition, compared to <body>{n}<body>*.

      // Compile the mandatory repetitions. We repeat `min() - 1` times,
      // such that the last repetition, compiled later, can be reused in a
      // loop.
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
      // Compile <body>+ into <body><body>*, and <body>{n,}, with n != 0,
      // into <body>{n}<body>*.

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
    if (ignore_captures_) {
      // Skips the `SET_REGISTER_TO_CP` instructions.
      node->body()->Accept(this, nullptr);
    } else {
      int index = node->index();
      int start_register = RegExpCapture::StartRegister(index);
      int end_register = RegExpCapture::EndRegister(index);

      assembler_.SetRegisterToCp(reverse_ ? end_register : start_register);
      node->body()->Accept(this, nullptr);
      assembler_.SetRegisterToCp(reverse_ ? start_register : end_register);
    }

    return nullptr;
  }

  void* VisitGroup(RegExpGroup* node, void*) override {
    node->body()->Accept(this, nullptr);
    return nullptr;
  }

  void* VisitLookaround(RegExpLookaround* node, void*) override {
    assembler_.ReadLookaroundTable(node->index(), node->is_positive());

    // Add the lookbehind to the queue of lookbehinds to be compiled.
    if (!ignore_lookarounds_) {
      lookarounds_.push_back(node);
    }

    return nullptr;
  }

  void* VisitBackReference(RegExpBackReference* node, void*) override {
    UNREACHABLE();
  }

  void* VisitEmpty(RegExpEmpty* node, void*) override { return nullptr; }

  void* VisitText(RegExpText* node, void*) override {
    if (!reverse_) {
      for (TextElement& text_el : *node->elements()) {
        text_el.tree()->Accept(this, nullptr);
      }
    } else {
      for (int i = node->elements()->length() - 1; i >= 0; --i) {
        node->elements()->at(i).tree()->Accept(this, nullptr);
      }
    }
    return nullptr;
  }

  int RemapQuantifier(int id) {
    DCHECK(v8_flags.experimental_regexp_engine_capture_group_opt);
    DCHECK(quantifier_id_remapping_.has_value());
    auto& map = quantifier_id_remapping_.value();

    if (!map.contains(id)) {
      map[id] = static_cast<int>(map.size());
    }

    return map[id];
  }

 private:
  Zone* zone_;

  // Stores the AST of the lookbehinds encountered in a queue. They are
  // compiled after the main expression, in breadth-first order.
  ZoneLinkedList<RegExpLookaround*> lookarounds_;

  std::optional<ZoneMap<int, int>> quantifier_id_remapping_;

  BytecodeAssembler assembler_;
  bool reverse_;

  // Do not produce `SET_REGISTER_TO_CP` instructions. Used when compiling
  // the lookarounds' matching automata, since the capture results will be never
  // be used.
  bool ignore_captures_;

  // Do not produce `READ_LOOKAROUND_TABLE` instructions. Used when compiling
  // the lookarounds' capturing automata, since the string is known to match the
  // expression.
  bool ignore_lookarounds_;
};

}  // namespace

ZoneList<RegExpInstruction> ExperimentalRegExpCompiler::Compile(
    RegExpTree* tree, RegExpFlags flags, Zone* zone) {
  return CompileVisitor::Compile(tree, flags, zone);
}

}  // namespace internal
}  // namespace v8
