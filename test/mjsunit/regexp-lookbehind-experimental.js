// Copyright 2009 the V8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Flags: --allow-natives-syntax --default-to-experimental-regexp-engine --experimental_regexp_engine_capture_group_opt

// Tests captures in positive and negative look-ahead in regular expressions.

function stringEscape(string) {
  // Converts string to source literal.
  return '"' + string.replace(/["\\]/g, "\\$1") + '"';
}

function testRE(re, input, expected_result) {
  assertEquals(%RegexpTypeTag(re), "EXPERIMENTAL");

  var testName = re + ".test(" + stringEscape(input) + ")";
  if (expected_result) {
    assertTrue(re.test(input), testName);
  } else {
    assertFalse(re.test(input), testName);
  }
}

function execRE(re, input, expected_result) {
  assertEquals(%RegexpTypeTag(re), "EXPERIMENTAL");

  var testName = re + ".exec('" + stringEscape(input) + "')";
  assertEquals(expected_result, re.exec(input), testName);
}

// Test of simple positive lookahead.

var re = /(?<=a)$/;
testRE(re, "a", true);
testRE(re, "b", false);
execRE(re, "a", [""]);

re = /\wf(?<=oo\w)$/;
testRE(re, "oof", true);
testRE(re, "oob", false);
testRE(re, "oaf", false);
testRE(re, "aof", false);
execRE(re, "oof", ["of"]);

re = /(?<=\W).(?<=\w)/;
testRE(re, " !a.", true);
testRE(re, " !.", false);
testRE(re, " !ba.", true);
execRE(re, " !ba.", ["b"]);

re = /..(?<=.(?<=o[^f])f)/;
testRE(re, "!oof ,", true);
testRE(re, "!of ,", false);
testRE(re, "off ,", false);
execRE(re, "!oof ,", ["of"]);

re = /^(?:a(?<=(.))|b|c)$/;
testRE(re, "a", true);
testRE(re, "b", true);
testRE(re, "c", true);
testRE(re, "d", false);
execRE(re, "a", ["a", "a"]);
execRE(re, "b", ["b", undefined]);
execRE(re, "c", ["c", undefined]);

execRE(/b(?<=(b))$/, "b", ["b", "b"]);
execRE(/^(?:(?<=(b))|a)b/, "ab", ["ab", undefined]);
execRE(/^bd(?:(?<=(b)(?:(?<=(c))|d))|)/, "bd", ["bd", "b", undefined]);

// Test of Negative Look-Ahead.

re = /.(?<!x)/;
testRE(re, "y", true);
testRE(re, "x", false);
execRE(re, "y", ["y"]);

// Test mixed nested look-ahead with captures.

re = /(?<=(?<=(x))(y))$/;
testRE(re, "xy", true);
testRE(re, "xz", false);
execRE(re, "xy", ["", "x", "y"]);

re = /(?<!(?<!(x))(y))$/;
testRE(re, "xy", true);
testRE(re, "yy", false);
execRE(re, "xy", ["", undefined, undefined]);

re = /(?<=(?<!(x))(y))$/;
testRE(re, "yy", true);
testRE(re, "xy", false);
execRE(re, "yy", ["", undefined, "y"]);

re = /^(?!(x)(?=(y)))/;
testRE(re, "xz", true);
testRE(re, "xy", false);
execRE(re, "xz", ["", undefined, undefined]);

re = /(?<=(?<!(?<=(x))(y))(z))$/;
testRE(re, "xaz", true);
testRE(re, "ayz", true);
testRE(re, "xyz", false);
testRE(re, "a", false);
execRE(re, "xaz", ["", undefined, undefined, "z"]);
execRE(re, "ayz", ["", undefined, undefined, "z"]);

re = /^(?!(x)(?=(y)(?!(z))))/;
re = /(?<!(?<=(?<!(x))(y))(z))$/;
testRE(re, "a", true);
testRE(re, "xa", true);
testRE(re, "xyz", true);
testRE(re, "ayz", false);
execRE(re, "a", ["", undefined, undefined, undefined]);
execRE(re, "xa", ["", undefined, undefined, undefined]);
execRE(re, "xyz", ["", undefined, undefined, undefined]);
