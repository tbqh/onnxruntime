// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

/*
 * This file defines SessionOptions Config Keys and format of the Config Values.
 *
 * The Naming Convention for a SessionOptions Config Key,
 * "[Area][.[SubArea1].[SubArea2]...].[Keyname]"
 * Such as "ep.cuda.use_arena"
 * The Config Key cannot be empty
 * The maximum length of the Config Key is 128
 *
 * The string format of a SessionOptions Config Value is defined individually for each Config.
 * The maximum length of the Config Value is 1024
 */

// Key for disable PrePacking,
// If the config value is set to "1" then the prepacking is disabled, otherwise prepacking is enabled (default value)
static const char* const kOrtSessionOptionsConfigDisablePrepacking = "session.disable_prepacking";

// A value of "1" means allocators registered in the env will be used. "0" means the allocators created in the session
// will be used. Use this to override the usage of env allocators on a per session level.
static const char* const kOrtSessionOptionsConfigUseEnvAllocators = "session.use_env_allocators";

// Set to 'ORT' (case sensitive) to load an ORT format model.
// If unset, model type will default to ONNX unless inferred from filename ('.ort' == ORT format) or bytes to be ORT
static const char* const kOrtSessionOptionsConfigLoadModelFormat = "session.load_model_format";

// Set to 'ORT' (case sensitive) to save optimized model in ORT format when SessionOptions.optimized_model_path is set.
// If unset, format will default to ONNX unless optimized_model_filepath ends in '.ort'.
static const char* const kOrtSessionOptionsConfigSaveModelFormat = "session.save_model_format";

// If a value is "1", flush-to-zero and denormal-as-zero are applied. The default is "0".
// When multiple sessions are created, a main thread doesn't override changes from succeeding session options,
// but threads in session thread pools follow option changes.
// When ORT runs with OpenMP, the same rule is applied, i.e. the first session option to flush-to-zero and
// denormal-as-zero is only applied to global OpenMP thread pool, which doesn't support per-session thread pool.
// Note that an alternative way not using this option at runtime is to train and export a model without denormals
// and that's recommended because turning this option on may hurt model accuracy.
static const char* const kOrtSessionOptionsConfigSetDenormalAsZero = "session.set_denormal_as_zero";

// It controls to run quantization model in QDQ (QuantizelinearDeQuantizelinear) format or not.
// "0": disable. ORT doesn't do fusion logic for QDQ format.
// "1": enable. ORT does fusion logic for QDQ format.
// Its default value is "1"
static const char* const kOrtSessionOptionsEnableQuantQDQ = "session.enable_quant_qdq";

// Setting intra op thread affnity - only implemented on windows for the branch
// affinity string follows format:
// logic_processor_id,logic_processor_id;logic_processor_id,processor_id
// semicolon isolates configurations among threads, while comma split processors where ith thread expected to attach to.
// e.g. 1,2,3;4,5
// specifies affinities for two threads, with the 1st thread attach to the 1st, 2nd, and 3rd processor, and 2nd thread to the 4th and 5th processor.
// To ease the configuration for thread across many processors, an interval is also allowed:
// e.g. 1-8;8-16;17-24
// orders that the 1st thread runs on first eight processors, 2nd thread runs on next eight processors, and so forth.
// Note:
// 1. Once set, the number of affinities must equal to intra_op_num_threads - 1, since ort does not set affinity on the main thread.
// 2. For windows, ort will infer the group id from a logic process id, e.g., assume there are two groups with each has 64 logic processors,
//    an id of 64 will be inferred as the last processor of the 1st group, while 65 will be treated as the 1st processor of the second group.
//    Hence 64-65 is an invalid configuration since a windows thread cannot be attached to processors crossing group boundary.
static const char* const kOrtSessionOptionsConfigIntraOpThreadAffinities = "session.intra_op_thread_affinities";