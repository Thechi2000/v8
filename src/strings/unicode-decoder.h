// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_STRINGS_UNICODE_DECODER_H_
#define V8_STRINGS_UNICODE_DECODER_H_

#include "src/base/vector.h"
#include "src/strings/unicode.h"

#if V8_ENABLE_WEBASSEMBLY
#include "src/third_party/utf8-decoder/generalized-utf8-decoder.h"
#endif

namespace v8 {
namespace internal {

// The return value may point to the first aligned word containing the first
// non-one-byte character, rather than directly to the non-one-byte character.
// If the return value is >= the passed length, the entire string was
// one-byte.
inline int NonAsciiStart(const uint8_t* chars, int length) {
  const uint8_t* start = chars;
  const uint8_t* limit = chars + length;

  if (static_cast<size_t>(length) >= kIntptrSize) {
    // Check unaligned bytes.
    while (!IsAligned(reinterpret_cast<intptr_t>(chars), kIntptrSize)) {
      if (*chars > unibrow::Utf8::kMaxOneByteChar) {
        return static_cast<int>(chars - start);
      }
      ++chars;
    }
    // Check aligned words.
    DCHECK_EQ(unibrow::Utf8::kMaxOneByteChar, 0x7F);
    const uintptr_t non_one_byte_mask = kUintptrAllBitsSet / 0xFF * 0x80;
    while (chars + sizeof(uintptr_t) <= limit) {
      if (*reinterpret_cast<const uintptr_t*>(chars) & non_one_byte_mask) {
        return static_cast<int>(chars - start);
      }
      chars += sizeof(uintptr_t);
    }
  }
  // Check remaining unaligned bytes.
  while (chars < limit) {
    if (*chars > unibrow::Utf8::kMaxOneByteChar) {
      return static_cast<int>(chars - start);
    }
    ++chars;
  }

  return static_cast<int>(chars - start);
}

template <class Decoder>
class Utf8DecoderBase {
 public:
  enum class Encoding : uint8_t { kAscii, kLatin1, kUtf16, kInvalid };

  bool is_invalid() const {
    return static_cast<const Decoder&>(*this).is_invalid();
  }
  bool is_ascii() const { return encoding_ == Encoding::kAscii; }
  bool is_one_byte() const { return encoding_ <= Encoding::kLatin1; }
  int utf16_length() const {
    DCHECK(!is_invalid());
    return utf16_length_;
  }
  int non_ascii_start() const {
    DCHECK(!is_invalid());
    return non_ascii_start_;
  }

  template <typename Char>
  void Decode(Char* out, const base::Vector<const uint8_t>& data);

 protected:
  explicit Utf8DecoderBase(const base::Vector<const uint8_t>& data);
  Encoding encoding_;
  int non_ascii_start_;
  int utf16_length_;
};

class V8_EXPORT_PRIVATE Utf8Decoder final
    : public Utf8DecoderBase<Utf8Decoder> {
 public:
  static bool InvalidCodePointSequence(uint32_t current, uint32_t previous) {
    // The DfaDecoder will only ever decode Unicode scalar values, and all
    // sequences of USVs are valid.
    DCHECK(!unibrow::Utf16::IsLeadSurrogate(current));
    DCHECK(!unibrow::Utf16::IsTrailSurrogate(current));
    return false;
  }
  static const bool kAllowIncompleteSequences = true;
  using DfaDecoder = Utf8DfaDecoder;

  explicit Utf8Decoder(const base::Vector<const uint8_t>& data)
      : Utf8DecoderBase(data) {}

  // This decoder never fails; an invalid byte sequence decodes to U+FFFD and
  // then the decode continues.
  bool is_invalid() const {
    DCHECK_NE(encoding_, Encoding::kInvalid);
    return false;
  }
};

#if V8_ENABLE_WEBASSEMBLY
// Like Utf8Decoder above, except that instead of replacing invalid sequences
// with U+FFFD, we have a separate Encoding::kInvalid state.
class Wtf8Decoder : public Utf8DecoderBase<Wtf8Decoder> {
 public:
  static bool InvalidCodePointSequence(uint32_t current, uint32_t previous) {
    return unibrow::Utf16::IsSurrogatePair(current, previous);
  }
  static const bool kAllowIncompleteSequences = false;
  using DfaDecoder = GeneralizedUtf8DfaDecoder;

  explicit Wtf8Decoder(const base::Vector<const uint8_t>& data)
      : Utf8DecoderBase(data) {}

  bool is_invalid() const { return encoding_ == Encoding::kInvalid; }
};
#endif  // V8_ENABLE_WEBASSEMBLY

}  // namespace internal
}  // namespace v8

#endif  // V8_STRINGS_UNICODE_DECODER_H_
