/*

Hypersoft Systems bin2inc (c) 2014, Triston J. Taylor

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list
   of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this
   list of conditions and the following disclaimer in the documentation and/or other
   materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.

*/

/* Yeah, I don't know how this works on other systems: ! portable */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Just turns byte streams into hex encoded char arrays over stdio is all */
static size_t encode(char * name) {

	int nc, x = 0, nl = strlen(name);
	for(x; x < nl; x++) {
		nc = name[x];
		if (isalpha(nc)) continue;
		else { name[x] = 0; break; }
	}

	int c = 0, l = 0, w = 0;

	printf("\nchar %s[] = {\n\n\t", name);

	while ((c = fgetc(stdin)) != EOF) {
		printf("0x%02x, ", c), l++, w++;
		if (w == 13) printf("\n\t"), w = 0;
	}

	size_t padding = (13 - w);

	while (w < 12) printf("0x%02x, ", 0), w++;

	printf("0x%02x\n\n}; unsigned %sLength = %i;\n\n", 0, name, l);
	printf("/* %s zero-padding: %i */\n\n", name, padding);

	return l;

}

int main(int argc, char ** argv) {
	return encode((argc > 1)?argv[1]:"stdin"), 0;
}
