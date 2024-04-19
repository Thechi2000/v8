// Copyright 2024 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_WASM_WASM_DEOPT_DATA_H_
#define V8_WASM_WASM_DEOPT_DATA_H_
#if !V8_ENABLE_WEBASSEMBLY
#error This header should only be included if WebAssembly is enabled.
#endif  // !V8_ENABLE_WEBASSEMBLY

#include "src/base/memory.h"
#include "src/utils/utils.h"
#include "src/zone/zone-containers.h"

namespace v8::internal::compiler {
class DeoptimizationLiteral;
}

namespace v8::internal::wasm {

// The "header" of the full deopt data for an optimized wasm function containing
// overall counts used to access the unerlying translated values, literals etc.
struct WasmDeoptData {
  uint32_t entry_count = 0;  // Count of deopt points.
  uint32_t translation_array_size = 0;
  uint32_t deopt_literals_size = 0;
  // The offset inside the code to the first deopt builtin call instruction.
  // This is used to map a pc back to a the "deopt index".
  int deopt_exit_start_offset = 0;
  // The count of eager deopt points.
  int eager_deopt_count = 0;
};

struct WasmDeoptEntry {
  // The wire bytes offset of the deopt point. This is used to map a deopt entry
  // to a liftoff deopt point.
  BytecodeOffset bytecode_offset = BytecodeOffset::None();
  // The index inside the translations array at which this deopt entry starts.
  // (The translations array is shared for all deopt points of a function.)
  int translation_index = -1;
};

// A view to access the deopt data stored in the WasmCode's metadata as raw
// bytes.
class WasmDeoptView {
 public:
  explicit WasmDeoptView(base::Vector<const uint8_t> deopt_data)
      : deopt_data_(deopt_data) {
    if (!deopt_data.empty()) {
      static_assert(std::is_trivially_copy_assignable_v<WasmDeoptData>);
      DCHECK_GE(deopt_data_.size(), sizeof(WasmDeoptData));
      std::memcpy(&base_data_, deopt_data_.begin(), sizeof(base_data_));
    }
  }

  bool HasDeoptData() const { return !deopt_data_.empty(); }

  const WasmDeoptData& GetDeoptData() const {
    DCHECK(HasDeoptData());
    return base_data_;
  }

  base::Vector<const uint8_t> GetTranslationsArray() const {
    DCHECK(HasDeoptData());
    return {deopt_data_.begin() + sizeof(base_data_),
            base_data_.translation_array_size};
  }

  WasmDeoptEntry GetDeoptEntry(uint32_t deopt_index) const {
    DCHECK(HasDeoptData());
    DCHECK(deopt_index < base_data_.entry_count);
    const uint8_t* begin = deopt_data_.begin() + sizeof(base_data_) +
                           base_data_.translation_array_size;
    return base::ReadUnalignedValue<WasmDeoptEntry>(reinterpret_cast<Address>(
        begin + sizeof(WasmDeoptEntry) * deopt_index));
  }

  std::vector<compiler::DeoptimizationLiteral>
  BuildDeoptimizationLiteralArray();

 private:
  base::Vector<const uint8_t> deopt_data_;
  WasmDeoptData base_data_;
};

class WasmDeoptDataProcessor {
 public:
  static base::OwnedVector<uint8_t> Serialize(
      int deopt_exit_start_offset, int eager_deopt_count,
      base::Vector<const uint8_t> translation_array,
      base::Vector<wasm::WasmDeoptEntry> deopt_entries,
      const ZoneDeque<compiler::DeoptimizationLiteral>& deopt_literals);
};

}  // namespace v8::internal::wasm
#endif  // V8_WASM_WASM_DEOPT_DATA_H_
