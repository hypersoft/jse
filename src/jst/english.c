/*

Hypersoft Systems JST AND Hypersoft Systems JSE
Copyright (c) 2014, Triston J. Taylor

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

const utf8 * jst_prop_is_nan = "isNaN";
const utf8 * jst_prop_parse_float = "parseFloat";
const utf8 * jst_prop_parse_int = "parseInt";

const utf8 * jst_prop_action = "action";
const utf8 * jst_prop_address = "address";
const utf8 * jst_prop_advance = "advance";
const utf8 * jst_prop_alias = "alias";
const utf8 * jst_prop_array = "array";
const utf8 * jst_prop_axis = "axis";
const utf8 * jst_prop_base = "base";
const utf8 * jst_prop_bass = "bass";
const utf8 * jst_prop_bits = "bits";
const utf8 * jst_prop_block = "block";
const utf8 * jst_prop_boolean = "boolean";
const utf8 * jst_prop_bool = "bool";
const utf8 * jst_prop_caption = "caption";
const utf8 * jst_prop_char = "char";
const utf8 * jst_prop_clear = "clear";
const utf8 * jst_prop_client = "client";
const utf8 * jst_prop_close = "close";
const utf8 * jst_prop_configurable = "configurable";
const utf8 * jst_prop_constant = "constant";
const utf8 * jst_prop_constructor = "constructor";
const utf8 * jst_prop_copy = "copy";
const utf8 * jst_prop_delete = "delete";
const utf8 * jst_prop_destination = "destination";
const utf8 * jst_prop_device = "device";
const utf8 * jst_prop_distance = "distance";
const utf8 * jst_prop_double = "double";
const utf8 * jst_prop_dynamic = "dynamic";
const utf8 * jst_prop_enum = "enum";
const utf8 * jst_prop_enumerable = "enumerable";
const utf8 * jst_prop_erase = "erase";
const utf8 * jst_prop_execute = "execute";
const utf8 * jst_prop_file = "file";
const utf8 * jst_prop_flag = "flag";
const utf8 * jst_prop_float = "float";
const utf8 * jst_prop_fork = "fork";
const utf8 * jst_prop_function = "function";
const utf8 * jst_prop_gain = "gain";
const utf8 * jst_prop_get = "get";
const utf8 * jst_prop_name = "name";
const utf8 * jst_prop_native = "native";
const utf8 * jst_prop_handle = "handle";
const utf8 * jst_prop_height = "height";
const utf8 * jst_prop_hex = "hex";
const utf8 * jst_prop_host = "host";
const utf8 * jst_prop_horizontal = "horizontal";
const utf8 * jst_prop_index = "index";
const utf8 * jst_prop_input = "input";
const utf8 * jst_prop_int64 = "int64";
const utf8 * jst_prop_integer = "integer";
const utf8 * jst_prop_int = "int";
const utf8 * jst_prop_json = "json";
const utf8 * jst_prop_key = "key";
const utf8 * jst_prop_label = "label";
const utf8 * jst_prop_lateral = "lateral";
const utf8 * jst_prop_length = "length";
const utf8 * jst_prop_level = "level";
const utf8 * jst_prop_list = "list";
const utf8 * jst_prop_link = "link";
const utf8 * jst_prop_limit = "limit";
const utf8 * jst_prop_load = "load";
const utf8 * jst_prop_loop = "loop";
const utf8 * jst_prop_long = "long";
const utf8 * jst_prop_major = "major";
const utf8 * jst_prop_mask = "mask";
const utf8 * jst_prop_master = "master";
const utf8 * jst_prop_maximum = "maximum";
const utf8 * jst_prop_mean = "mean";
const utf8 * jst_prop_media = "media";
const utf8 * jst_prop_median = "median";
const utf8 * jst_prop_memory = "memory";
const utf8 * jst_prop_minimum = "minimum";
const utf8 * jst_prop_minor = "minor";
const utf8 * jst_prop_mode = "mode";
const utf8 * jst_prop_network = "network";
const utf8 * jst_prop_node = "node";
const utf8 * jst_prop_octal = "octal";
const utf8 * jst_prop_offset = "offset";
const utf8 * jst_prop_open = "open";
const utf8 * jst_prop_output = "output";
const utf8 * jst_prop_paste = "paste";
const utf8 * jst_prop_path = "path";
const utf8 * jst_prop_peek = "peek";
const utf8 * jst_prop_pipe = "pipe";
const utf8 * jst_prop_pitch = "pitch";
const utf8 * jst_prop_pointer = "pointer";
const utf8 * jst_prop_poke = "poke";
const utf8 * jst_prop_pop = "pop";
const utf8 * jst_prop_port = "port";
const utf8 * jst_prop_power = "power";
const utf8 * jst_prop_process = "process";
const utf8 * jst_prop_procedure = "procedure";
const utf8 * jst_prop_property = "property";
const utf8 * jst_prop_prototype = "prototype";
const utf8 * jst_prop_private = "private";
const utf8 * jst_prop_push = "push";
const utf8 * jst_prop_radians = "radians";
const utf8 * jst_prop_radix = "radix";
const utf8 * jst_prop_random = "random";
const utf8 * jst_prop_read_only = "readOnly";
const utf8 * jst_prop_reference = "reference";
const utf8 * jst_prop_remove = "remove";
const utf8 * jst_prop_rewind = "rewind";
const utf8 * jst_prop_resource = "resource";
const utf8 * jst_prop_result = "result";
const utf8 * jst_prop_seek = "seek";
const utf8 * jst_prop_shift = "shift";
const utf8 * jst_prop_source = "source";
const utf8 * jst_prop_set = "server";
const utf8 * jst_prop_signed = "signed";
const utf8 * jst_prop_signal = "signal";
const utf8 * jst_prop_size = "size";
const utf8 * jst_prop_split = "split";
const utf8 * jst_prop_slave = "slave";
const utf8 * jst_prop_status = "status";
const utf8 * jst_prop_stream = "stream";
const utf8 * jst_prop_string = "string";
const utf8 * jst_prop_struct = "struct";
const utf8 * jst_prop_to_string = "toString";
const utf8 * jst_prop_text = "text";
const utf8 * jst_prop_time = "time";
const utf8 * jst_prop_trap = "trap";
const utf8 * jst_prop_treble = "treble";
const utf8 * jst_prop_type = "type";
const utf8 * jst_prop_value_of = "valueOf";
const utf8 * jst_prop_union = "union";
const utf8 * jst_prop_unload = "unload";
const utf8 * jst_prop_unsigned = "unsigned";
const utf8 * jst_prop_user_data = "userData";
const utf8 * jst_prop_utf = "utf";
const utf8 * jst_prop_value = "value";
const utf8 * jst_prop_version = "version";
const utf8 * jst_prop_vertical = "vertical";
const utf8 * jst_prop_void = "void";
const utf8 * jst_prop_weight = "weight";
const utf8 * jst_prop_width = "width";
const utf8 * jst_prop_x = "x";
const utf8 * jst_prop_y = "y";
const utf8 * jst_prop_z = "z";
const utf8 * jst_prop_writeable = "writable";
