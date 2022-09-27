#!/usr/bin/env python3
# Copyright 2019 the V8 project authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can
# be found in the LICENSE file.

# This file is automatically generated by mkgrokdump and should not
# be modified manually.

# List of known V8 instance types.
# yapf: disable

INSTANCE_TYPES = {
  0: "INTERNALIZED_STRING_TYPE",
  2: "EXTERNAL_INTERNALIZED_STRING_TYPE",
  8: "ONE_BYTE_INTERNALIZED_STRING_TYPE",
  10: "EXTERNAL_ONE_BYTE_INTERNALIZED_STRING_TYPE",
  18: "UNCACHED_EXTERNAL_INTERNALIZED_STRING_TYPE",
  26: "UNCACHED_EXTERNAL_ONE_BYTE_INTERNALIZED_STRING_TYPE",
  32: "STRING_TYPE",
  33: "CONS_STRING_TYPE",
  34: "EXTERNAL_STRING_TYPE",
  35: "SLICED_STRING_TYPE",
  37: "THIN_STRING_TYPE",
  40: "ONE_BYTE_STRING_TYPE",
  41: "CONS_ONE_BYTE_STRING_TYPE",
  42: "EXTERNAL_ONE_BYTE_STRING_TYPE",
  43: "SLICED_ONE_BYTE_STRING_TYPE",
  45: "THIN_ONE_BYTE_STRING_TYPE",
  50: "UNCACHED_EXTERNAL_STRING_TYPE",
  58: "UNCACHED_EXTERNAL_ONE_BYTE_STRING_TYPE",
  96: "SHARED_STRING_TYPE",
  98: "SHARED_EXTERNAL_STRING_TYPE",
  101: "SHARED_THIN_STRING_TYPE",
  104: "SHARED_ONE_BYTE_STRING_TYPE",
  106: "SHARED_EXTERNAL_ONE_BYTE_STRING_TYPE",
  109: "SHARED_THIN_ONE_BYTE_STRING_TYPE",
  114: "SHARED_UNCACHED_EXTERNAL_STRING_TYPE",
  122: "SHARED_UNCACHED_EXTERNAL_ONE_BYTE_STRING_TYPE",
  128: "SYMBOL_TYPE",
  129: "BIG_INT_BASE_TYPE",
  130: "HEAP_NUMBER_TYPE",
  131: "ODDBALL_TYPE",
  132: "PROMISE_FULFILL_REACTION_JOB_TASK_TYPE",
  133: "PROMISE_REJECT_REACTION_JOB_TASK_TYPE",
  134: "CALLABLE_TASK_TYPE",
  135: "CALLBACK_TASK_TYPE",
  136: "PROMISE_RESOLVE_THENABLE_JOB_TASK_TYPE",
  137: "LOAD_HANDLER_TYPE",
  138: "STORE_HANDLER_TYPE",
  139: "FUNCTION_TEMPLATE_INFO_TYPE",
  140: "OBJECT_TEMPLATE_INFO_TYPE",
  141: "ACCESS_CHECK_INFO_TYPE",
  142: "ACCESSOR_PAIR_TYPE",
  143: "ALIASED_ARGUMENTS_ENTRY_TYPE",
  144: "ALLOCATION_MEMENTO_TYPE",
  145: "ALLOCATION_SITE_TYPE",
  146: "ARRAY_BOILERPLATE_DESCRIPTION_TYPE",
  147: "ASM_WASM_DATA_TYPE",
  148: "ASYNC_GENERATOR_REQUEST_TYPE",
  149: "BREAK_POINT_TYPE",
  150: "BREAK_POINT_INFO_TYPE",
  151: "CALL_SITE_INFO_TYPE",
  152: "CLASS_POSITIONS_TYPE",
  153: "DEBUG_INFO_TYPE",
  154: "ENUM_CACHE_TYPE",
  155: "ERROR_STACK_DATA_TYPE",
  156: "FEEDBACK_CELL_TYPE",
  157: "FUNCTION_TEMPLATE_RARE_DATA_TYPE",
  158: "INTERCEPTOR_INFO_TYPE",
  159: "INTERPRETER_DATA_TYPE",
  160: "MODULE_REQUEST_TYPE",
  161: "PROMISE_CAPABILITY_TYPE",
  162: "PROMISE_ON_STACK_TYPE",
  163: "PROMISE_REACTION_TYPE",
  164: "PROPERTY_DESCRIPTOR_OBJECT_TYPE",
  165: "PROTOTYPE_INFO_TYPE",
  166: "REG_EXP_BOILERPLATE_DESCRIPTION_TYPE",
  167: "SCRIPT_TYPE",
  168: "SCRIPT_OR_MODULE_TYPE",
  169: "SOURCE_TEXT_MODULE_INFO_ENTRY_TYPE",
  170: "STACK_FRAME_INFO_TYPE",
  171: "TEMPLATE_OBJECT_DESCRIPTION_TYPE",
  172: "TUPLE2_TYPE",
  173: "WASM_EXCEPTION_TAG_TYPE",
  174: "WASM_INDIRECT_FUNCTION_TABLE_TYPE",
  175: "FIXED_ARRAY_TYPE",
  176: "HASH_TABLE_TYPE",
  177: "EPHEMERON_HASH_TABLE_TYPE",
  178: "GLOBAL_DICTIONARY_TYPE",
  179: "NAME_DICTIONARY_TYPE",
  180: "NAME_TO_INDEX_HASH_TABLE_TYPE",
  181: "NUMBER_DICTIONARY_TYPE",
  182: "ORDERED_HASH_MAP_TYPE",
  183: "ORDERED_HASH_SET_TYPE",
  184: "ORDERED_NAME_DICTIONARY_TYPE",
  185: "REGISTERED_SYMBOL_TABLE_TYPE",
  186: "SIMPLE_NUMBER_DICTIONARY_TYPE",
  187: "CLOSURE_FEEDBACK_CELL_ARRAY_TYPE",
  188: "OBJECT_BOILERPLATE_DESCRIPTION_TYPE",
  189: "SCRIPT_CONTEXT_TABLE_TYPE",
  190: "BYTE_ARRAY_TYPE",
  191: "BYTECODE_ARRAY_TYPE",
  192: "FIXED_DOUBLE_ARRAY_TYPE",
  193: "INTERNAL_CLASS_WITH_SMI_ELEMENTS_TYPE",
  194: "SLOPPY_ARGUMENTS_ELEMENTS_TYPE",
  195: "TURBOFAN_BITSET_TYPE_TYPE",
  196: "TURBOFAN_HEAP_CONSTANT_TYPE_TYPE",
  197: "TURBOFAN_OTHER_NUMBER_CONSTANT_TYPE_TYPE",
  198: "TURBOFAN_RANGE_TYPE_TYPE",
  199: "TURBOFAN_UNION_TYPE_TYPE",
  200: "UNCOMPILED_DATA_WITH_PREPARSE_DATA_TYPE",
  201: "UNCOMPILED_DATA_WITH_PREPARSE_DATA_AND_JOB_TYPE",
  202: "UNCOMPILED_DATA_WITHOUT_PREPARSE_DATA_TYPE",
  203: "UNCOMPILED_DATA_WITHOUT_PREPARSE_DATA_WITH_JOB_TYPE",
  204: "FOREIGN_TYPE",
  205: "AWAIT_CONTEXT_TYPE",
  206: "BLOCK_CONTEXT_TYPE",
  207: "CATCH_CONTEXT_TYPE",
  208: "DEBUG_EVALUATE_CONTEXT_TYPE",
  209: "EVAL_CONTEXT_TYPE",
  210: "FUNCTION_CONTEXT_TYPE",
  211: "MODULE_CONTEXT_TYPE",
  212: "NATIVE_CONTEXT_TYPE",
  213: "SCRIPT_CONTEXT_TYPE",
  214: "WITH_CONTEXT_TYPE",
  215: "WASM_FUNCTION_DATA_TYPE",
  216: "WASM_CAPI_FUNCTION_DATA_TYPE",
  217: "WASM_EXPORTED_FUNCTION_DATA_TYPE",
  218: "WASM_JS_FUNCTION_DATA_TYPE",
  219: "EXPORTED_SUB_CLASS_BASE_TYPE",
  220: "EXPORTED_SUB_CLASS_TYPE",
  221: "EXPORTED_SUB_CLASS2_TYPE",
  222: "SMALL_ORDERED_HASH_MAP_TYPE",
  223: "SMALL_ORDERED_HASH_SET_TYPE",
  224: "SMALL_ORDERED_NAME_DICTIONARY_TYPE",
  225: "ABSTRACT_INTERNAL_CLASS_SUBCLASS1_TYPE",
  226: "ABSTRACT_INTERNAL_CLASS_SUBCLASS2_TYPE",
  227: "DESCRIPTOR_ARRAY_TYPE",
  228: "STRONG_DESCRIPTOR_ARRAY_TYPE",
  229: "SOURCE_TEXT_MODULE_TYPE",
  230: "SYNTHETIC_MODULE_TYPE",
  231: "WEAK_FIXED_ARRAY_TYPE",
  232: "TRANSITION_ARRAY_TYPE",
  233: "ACCESSOR_INFO_TYPE",
  234: "CACHED_TEMPLATE_OBJECT_TYPE",
  235: "CALL_HANDLER_INFO_TYPE",
  236: "CELL_TYPE",
  237: "CODE_TYPE",
  238: "CODE_DATA_CONTAINER_TYPE",
  239: "COVERAGE_INFO_TYPE",
  240: "EMBEDDER_DATA_ARRAY_TYPE",
  241: "FEEDBACK_METADATA_TYPE",
  242: "FEEDBACK_VECTOR_TYPE",
  243: "FILLER_TYPE",
  244: "FREE_SPACE_TYPE",
  245: "INTERNAL_CLASS_TYPE",
  246: "INTERNAL_CLASS_WITH_STRUCT_ELEMENTS_TYPE",
  247: "MAP_TYPE",
  248: "MEGA_DOM_HANDLER_TYPE",
  249: "ON_HEAP_BASIC_BLOCK_PROFILER_DATA_TYPE",
  250: "PREPARSE_DATA_TYPE",
  251: "PROPERTY_ARRAY_TYPE",
  252: "PROPERTY_CELL_TYPE",
  253: "SCOPE_INFO_TYPE",
  254: "SHARED_FUNCTION_INFO_TYPE",
  255: "SMI_BOX_TYPE",
  256: "SMI_PAIR_TYPE",
  257: "SORT_STATE_TYPE",
  258: "SWISS_NAME_DICTIONARY_TYPE",
  259: "WASM_API_FUNCTION_REF_TYPE",
  260: "WASM_CONTINUATION_OBJECT_TYPE",
  261: "WASM_INTERNAL_FUNCTION_TYPE",
  262: "WASM_RESUME_DATA_TYPE",
  263: "WASM_STRING_VIEW_ITER_TYPE",
  264: "WASM_TYPE_INFO_TYPE",
  265: "WEAK_ARRAY_LIST_TYPE",
  266: "WEAK_CELL_TYPE",
  267: "WASM_ARRAY_TYPE",
  268: "WASM_STRUCT_TYPE",
  269: "JS_PROXY_TYPE",
  1057: "JS_OBJECT_TYPE",
  270: "JS_GLOBAL_OBJECT_TYPE",
  271: "JS_GLOBAL_PROXY_TYPE",
  272: "JS_MODULE_NAMESPACE_TYPE",
  1040: "JS_SPECIAL_API_OBJECT_TYPE",
  1041: "JS_PRIMITIVE_WRAPPER_TYPE",
  1058: "JS_API_OBJECT_TYPE",
  2058: "JS_LAST_DUMMY_API_OBJECT_TYPE",
  2059: "JS_DATA_VIEW_TYPE",
  2060: "JS_TYPED_ARRAY_TYPE",
  2061: "JS_ARRAY_BUFFER_TYPE",
  2062: "JS_PROMISE_TYPE",
  2063: "JS_BOUND_FUNCTION_TYPE",
  2064: "JS_WRAPPED_FUNCTION_TYPE",
  2065: "JS_FUNCTION_TYPE",
  2066: "BIGINT64_TYPED_ARRAY_CONSTRUCTOR_TYPE",
  2067: "BIGUINT64_TYPED_ARRAY_CONSTRUCTOR_TYPE",
  2068: "FLOAT32_TYPED_ARRAY_CONSTRUCTOR_TYPE",
  2069: "FLOAT64_TYPED_ARRAY_CONSTRUCTOR_TYPE",
  2070: "INT16_TYPED_ARRAY_CONSTRUCTOR_TYPE",
  2071: "INT32_TYPED_ARRAY_CONSTRUCTOR_TYPE",
  2072: "INT8_TYPED_ARRAY_CONSTRUCTOR_TYPE",
  2073: "UINT16_TYPED_ARRAY_CONSTRUCTOR_TYPE",
  2074: "UINT32_TYPED_ARRAY_CONSTRUCTOR_TYPE",
  2075: "UINT8_CLAMPED_TYPED_ARRAY_CONSTRUCTOR_TYPE",
  2076: "UINT8_TYPED_ARRAY_CONSTRUCTOR_TYPE",
  2077: "JS_ARRAY_CONSTRUCTOR_TYPE",
  2078: "JS_PROMISE_CONSTRUCTOR_TYPE",
  2079: "JS_REG_EXP_CONSTRUCTOR_TYPE",
  2080: "JS_CLASS_CONSTRUCTOR_TYPE",
  2081: "JS_ARRAY_ITERATOR_PROTOTYPE_TYPE",
  2082: "JS_ITERATOR_PROTOTYPE_TYPE",
  2083: "JS_MAP_ITERATOR_PROTOTYPE_TYPE",
  2084: "JS_OBJECT_PROTOTYPE_TYPE",
  2085: "JS_PROMISE_PROTOTYPE_TYPE",
  2086: "JS_REG_EXP_PROTOTYPE_TYPE",
  2087: "JS_SET_ITERATOR_PROTOTYPE_TYPE",
  2088: "JS_SET_PROTOTYPE_TYPE",
  2089: "JS_STRING_ITERATOR_PROTOTYPE_TYPE",
  2090: "JS_TYPED_ARRAY_PROTOTYPE_TYPE",
  2091: "JS_MAP_KEY_ITERATOR_TYPE",
  2092: "JS_MAP_KEY_VALUE_ITERATOR_TYPE",
  2093: "JS_MAP_VALUE_ITERATOR_TYPE",
  2094: "JS_SET_KEY_VALUE_ITERATOR_TYPE",
  2095: "JS_SET_VALUE_ITERATOR_TYPE",
  2096: "JS_GENERATOR_OBJECT_TYPE",
  2097: "JS_ASYNC_FUNCTION_OBJECT_TYPE",
  2098: "JS_ASYNC_GENERATOR_OBJECT_TYPE",
  2099: "JS_MAP_TYPE",
  2100: "JS_SET_TYPE",
  2101: "JS_ATOMICS_CONDITION_TYPE",
  2102: "JS_ATOMICS_MUTEX_TYPE",
  2103: "JS_WEAK_MAP_TYPE",
  2104: "JS_WEAK_SET_TYPE",
  2105: "JS_ARGUMENTS_OBJECT_TYPE",
  2106: "JS_ARRAY_TYPE",
  2107: "JS_ARRAY_ITERATOR_TYPE",
  2108: "JS_ASYNC_FROM_SYNC_ITERATOR_TYPE",
  2109: "JS_COLLATOR_TYPE",
  2110: "JS_CONTEXT_EXTENSION_OBJECT_TYPE",
  2111: "JS_DATE_TYPE",
  2112: "JS_DATE_TIME_FORMAT_TYPE",
  2113: "JS_DISPLAY_NAMES_TYPE",
  2114: "JS_ERROR_TYPE",
  2115: "JS_EXTERNAL_OBJECT_TYPE",
  2116: "JS_FINALIZATION_REGISTRY_TYPE",
  2117: "JS_LIST_FORMAT_TYPE",
  2118: "JS_LOCALE_TYPE",
  2119: "JS_MESSAGE_OBJECT_TYPE",
  2120: "JS_NUMBER_FORMAT_TYPE",
  2121: "JS_PLURAL_RULES_TYPE",
  2122: "JS_REG_EXP_TYPE",
  2123: "JS_REG_EXP_STRING_ITERATOR_TYPE",
  2124: "JS_RELATIVE_TIME_FORMAT_TYPE",
  2125: "JS_SEGMENT_ITERATOR_TYPE",
  2126: "JS_SEGMENTER_TYPE",
  2127: "JS_SEGMENTS_TYPE",
  2128: "JS_SHADOW_REALM_TYPE",
  2129: "JS_SHARED_ARRAY_TYPE",
  2130: "JS_SHARED_STRUCT_TYPE",
  2131: "JS_STRING_ITERATOR_TYPE",
  2132: "JS_TEMPORAL_CALENDAR_TYPE",
  2133: "JS_TEMPORAL_DURATION_TYPE",
  2134: "JS_TEMPORAL_INSTANT_TYPE",
  2135: "JS_TEMPORAL_PLAIN_DATE_TYPE",
  2136: "JS_TEMPORAL_PLAIN_DATE_TIME_TYPE",
  2137: "JS_TEMPORAL_PLAIN_MONTH_DAY_TYPE",
  2138: "JS_TEMPORAL_PLAIN_TIME_TYPE",
  2139: "JS_TEMPORAL_PLAIN_YEAR_MONTH_TYPE",
  2140: "JS_TEMPORAL_TIME_ZONE_TYPE",
  2141: "JS_TEMPORAL_ZONED_DATE_TIME_TYPE",
  2142: "JS_V8_BREAK_ITERATOR_TYPE",
  2143: "JS_WEAK_REF_TYPE",
  2144: "WASM_EXCEPTION_PACKAGE_TYPE",
  2145: "WASM_GLOBAL_OBJECT_TYPE",
  2146: "WASM_INSTANCE_OBJECT_TYPE",
  2147: "WASM_MEMORY_OBJECT_TYPE",
  2148: "WASM_MODULE_OBJECT_TYPE",
  2149: "WASM_SUSPENDER_OBJECT_TYPE",
  2150: "WASM_TABLE_OBJECT_TYPE",
  2151: "WASM_TAG_OBJECT_TYPE",
  2152: "WASM_VALUE_OBJECT_TYPE",
}

