/* maxipng.h - Shapes for Maxicode output to PNG file */

/*
    libzint - the open source barcode library
    Copyright (C) 2008 Robin Stuart <robin@zint.org.uk>

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name of the project nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.
*/

/* This file contains the pixel-by-pixel representation of maxicode glyphs
   at a resolution of 12 pixels per millimeter. hexagon[] is taken directly
   from ISO 16023 Annex J. bullseye[] was calculated by the Gimp */

#define SSET	"0123456789ABCDEF"

static const int hexagon[120] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 1, 1, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 0, 0,
    0, 0, 1, 1, 1, 1, 1, 1, 1, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 1, 1, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 0, 0,
    0, 0, 0, 0, 1, 1, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const unsigned int bullseye_compressed[] = {
    0,0,0,0,0,255,248,0,0,0,0,0,
    0,0,0,0,31,255,255,192,0,0,0,0,
    0,0,0,1,255,255,255,252,0,0,0,0,
    0,0,0,7,255,255,255,255,0,0,0,0,
    0,0,0,31,255,255,255,255,192,0,0,0,
    0,0,0,127,255,255,255,255,240,0,0,0,
    0,0,1,255,255,255,255,255,252,0,0,0,
    0,0,7,255,255,255,255,255,255,0,0,0,
    0,0,15,255,255,0,7,255,255,128,0,0,
    0,0,63,255,240,0,0,127,255,224,0,0,
    0,0,127,255,128,0,0,15,255,240,0,0,
    0,0,255,252,0,0,0,1,255,248,0,0,
    0,1,255,240,0,0,0,0,127,252,0,0,
    0,3,255,224,0,0,0,0,63,254,0,0,
    0,7,255,128,0,0,0,0,15,255,0,0,
    0,15,255,0,0,0,0,0,7,255,128,0,
    0,31,252,0,0,127,240,0,1,255,192,0,
    0,63,248,0,7,255,255,0,0,255,224,0,
    0,127,240,0,63,255,255,224,0,127,240,0,
    0,127,224,0,255,255,255,248,0,63,240,0,
    0,255,192,1,255,255,255,252,0,31,248,0,
    1,255,128,7,255,255,255,255,0,15,252,0,
    1,255,0,15,255,255,255,255,128,7,252,0,
    3,255,0,63,255,255,255,255,224,7,254,0,
    3,254,0,127,255,192,31,255,240,3,254,0,
    7,252,0,255,252,0,1,255,248,1,255,0,
    7,252,1,255,240,0,0,127,252,1,255,0,
    15,248,1,255,192,0,0,31,252,0,255,128,
    15,240,3,255,128,0,0,15,254,0,127,128,
    31,240,7,255,0,0,0,7,255,0,127,192,
    31,224,7,254,0,0,0,3,255,0,63,192,
    63,224,15,252,0,0,0,1,255,128,63,224,
    63,224,31,248,0,63,192,0,255,192,63,224,
    63,192,31,240,0,255,240,0,127,192,31,224,
    63,192,63,224,3,255,252,0,63,224,31,224,
    127,192,63,224,7,255,254,0,63,224,31,240,
    127,128,63,192,15,255,255,0,31,224,15,240,
    127,128,127,192,31,255,255,128,31,240,15,240,
    127,128,127,128,63,255,255,192,15,240,15,240,
    127,128,127,128,63,255,255,192,15,240,15,240,
    255,0,127,128,127,240,255,224,15,240,7,240,
    255,0,255,128,127,192,63,224,15,248,7,240,
    255,0,255,0,255,128,31,240,7,248,7,240,
    255,0,255,0,255,128,31,240,7,248,7,240,
    255,0,255,0,255,0,15,240,7,248,7,240,
    255,0,255,0,255,0,15,240,7,248,7,240,
    255,0,255,0,255,0,15,240,7,248,7,240,
    255,0,255,0,255,0,15,240,7,248,7,240,
    255,0,255,0,255,128,31,240,7,248,7,240,
    255,0,255,0,255,128,31,240,7,248,7,240,
    255,0,255,0,127,192,63,224,7,248,7,240,
    255,0,255,128,127,240,255,224,15,248,7,240,
    255,0,127,128,63,255,255,192,15,240,7,240,
    127,128,127,128,63,255,255,192,15,240,15,240,
    127,128,127,128,31,255,255,128,15,240,15,240,
    127,128,127,192,15,255,255,0,31,240,15,240,
    127,128,63,192,7,255,254,0,31,224,15,240,
    127,192,63,224,3,255,252,0,63,224,31,240,
    63,192,63,224,0,255,240,0,63,224,31,224,
    63,192,31,240,0,63,192,0,127,192,31,224,
    63,224,31,248,0,0,0,0,255,192,63,224,
    63,224,15,252,0,0,0,1,255,128,63,224,
    31,224,7,254,0,0,0,3,255,0,63,192,
    31,240,7,255,0,0,0,7,255,0,127,192,
    15,240,3,255,128,0,0,15,254,0,127,128,
    15,248,1,255,192,0,0,31,252,0,255,128,
    7,252,1,255,240,0,0,127,252,1,255,0,
    7,252,0,255,252,0,1,255,248,1,255,0,
    3,254,0,127,255,192,31,255,240,3,254,0,
    3,255,0,63,255,255,255,255,224,7,254,0,
    1,255,0,15,255,255,255,255,128,7,252,0,
    1,255,128,7,255,255,255,255,0,15,252,0,
    0,255,192,1,255,255,255,252,0,31,248,0,
    0,127,224,0,255,255,255,248,0,63,240,0,
    0,127,240,0,63,255,255,224,0,127,240,0,
    0,63,248,0,7,255,255,0,0,255,224,0,
    0,31,252,0,0,127,240,0,1,255,192,0,
    0,15,255,0,0,0,0,0,7,255,128,0,
    0,7,255,128,0,0,0,0,15,255,0,0,
    0,3,255,224,0,0,0,0,63,254,0,0,
    0,1,255,240,0,0,0,0,127,252,0,0,
    0,0,255,252,0,0,0,1,255,248,0,0,
    0,0,127,255,128,0,0,15,255,240,0,0,
    0,0,63,255,240,0,0,127,255,224,0,0,
    0,0,15,255,255,0,7,255,255,128,0,0,
    0,0,7,255,255,255,255,255,255,0,0,0,
    0,0,1,255,255,255,255,255,252,0,0,0,
    0,0,0,127,255,255,255,255,240,0,0,0,
    0,0,0,31,255,255,255,255,192,0,0,0,
    0,0,0,7,255,255,255,255,0,0,0,0,
    0,0,0,1,255,255,255,252,0,0,0,0,
    0,0,0,0,31,255,255,192,0,0,0,0,
    0,0,0,0,0,255,248,0,0,0,0,0
};
