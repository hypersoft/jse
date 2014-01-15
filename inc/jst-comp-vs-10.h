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

/* Visual Studio 2010 is known to support optional variadic macro arguments */

#define JST(NAME, ...) NAME (ctx, __VA_ARGS__, exception)
#define JSTAction(NAME, ...) NAME (ctx, __VA_ARGS__)
#define JSTUtility(...) (register JSTContext ctx, __VA_ARGS__, JSTValue *exception)

#define JSTConstructorCall(o, ...) JSTConstructorCall_(ctx, exception, o, __VA_ARGS__, JSTReservedAddress)
#define JSTFunctionCall(f, o, ...) JSTFunctionCall_(ctx, exception, f, o, __VA_ARGS__, JSTReservedAddress)
#define JSTScriptNativeError(ei, f, ...) (JSTScriptNativeError_(ctx, exception, __FILE__, __LINE__, ei, f, __VA_ARGS__), JSTValueNull)