# List of known V8 maps.
KNOWN_MAPS = {
    ("read_only_space", 0x02139): (247, "MetaMap"),
    ("read_only_space", 0x02161): (131, "NullMap"),
    ("read_only_space", 0x02189): (228, "StrongDescriptorArrayMap"),
    ("read_only_space", 0x021b1): (265, "WeakArrayListMap"),
    ("read_only_space", 0x021f5): (154, "EnumCacheMap"),
    ("read_only_space", 0x02229): (175, "FixedArrayMap"),
    ("read_only_space", 0x02275): (8, "OneByteInternalizedStringMap"),
    ("read_only_space", 0x022c1): (244, "FreeSpaceMap"),
    ("read_only_space", 0x022e9): (243, "OnePointerFillerMap"),
    ("read_only_space", 0x02311): (243, "TwoPointerFillerMap"),
    ("read_only_space", 0x02339): (131, "UninitializedMap"),
    ("read_only_space", 0x023b1): (131, "UndefinedMap"),
    ("read_only_space", 0x023f5): (130, "HeapNumberMap"),
    ("read_only_space", 0x02429): (131, "TheHoleMap"),
    ("read_only_space", 0x02489): (131, "BooleanMap"),
    ("read_only_space", 0x0252d): (190, "ByteArrayMap"),
    ("read_only_space", 0x02555): (175, "FixedCOWArrayMap"),
    ("read_only_space", 0x0257d): (176, "HashTableMap"),
    ("read_only_space", 0x025a5): (128, "SymbolMap"),
    ("read_only_space", 0x025cd): (40, "OneByteStringMap"),
    ("read_only_space", 0x025f5): (253, "ScopeInfoMap"),
    ("read_only_space", 0x0261d): (254, "SharedFunctionInfoMap"),
    ("read_only_space", 0x02645): (237, "CodeMap"),
    ("read_only_space", 0x0266d): (236, "CellMap"),
    ("read_only_space", 0x02695): (252, "GlobalPropertyCellMap"),
    ("read_only_space", 0x026bd): (204, "ForeignMap"),
    ("read_only_space", 0x026e5): (232, "TransitionArrayMap"),
    ("read_only_space", 0x0270d): (45, "ThinOneByteStringMap"),
    ("read_only_space", 0x02735): (242, "FeedbackVectorMap"),
    ("read_only_space", 0x0276d): (131, "ArgumentsMarkerMap"),
    ("read_only_space", 0x027cd): (131, "ExceptionMap"),
    ("read_only_space", 0x02829): (131, "TerminationExceptionMap"),
    ("read_only_space", 0x02891): (131, "OptimizedOutMap"),
    ("read_only_space", 0x028f1): (131, "StaleRegisterMap"),
    ("read_only_space", 0x02951): (189, "ScriptContextTableMap"),
    ("read_only_space", 0x02979): (187, "ClosureFeedbackCellArrayMap"),
    ("read_only_space", 0x029a1): (241, "FeedbackMetadataArrayMap"),
    ("read_only_space", 0x029c9): (175, "ArrayListMap"),
    ("read_only_space", 0x029f1): (129, "BigIntMap"),
    ("read_only_space", 0x02a19): (188, "ObjectBoilerplateDescriptionMap"),
    ("read_only_space", 0x02a41): (191, "BytecodeArrayMap"),
    ("read_only_space", 0x02a69): (238, "CodeDataContainerMap"),
    ("read_only_space", 0x02a91): (239, "CoverageInfoMap"),
    ("read_only_space", 0x02ab9): (192, "FixedDoubleArrayMap"),
    ("read_only_space", 0x02ae1): (178, "GlobalDictionaryMap"),
    ("read_only_space", 0x02b09): (156, "ManyClosuresCellMap"),
    ("read_only_space", 0x02b31): (248, "MegaDomHandlerMap"),
    ("read_only_space", 0x02b59): (175, "ModuleInfoMap"),
    ("read_only_space", 0x02b81): (179, "NameDictionaryMap"),
    ("read_only_space", 0x02ba9): (156, "NoClosuresCellMap"),
    ("read_only_space", 0x02bd1): (181, "NumberDictionaryMap"),
    ("read_only_space", 0x02bf9): (156, "OneClosureCellMap"),
    ("read_only_space", 0x02c21): (182, "OrderedHashMapMap"),
    ("read_only_space", 0x02c49): (183, "OrderedHashSetMap"),
    ("read_only_space", 0x02c71): (180, "NameToIndexHashTableMap"),
    ("read_only_space", 0x02c99): (185, "RegisteredSymbolTableMap"),
    ("read_only_space", 0x02cc1): (184, "OrderedNameDictionaryMap"),
    ("read_only_space", 0x02ce9): (250, "PreparseDataMap"),
    ("read_only_space", 0x02d11): (251, "PropertyArrayMap"),
    ("read_only_space", 0x02d39): (233, "AccessorInfoMap"),
    ("read_only_space", 0x02d61): (235, "SideEffectCallHandlerInfoMap"),
    ("read_only_space", 0x02d89): (235, "SideEffectFreeCallHandlerInfoMap"),
    ("read_only_space", 0x02db1): (235, "NextCallSideEffectFreeCallHandlerInfoMap"),
    ("read_only_space", 0x02dd9): (186, "SimpleNumberDictionaryMap"),
    ("read_only_space", 0x02e01): (222, "SmallOrderedHashMapMap"),
    ("read_only_space", 0x02e29): (223, "SmallOrderedHashSetMap"),
    ("read_only_space", 0x02e51): (224, "SmallOrderedNameDictionaryMap"),
    ("read_only_space", 0x02e79): (229, "SourceTextModuleMap"),
    ("read_only_space", 0x02ea1): (258, "SwissNameDictionaryMap"),
    ("read_only_space", 0x02ec9): (230, "SyntheticModuleMap"),
    ("read_only_space", 0x02ef1): (259, "WasmApiFunctionRefMap"),
    ("read_only_space", 0x02f19): (216, "WasmCapiFunctionDataMap"),
    ("read_only_space", 0x02f41): (217, "WasmExportedFunctionDataMap"),
    ("read_only_space", 0x02f69): (261, "WasmInternalFunctionMap"),
    ("read_only_space", 0x02f91): (218, "WasmJSFunctionDataMap"),
    ("read_only_space", 0x02fb9): (262, "WasmResumeDataMap"),
    ("read_only_space", 0x02fe1): (264, "WasmTypeInfoMap"),
    ("read_only_space", 0x03009): (260, "WasmContinuationObjectMap"),
    ("read_only_space", 0x03031): (231, "WeakFixedArrayMap"),
    ("read_only_space", 0x03059): (177, "EphemeronHashTableMap"),
    ("read_only_space", 0x03081): (240, "EmbedderDataArrayMap"),
    ("read_only_space", 0x030a9): (266, "WeakCellMap"),
    ("read_only_space", 0x030d1): (32, "StringMap"),
    ("read_only_space", 0x030f9): (41, "ConsOneByteStringMap"),
    ("read_only_space", 0x03121): (33, "ConsStringMap"),
    ("read_only_space", 0x03149): (37, "ThinStringMap"),
    ("read_only_space", 0x03171): (35, "SlicedStringMap"),
    ("read_only_space", 0x03199): (43, "SlicedOneByteStringMap"),
    ("read_only_space", 0x031c1): (34, "ExternalStringMap"),
    ("read_only_space", 0x031e9): (42, "ExternalOneByteStringMap"),
    ("read_only_space", 0x03211): (50, "UncachedExternalStringMap"),
    ("read_only_space", 0x03239): (0, "InternalizedStringMap"),
    ("read_only_space", 0x03261): (2, "ExternalInternalizedStringMap"),
    ("read_only_space", 0x03289): (10, "ExternalOneByteInternalizedStringMap"),
    ("read_only_space", 0x032b1): (18, "UncachedExternalInternalizedStringMap"),
    ("read_only_space", 0x032d9): (26, "UncachedExternalOneByteInternalizedStringMap"),
    ("read_only_space", 0x03301): (58, "UncachedExternalOneByteStringMap"),
    ("read_only_space", 0x03329): (104, "SharedOneByteStringMap"),
    ("read_only_space", 0x03351): (96, "SharedStringMap"),
    ("read_only_space", 0x03379): (106, "SharedExternalOneByteStringMap"),
    ("read_only_space", 0x033a1): (98, "SharedExternalStringMap"),
    ("read_only_space", 0x033c9): (122, "SharedUncachedExternalOneByteStringMap"),
    ("read_only_space", 0x033f1): (114, "SharedUncachedExternalStringMap"),
    ("read_only_space", 0x03419): (109, "SharedThinOneByteStringMap"),
    ("read_only_space", 0x03441): (101, "SharedThinStringMap"),
    ("read_only_space", 0x03469): (131, "SelfReferenceMarkerMap"),
    ("read_only_space", 0x03491): (131, "BasicBlockCountersMarkerMap"),
    ("read_only_space", 0x034d5): (146, "ArrayBoilerplateDescriptionMap"),
    ("read_only_space", 0x035d5): (158, "InterceptorInfoMap"),
    ("read_only_space", 0x07441): (132, "PromiseFulfillReactionJobTaskMap"),
    ("read_only_space", 0x07469): (133, "PromiseRejectReactionJobTaskMap"),
    ("read_only_space", 0x07491): (134, "CallableTaskMap"),
    ("read_only_space", 0x074b9): (135, "CallbackTaskMap"),
    ("read_only_space", 0x074e1): (136, "PromiseResolveThenableJobTaskMap"),
    ("read_only_space", 0x07509): (139, "FunctionTemplateInfoMap"),
    ("read_only_space", 0x07531): (140, "ObjectTemplateInfoMap"),
    ("read_only_space", 0x07559): (141, "AccessCheckInfoMap"),
    ("read_only_space", 0x07581): (142, "AccessorPairMap"),
    ("read_only_space", 0x075a9): (143, "AliasedArgumentsEntryMap"),
    ("read_only_space", 0x075d1): (144, "AllocationMementoMap"),
    ("read_only_space", 0x075f9): (147, "AsmWasmDataMap"),
    ("read_only_space", 0x07621): (148, "AsyncGeneratorRequestMap"),
    ("read_only_space", 0x07649): (149, "BreakPointMap"),
    ("read_only_space", 0x07671): (150, "BreakPointInfoMap"),
    ("read_only_space", 0x07699): (151, "CallSiteInfoMap"),
    ("read_only_space", 0x076c1): (152, "ClassPositionsMap"),
    ("read_only_space", 0x076e9): (153, "DebugInfoMap"),
    ("read_only_space", 0x07711): (155, "ErrorStackDataMap"),
    ("read_only_space", 0x07739): (157, "FunctionTemplateRareDataMap"),
    ("read_only_space", 0x07761): (159, "InterpreterDataMap"),
    ("read_only_space", 0x07789): (160, "ModuleRequestMap"),
    ("read_only_space", 0x077b1): (161, "PromiseCapabilityMap"),
    ("read_only_space", 0x077d9): (162, "PromiseOnStackMap"),
    ("read_only_space", 0x07801): (163, "PromiseReactionMap"),
    ("read_only_space", 0x07829): (164, "PropertyDescriptorObjectMap"),
    ("read_only_space", 0x07851): (165, "PrototypeInfoMap"),
    ("read_only_space", 0x07879): (166, "RegExpBoilerplateDescriptionMap"),
    ("read_only_space", 0x078a1): (167, "ScriptMap"),
    ("read_only_space", 0x078c9): (168, "ScriptOrModuleMap"),
    ("read_only_space", 0x078f1): (169, "SourceTextModuleInfoEntryMap"),
    ("read_only_space", 0x07919): (170, "StackFrameInfoMap"),
    ("read_only_space", 0x07941): (171, "TemplateObjectDescriptionMap"),
    ("read_only_space", 0x07969): (172, "Tuple2Map"),
    ("read_only_space", 0x07991): (173, "WasmExceptionTagMap"),
    ("read_only_space", 0x079b9): (174, "WasmIndirectFunctionTableMap"),
    ("read_only_space", 0x079e1): (194, "SloppyArgumentsElementsMap"),
    ("read_only_space", 0x07a09): (227, "DescriptorArrayMap"),
    ("read_only_space", 0x07a31): (202, "UncompiledDataWithoutPreparseDataMap"),
    ("read_only_space", 0x07a59): (200, "UncompiledDataWithPreparseDataMap"),
    ("read_only_space", 0x07a81): (203, "UncompiledDataWithoutPreparseDataWithJobMap"),
    ("read_only_space", 0x07aa9): (201, "UncompiledDataWithPreparseDataAndJobMap"),
    ("read_only_space", 0x07ad1): (249, "OnHeapBasicBlockProfilerDataMap"),
    ("read_only_space", 0x07af9): (234, "CachedTemplateObjectMap"),
    ("read_only_space", 0x07b21): (195, "TurbofanBitsetTypeMap"),
    ("read_only_space", 0x07b49): (199, "TurbofanUnionTypeMap"),
    ("read_only_space", 0x07b71): (198, "TurbofanRangeTypeMap"),
    ("read_only_space", 0x07b99): (196, "TurbofanHeapConstantTypeMap"),
    ("read_only_space", 0x07bc1): (197, "TurbofanOtherNumberConstantTypeMap"),
    ("read_only_space", 0x07be9): (245, "InternalClassMap"),
    ("read_only_space", 0x07c11): (256, "SmiPairMap"),
    ("read_only_space", 0x07c39): (255, "SmiBoxMap"),
    ("read_only_space", 0x07c61): (219, "ExportedSubClassBaseMap"),
    ("read_only_space", 0x07c89): (220, "ExportedSubClassMap"),
    ("read_only_space", 0x07cb1): (225, "AbstractInternalClassSubclass1Map"),
    ("read_only_space", 0x07cd9): (226, "AbstractInternalClassSubclass2Map"),
    ("read_only_space", 0x07d01): (193, "InternalClassWithSmiElementsMap"),
    ("read_only_space", 0x07d29): (246, "InternalClassWithStructElementsMap"),
    ("read_only_space", 0x07d51): (221, "ExportedSubClass2Map"),
    ("read_only_space", 0x07d79): (257, "SortStateMap"),
    ("read_only_space", 0x07da1): (263, "WasmStringViewIterMap"),
    ("read_only_space", 0x07dc9): (145, "AllocationSiteWithWeakNextMap"),
    ("read_only_space", 0x07df1): (145, "AllocationSiteWithoutWeakNextMap"),
    ("read_only_space", 0x07ed1): (137, "LoadHandler1Map"),
    ("read_only_space", 0x07ef9): (137, "LoadHandler2Map"),
    ("read_only_space", 0x07f21): (137, "LoadHandler3Map"),
    ("read_only_space", 0x07f49): (138, "StoreHandler0Map"),
    ("read_only_space", 0x07f71): (138, "StoreHandler1Map"),
    ("read_only_space", 0x07f99): (138, "StoreHandler2Map"),
    ("read_only_space", 0x07fc1): (138, "StoreHandler3Map"),
    ("old_space", 0x0439d): (2115, "ExternalMap"),
    ("old_space", 0x043cd): (2119, "JSMessageObjectMap"),
}

# List of known V8 objects.
KNOWN_OBJECTS = {
  ("read_only_space", 0x021d9): "EmptyWeakArrayList",
  ("read_only_space", 0x021e5): "EmptyDescriptorArray",
  ("read_only_space", 0x0221d): "EmptyEnumCache",
  ("read_only_space", 0x02251): "EmptyFixedArray",
  ("read_only_space", 0x02259): "NullValue",
  ("read_only_space", 0x02361): "UninitializedValue",
  ("read_only_space", 0x023d9): "UndefinedValue",
  ("read_only_space", 0x0241d): "NanValue",
  ("read_only_space", 0x02451): "TheHoleValue",
  ("read_only_space", 0x0247d): "HoleNanValue",
  ("read_only_space", 0x024b1): "TrueValue",
  ("read_only_space", 0x024f1): "FalseValue",
  ("read_only_space", 0x02521): "empty_string",
  ("read_only_space", 0x0275d): "EmptyScopeInfo",
  ("read_only_space", 0x02795): "ArgumentsMarker",
  ("read_only_space", 0x027f5): "Exception",
  ("read_only_space", 0x02851): "TerminationException",
  ("read_only_space", 0x028b9): "OptimizedOut",
  ("read_only_space", 0x02919): "StaleRegister",
  ("read_only_space", 0x034b9): "EmptyPropertyArray",
  ("read_only_space", 0x034c1): "EmptyByteArray",
  ("read_only_space", 0x034c9): "EmptyObjectBoilerplateDescription",
  ("read_only_space", 0x034fd): "EmptyArrayBoilerplateDescription",
  ("read_only_space", 0x03509): "EmptyClosureFeedbackCellArray",
  ("read_only_space", 0x03511): "EmptySlowElementDictionary",
  ("read_only_space", 0x03535): "EmptyOrderedHashMap",
  ("read_only_space", 0x03549): "EmptyOrderedHashSet",
  ("read_only_space", 0x0355d): "EmptyFeedbackMetadata",
  ("read_only_space", 0x03569): "EmptyPropertyDictionary",
  ("read_only_space", 0x03591): "EmptyOrderedPropertyDictionary",
  ("read_only_space", 0x035a9): "EmptySwissPropertyDictionary",
  ("read_only_space", 0x035fd): "NoOpInterceptorInfo",
  ("read_only_space", 0x03625): "EmptyArrayList",
  ("read_only_space", 0x03631): "EmptyWeakFixedArray",
  ("read_only_space", 0x03639): "InfinityValue",
  ("read_only_space", 0x03645): "MinusZeroValue",
  ("read_only_space", 0x03651): "MinusInfinityValue",
  ("read_only_space", 0x0365d): "SingleCharacterStringTable",
  ("read_only_space", 0x04a65): "SelfReferenceMarker",
  ("read_only_space", 0x04aa5): "BasicBlockCountersMarker",
  ("read_only_space", 0x04ae9): "OffHeapTrampolineRelocationInfo",
  ("read_only_space", 0x04af5): "GlobalThisBindingScopeInfo",
  ("read_only_space", 0x04b25): "EmptyFunctionScopeInfo",
  ("read_only_space", 0x04b49): "NativeScopeInfo",
  ("read_only_space", 0x04b61): "HashSeed",
  ("old_space", 0x04235): "ArgumentsIteratorAccessor",
  ("old_space", 0x0424d): "ArrayLengthAccessor",
  ("old_space", 0x04265): "BoundFunctionLengthAccessor",
  ("old_space", 0x0427d): "BoundFunctionNameAccessor",
  ("old_space", 0x04295): "ErrorStackAccessor",
  ("old_space", 0x042ad): "FunctionArgumentsAccessor",
  ("old_space", 0x042c5): "FunctionCallerAccessor",
  ("old_space", 0x042dd): "FunctionNameAccessor",
  ("old_space", 0x042f5): "FunctionLengthAccessor",
  ("old_space", 0x0430d): "FunctionPrototypeAccessor",
  ("old_space", 0x04325): "SharedArrayLengthAccessor",
  ("old_space", 0x0433d): "StringLengthAccessor",
  ("old_space", 0x04355): "ValueUnavailableAccessor",
  ("old_space", 0x0436d): "WrappedFunctionLengthAccessor",
  ("old_space", 0x04385): "WrappedFunctionNameAccessor",
  ("old_space", 0x0439d): "ExternalMap",
  ("old_space", 0x043c5): "InvalidPrototypeValidityCell",
  ("old_space", 0x043cd): "JSMessageObjectMap",
  ("old_space", 0x043f5): "EmptyScript",
  ("old_space", 0x04439): "ManyClosuresCell",
  ("old_space", 0x04445): "ArrayConstructorProtector",
  ("old_space", 0x04459): "NoElementsProtector",
  ("old_space", 0x0446d): "MegaDOMProtector",
  ("old_space", 0x04481): "IsConcatSpreadableProtector",
  ("old_space", 0x04495): "ArraySpeciesProtector",
  ("old_space", 0x044a9): "TypedArraySpeciesProtector",
  ("old_space", 0x044bd): "PromiseSpeciesProtector",
  ("old_space", 0x044d1): "RegExpSpeciesProtector",
  ("old_space", 0x044e5): "StringLengthProtector",
  ("old_space", 0x044f9): "ArrayIteratorProtector",
  ("old_space", 0x0450d): "ArrayBufferDetachingProtector",
  ("old_space", 0x04521): "PromiseHookProtector",
  ("old_space", 0x04535): "PromiseResolveProtector",
  ("old_space", 0x04549): "MapIteratorProtector",
  ("old_space", 0x0455d): "PromiseThenProtector",
  ("old_space", 0x04571): "SetIteratorProtector",
  ("old_space", 0x04585): "StringIteratorProtector",
  ("old_space", 0x04599): "StringSplitCache",
  ("old_space", 0x049a1): "RegExpMultipleCache",
  ("old_space", 0x04da9): "BuiltinsConstantsTable",
  ("old_space", 0x05205): "AsyncFunctionAwaitRejectSharedFun",
  ("old_space", 0x05229): "AsyncFunctionAwaitResolveSharedFun",
  ("old_space", 0x0524d): "AsyncGeneratorAwaitRejectSharedFun",
  ("old_space", 0x05271): "AsyncGeneratorAwaitResolveSharedFun",
  ("old_space", 0x05295): "AsyncGeneratorYieldResolveSharedFun",
  ("old_space", 0x052b9): "AsyncGeneratorReturnResolveSharedFun",
  ("old_space", 0x052dd): "AsyncGeneratorReturnClosedRejectSharedFun",
  ("old_space", 0x05301): "AsyncGeneratorReturnClosedResolveSharedFun",
  ("old_space", 0x05325): "AsyncIteratorValueUnwrapSharedFun",
  ("old_space", 0x05349): "PromiseAllResolveElementSharedFun",
  ("old_space", 0x0536d): "PromiseAllSettledResolveElementSharedFun",
  ("old_space", 0x05391): "PromiseAllSettledRejectElementSharedFun",
  ("old_space", 0x053b5): "PromiseAnyRejectElementSharedFun",
  ("old_space", 0x053d9): "PromiseCapabilityDefaultRejectSharedFun",
  ("old_space", 0x053fd): "PromiseCapabilityDefaultResolveSharedFun",
  ("old_space", 0x05421): "PromiseCatchFinallySharedFun",
  ("old_space", 0x05445): "PromiseGetCapabilitiesExecutorSharedFun",
  ("old_space", 0x05469): "PromiseThenFinallySharedFun",
  ("old_space", 0x0548d): "PromiseThrowerFinallySharedFun",
  ("old_space", 0x054b1): "PromiseValueThunkFinallySharedFun",
  ("old_space", 0x054d5): "ProxyRevokeSharedFun",
  ("old_space", 0x054f9): "ShadowRealmImportValueFulfilledSFI",
  ("old_space", 0x0551d): "SourceTextModuleExecuteAsyncModuleFulfilledSFI",
  ("old_space", 0x05541): "SourceTextModuleExecuteAsyncModuleRejectedSFI",
}

# Lower 32 bits of first page addresses for various heap spaces.
HEAP_FIRST_PAGES = {
  0x000c0000: "old_space",
  0x00000000: "read_only_space",
}

# List of known V8 Frame Markers.
FRAME_MARKERS = (
  "ENTRY",
  "CONSTRUCT_ENTRY",
  "EXIT",
  "WASM",
  "WASM_TO_JS",
  "WASM_TO_JS_FUNCTION",
  "JS_TO_WASM",
  "STACK_SWITCH",
  "WASM_DEBUG_BREAK",
  "C_WASM_ENTRY",
  "WASM_EXIT",
  "WASM_COMPILE_LAZY",
  "INTERPRETED",
  "BASELINE",
  "MAGLEV",
  "TURBOFAN",
  "STUB",
  "TURBOFAN_STUB_WITH_CONTEXT",
  "BUILTIN_CONTINUATION",
  "JAVA_SCRIPT_BUILTIN_CONTINUATION",
  "JAVA_SCRIPT_BUILTIN_CONTINUATION_WITH_CATCH",
  "INTERNAL",
  "CONSTRUCT",
  "BUILTIN",
  "BUILTIN_EXIT",
  "NATIVE",
)

# This set of constants is generated from a shipping build.
