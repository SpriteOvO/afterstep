/* 
 * All the code below represent subset of the 
 * Bruno Haible's libiconv library, homepage
 * http://clisp.cons.org/~haible/packages-libiconv.html
 *
 * Based on libiconv ver. 1.17
 * See below for copyright notice.
 */

/*
 * Copyright (C) 1999-2001 Free Software Foundation, Inc.
 * This file is part of the GNU LIBICONV Library.
 *
 * The GNU LIBICONV Library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * The GNU LIBICONV Library is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with the GNU LIBICONV Library; see the file COPYING.LIB.
 * If not, write to the Free Software Foundation, Inc., 59 Temple Place -
 * Suite 330, Boston, MA 02111-1307, USA.
 */

#include "config.h"

/*#define LOCAL_DEBUG*/
/*#define DO_CLOCKING*/

/* #define  I18N */

#include "string.h"
#include "char2uni.h"

/*
 * ISO-8859-1
 */
static const unsigned short _as_iso8859_1_2uni[128] = {
/* does not really require translation, but we'll stick it in 
   there for uniformity : */
  /* 0x80 */
  0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 
  0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, 
  /* 0x90 */
  0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 
  0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, 
  /* 0xa0 */
  0x00a0, 0x00a1, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x00a7, 
  0x00a8, 0x00a9, 0x00aa, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00af, 
  /* 0xb0 */
  0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x00b6, 0x00b7, 
  0x00b8, 0x00b9, 0x00ba, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0x00bf, 
  /* 0xc0 */
  0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7,   
  0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf,   
  /* 0xd0 */
  0x00d0, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x00d7, 
  0x00d8, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x00dd, 0x00de, 0x00df, 
  /* 0xe0 */
  0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7, 
  0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef, 
  /* 0xf0 */
  0x00f0, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00f7, 
  0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x00ff 
};

#ifdef  I18N
/*
 * ISO-8859-2
 */

static const unsigned short _as_iso8859_2_2uni[128] = {
  /* 0x80 */
  0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 
  0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, 
  /* 0x90 */
  0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 
  0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, 
  /* 0xa0 */
  0x00a0, 0x0104, 0x02d8, 0x0141, 0x00a4, 0x013d, 0x015a, 0x00a7,
  0x00a8, 0x0160, 0x015e, 0x0164, 0x0179, 0x00ad, 0x017d, 0x017b,
  /* 0xb0 */
  0x00b0, 0x0105, 0x02db, 0x0142, 0x00b4, 0x013e, 0x015b, 0x02c7,
  0x00b8, 0x0161, 0x015f, 0x0165, 0x017a, 0x02dd, 0x017e, 0x017c,
  /* 0xc0 */
  0x0154, 0x00c1, 0x00c2, 0x0102, 0x00c4, 0x0139, 0x0106, 0x00c7,
  0x010c, 0x00c9, 0x0118, 0x00cb, 0x011a, 0x00cd, 0x00ce, 0x010e,
  /* 0xd0 */
  0x0110, 0x0143, 0x0147, 0x00d3, 0x00d4, 0x0150, 0x00d6, 0x00d7,
  0x0158, 0x016e, 0x00da, 0x0170, 0x00dc, 0x00dd, 0x0162, 0x00df,
  /* 0xe0 */
  0x0155, 0x00e1, 0x00e2, 0x0103, 0x00e4, 0x013a, 0x0107, 0x00e7,
  0x010d, 0x00e9, 0x0119, 0x00eb, 0x011b, 0x00ed, 0x00ee, 0x010f,
  /* 0xf0 */
  0x0111, 0x0144, 0x0148, 0x00f3, 0x00f4, 0x0151, 0x00f6, 0x00f7,
  0x0159, 0x016f, 0x00fa, 0x0171, 0x00fc, 0x00fd, 0x0163, 0x02d9,
};

/*
 * ISO-8859-3
 */

static const unsigned short _as_iso8859_3_2uni[128] = {
  /* 0x80 */
  0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 
  0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, 
  /* 0x90 */
  0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 
  0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, 
  /* 0xa0 */
  0x00a0, 0x0126, 0x02d8, 0x00a3, 0x00a4, 0xfffd, 0x0124, 0x00a7,
  0x00a8, 0x0130, 0x015e, 0x011e, 0x0134, 0x00ad, 0xfffd, 0x017b,
  /* 0xb0 */
  0x00b0, 0x0127, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x0125, 0x00b7,
  0x00b8, 0x0131, 0x015f, 0x011f, 0x0135, 0x00bd, 0xfffd, 0x017c,
  /* 0xc0 */
  0x00c0, 0x00c1, 0x00c2, 0xfffd, 0x00c4, 0x010a, 0x0108, 0x00c7,
  0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf,
  /* 0xd0 */
  0xfffd, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x0120, 0x00d6, 0x00d7,
  0x011c, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x016c, 0x015c, 0x00df,
  /* 0xe0 */
  0x00e0, 0x00e1, 0x00e2, 0xfffd, 0x00e4, 0x010b, 0x0109, 0x00e7,
  0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef,
  /* 0xf0 */
  0xfffd, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x0121, 0x00f6, 0x00f7,
  0x011d, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x016d, 0x015d, 0x02d9,
};


/*
 * ISO-8859-4
 */

static const unsigned short _as_iso8859_4_2uni[128] = {
  /* 0x80 */
  0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 
  0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, 
  /* 0x90 */
  0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 
  0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, 
  /* 0xa0 */
  0x00a0, 0x0104, 0x0138, 0x0156, 0x00a4, 0x0128, 0x013b, 0x00a7,
  0x00a8, 0x0160, 0x0112, 0x0122, 0x0166, 0x00ad, 0x017d, 0x00af,
  /* 0xb0 */
  0x00b0, 0x0105, 0x02db, 0x0157, 0x00b4, 0x0129, 0x013c, 0x02c7,
  0x00b8, 0x0161, 0x0113, 0x0123, 0x0167, 0x014a, 0x017e, 0x014b,
  /* 0xc0 */
  0x0100, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x012e,
  0x010c, 0x00c9, 0x0118, 0x00cb, 0x0116, 0x00cd, 0x00ce, 0x012a,
  /* 0xd0 */
  0x0110, 0x0145, 0x014c, 0x0136, 0x00d4, 0x00d5, 0x00d6, 0x00d7,
  0x00d8, 0x0172, 0x00da, 0x00db, 0x00dc, 0x0168, 0x016a, 0x00df,
  /* 0xe0 */
  0x0101, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x012f,
  0x010d, 0x00e9, 0x0119, 0x00eb, 0x0117, 0x00ed, 0x00ee, 0x012b,
  /* 0xf0 */
  0x0111, 0x0146, 0x014d, 0x0137, 0x00f4, 0x00f5, 0x00f6, 0x00f7,
  0x00f8, 0x0173, 0x00fa, 0x00fb, 0x00fc, 0x0169, 0x016b, 0x02d9,
};

/*
 * ISO-8859-5
 */

static const unsigned short _as_iso8859_5_2uni[128] = {
  /* 0x80 */
  0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 
  0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, 
  /* 0x90 */
  0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 
  0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, 
  /* 0xa0 */
  0x00a0, 0x0401, 0x0402, 0x0403, 0x0404, 0x0405, 0x0406, 0x0407,
  0x0408, 0x0409, 0x040a, 0x040b, 0x040c, 0x00ad, 0x040e, 0x040f,
  /* 0xb0 */
  0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
  0x0418, 0x0419, 0x041a, 0x041b, 0x041c, 0x041d, 0x041e, 0x041f,
  /* 0xc0 */
  0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
  0x0428, 0x0429, 0x042a, 0x042b, 0x042c, 0x042d, 0x042e, 0x042f,
  /* 0xd0 */
  0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
  0x0438, 0x0439, 0x043a, 0x043b, 0x043c, 0x043d, 0x043e, 0x043f,
  /* 0xe0 */
  0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
  0x0448, 0x0449, 0x044a, 0x044b, 0x044c, 0x044d, 0x044e, 0x044f,
  /* 0xf0 */
  0x2116, 0x0451, 0x0452, 0x0453, 0x0454, 0x0455, 0x0456, 0x0457,
  0x0458, 0x0459, 0x045a, 0x045b, 0x045c, 0x00a7, 0x045e, 0x045f,
};

/*
 * ISO-8859-6
 */

static const unsigned short _as_iso8859_6_2uni[128] = {
  /* 0x80 */
  0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 
  0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, 
  /* 0x90 */
  0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 
  0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, 
  /* 0xa0 */
  0x00a0, 0xfffd, 0xfffd, 0xfffd, 0x00a4, 0xfffd, 0xfffd, 0xfffd,
  0xfffd, 0xfffd, 0xfffd, 0xfffd, 0x060c, 0x00ad, 0xfffd, 0xfffd,
  /* 0xb0 */
  0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd,
  0xfffd, 0xfffd, 0xfffd, 0x061b, 0xfffd, 0xfffd, 0xfffd, 0x061f,
  /* 0xc0 */
  0xfffd, 0x0621, 0x0622, 0x0623, 0x0624, 0x0625, 0x0626, 0x0627,
  0x0628, 0x0629, 0x062a, 0x062b, 0x062c, 0x062d, 0x062e, 0x062f,
  /* 0xd0 */
  0x0630, 0x0631, 0x0632, 0x0633, 0x0634, 0x0635, 0x0636, 0x0637,
  0x0638, 0x0639, 0x063a, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd,
  /* 0xe0 */
  0x0640, 0x0641, 0x0642, 0x0643, 0x0644, 0x0645, 0x0646, 0x0647,
  0x0648, 0x0649, 0x064a, 0x064b, 0x064c, 0x064d, 0x064e, 0x064f,
  /* 0xf0 */
  0x0650, 0x0651, 0x0652, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd,
  0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd,
};

/*
 * ISO-8859-7
 */

static const unsigned short _as_iso8859_7_2uni[128] = {
  /* 0x80 */
  0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 
  0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, 
  /* 0x90 */
  0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 
  0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, 
  /* 0xa0 */
  0x00a0, 0x2018, 0x2019, 0x00a3, 0xfffd, 0xfffd, 0x00a6, 0x00a7,
  0x00a8, 0x00a9, 0xfffd, 0x00ab, 0x00ac, 0x00ad, 0xfffd, 0x2015,
  /* 0xb0 */
  0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x0384, 0x0385, 0x0386, 0x00b7,
  0x0388, 0x0389, 0x038a, 0x00bb, 0x038c, 0x00bd, 0x038e, 0x038f,
  /* 0xc0 */
  0x0390, 0x0391, 0x0392, 0x0393, 0x0394, 0x0395, 0x0396, 0x0397,
  0x0398, 0x0399, 0x039a, 0x039b, 0x039c, 0x039d, 0x039e, 0x039f,
  /* 0xd0 */
  0x03a0, 0x03a1, 0xfffd, 0x03a3, 0x03a4, 0x03a5, 0x03a6, 0x03a7,
  0x03a8, 0x03a9, 0x03aa, 0x03ab, 0x03ac, 0x03ad, 0x03ae, 0x03af,
  /* 0xe0 */
  0x03b0, 0x03b1, 0x03b2, 0x03b3, 0x03b4, 0x03b5, 0x03b6, 0x03b7,
  0x03b8, 0x03b9, 0x03ba, 0x03bb, 0x03bc, 0x03bd, 0x03be, 0x03bf,
  /* 0xf0 */
  0x03c0, 0x03c1, 0x03c2, 0x03c3, 0x03c4, 0x03c5, 0x03c6, 0x03c7,
  0x03c8, 0x03c9, 0x03ca, 0x03cb, 0x03cc, 0x03cd, 0x03ce, 0xfffd,
};

/*
 * ISO-8859-8
 */

static const unsigned short _as_iso8859_8_2uni[128] = {
  /* 0x80 */
  0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 
  0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, 
  /* 0x90 */
  0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 
  0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, 
  /* 0xa0 */
  0x00a0, 0xfffd, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x00a7,
  0x00a8, 0x00a9, 0x00d7, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00af,
  /* 0xb0 */
  0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x00b6, 0x00b7,
  0x00b8, 0x00b9, 0x00f7, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0xfffd,
  /* 0xc0 */
  0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd,
  0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd,
  /* 0xd0 */
  0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd,
  0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0x2017,
  /* 0xe0 */
  0x05d0, 0x05d1, 0x05d2, 0x05d3, 0x05d4, 0x05d5, 0x05d6, 0x05d7,
  0x05d8, 0x05d9, 0x05da, 0x05db, 0x05dc, 0x05dd, 0x05de, 0x05df,
  /* 0xf0 */
  0x05e0, 0x05e1, 0x05e2, 0x05e3, 0x05e4, 0x05e5, 0x05e6, 0x05e7,
  0x05e8, 0x05e9, 0x05ea, 0xfffd, 0xfffd, 0x200e, 0x200f, 0xfffd,
};

/*
 * ISO-8859-9
 */

static const unsigned short _as_iso8859_9_2uni[128] = {
  /* 0x80 */
  0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 
  0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, 
  /* 0x90 */
  0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 
  0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, 
  /* 0xa0 */
  0x00a0, 0x00a1, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x00a7,
  0x00a8, 0x00a9, 0x00aa, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00af,
  /* 0xb0 */
  0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x00b6, 0x00b7,
  0x00b8, 0x00b9, 0x00ba, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0x00bf,
  /* 0xc0 */
  0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7,
  0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf,
  /* 0xd0 */
  0x011e, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x00d7,
  0x00d8, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x0130, 0x015e, 0x00df,
  /* 0xe0 */
  0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7,
  0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef,
  /* 0xf0 */
  0x011f, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00f7,
  0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x0131, 0x015f, 0x00ff,
};

/*
 * ISO-8859-10
 */

static const unsigned short _as_iso8859_10_2uni[128] = {
  /* 0x80 */
  0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 
  0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, 
  /* 0x90 */
  0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 
  0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, 
  /* 0xa0 */
  0x00a0, 0x0104, 0x0112, 0x0122, 0x012a, 0x0128, 0x0136, 0x00a7,
  0x013b, 0x0110, 0x0160, 0x0166, 0x017d, 0x00ad, 0x016a, 0x014a,
  /* 0xb0 */
  0x00b0, 0x0105, 0x0113, 0x0123, 0x012b, 0x0129, 0x0137, 0x00b7,
  0x013c, 0x0111, 0x0161, 0x0167, 0x017e, 0x2015, 0x016b, 0x014b,
  /* 0xc0 */
  0x0100, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x012e,
  0x010c, 0x00c9, 0x0118, 0x00cb, 0x0116, 0x00cd, 0x00ce, 0x00cf,
  /* 0xd0 */
  0x00d0, 0x0145, 0x014c, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x0168,
  0x00d8, 0x0172, 0x00da, 0x00db, 0x00dc, 0x00dd, 0x00de, 0x00df,
  /* 0xe0 */
  0x0101, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x012f,
  0x010d, 0x00e9, 0x0119, 0x00eb, 0x0117, 0x00ed, 0x00ee, 0x00ef,
  /* 0xf0 */
  0x00f0, 0x0146, 0x014d, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x0169,
  0x00f8, 0x0173, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x0138,
};

/*
 * ISO-8859-13
 */

static const unsigned short _as_iso8859_13_2uni[128] = {
  /* 0x80 */
  0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 
  0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, 
  /* 0x90 */
  0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 
  0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, 
  /* 0xa0 */
  0x00a0, 0x201d, 0x00a2, 0x00a3, 0x00a4, 0x201e, 0x00a6, 0x00a7,
  0x00d8, 0x00a9, 0x0156, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00c6,
  /* 0xb0 */
  0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x201c, 0x00b5, 0x00b6, 0x00b7,
  0x00f8, 0x00b9, 0x0157, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0x00e6,
  /* 0xc0 */
  0x0104, 0x012e, 0x0100, 0x0106, 0x00c4, 0x00c5, 0x0118, 0x0112,
  0x010c, 0x00c9, 0x0179, 0x0116, 0x0122, 0x0136, 0x012a, 0x013b,
  /* 0xd0 */
  0x0160, 0x0143, 0x0145, 0x00d3, 0x014c, 0x00d5, 0x00d6, 0x00d7,
  0x0172, 0x0141, 0x015a, 0x016a, 0x00dc, 0x017b, 0x017d, 0x00df,
  /* 0xe0 */
  0x0105, 0x012f, 0x0101, 0x0107, 0x00e4, 0x00e5, 0x0119, 0x0113,
  0x010d, 0x00e9, 0x017a, 0x0117, 0x0123, 0x0137, 0x012b, 0x013c,
  /* 0xf0 */
  0x0161, 0x0144, 0x0146, 0x00f3, 0x014d, 0x00f5, 0x00f6, 0x00f7,
  0x0173, 0x0142, 0x015b, 0x016b, 0x00fc, 0x017c, 0x017e, 0x2019,
};

/*
 * ISO-8859-14
 */

static const unsigned short _as_iso8859_14_2uni[128] = {
  /* 0x80 */
  0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 
  0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, 
  /* 0x90 */
  0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 
  0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, 
  /* 0xa0 */
  0x00a0, 0x1e02, 0x1e03, 0x00a3, 0x010a, 0x010b, 0x1e0a, 0x00a7,
  0x1e80, 0x00a9, 0x1e82, 0x1e0b, 0x1ef2, 0x00ad, 0x00ae, 0x0178,
  /* 0xb0 */
  0x1e1e, 0x1e1f, 0x0120, 0x0121, 0x1e40, 0x1e41, 0x00b6, 0x1e56,
  0x1e81, 0x1e57, 0x1e83, 0x1e60, 0x1ef3, 0x1e84, 0x1e85, 0x1e61,
  /* 0xc0 */
  0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7,
  0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf,
  /* 0xd0 */
  0x0174, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x1e6a,
  0x00d8, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x00dd, 0x0176, 0x00df,
  /* 0xe0 */
  0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7,
  0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef,
  /* 0xf0 */
  0x0175, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x1e6b,
  0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x0177, 0x00ff,
};

/*
 * ISO-8859-15
 */

static const unsigned short _as_iso8859_15_2uni[128] = {
  /* 0x80 */
  0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 
  0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, 
  /* 0x90 */
  0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 
  0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, 
  /* 0xa0 */
  0x00a0, 0x00a1, 0x00a2, 0x00a3, 0x20ac, 0x00a5, 0x0160, 0x00a7,
  0x0161, 0x00a9, 0x00aa, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00af,
  /* 0xb0 */
  0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x017d, 0x00b5, 0x00b6, 0x00b7,
  0x017e, 0x00b9, 0x00ba, 0x00bb, 0x0152, 0x0153, 0x0178, 0x00bf,
  /* 0xc0 */
  0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7,   
  0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf,   
  /* 0xd0 */
  0x00d0, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x00d7, 
  0x00d8, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x00dd, 0x00de, 0x00df, 
  /* 0xe0 */
  0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7, 
  0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef, 
  /* 0xf0 */
  0x00f0, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00f7, 
  0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x00ff 
};

/*
 * ISO-8859-16
 */

static const unsigned short _as_iso8859_16_2uni[128] = {
  /* 0x80 */
  0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 
  0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, 
  /* 0x90 */
  0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 
  0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, 
  /* 0xa0 */
  0x00a0, 0x0104, 0x0105, 0x0141, 0x20ac, 0x00ab, 0x0160, 0x00a7,
  0x0161, 0x00a9, 0x0218, 0x201e, 0x0179, 0x00ad, 0x017a, 0x017b,
  /* 0xb0 */
  0x00b0, 0x00b1, 0x010c, 0x0142, 0x017d, 0x201d, 0x00b6, 0x00b7,
  0x017e, 0x010d, 0x0219, 0x00bb, 0x0152, 0x0153, 0x0178, 0x017c,
  /* 0xc0 */
  0x00c0, 0x00c1, 0x00c2, 0x0102, 0x00c4, 0x0106, 0x00c6, 0x00c7,
  0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf,
  /* 0xd0 */
  0x0110, 0x0143, 0x00d2, 0x00d3, 0x00d4, 0x0150, 0x00d6, 0x015a,
  0x0170, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x0118, 0x021a, 0x00df,
  /* 0xe0 */
  0x00e0, 0x00e1, 0x00e2, 0x0103, 0x00e4, 0x0107, 0x00e6, 0x00e7,
  0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef,
  /* 0xf0 */
  0x0111, 0x0144, 0x00f2, 0x00f3, 0x00f4, 0x0151, 0x00f6, 0x015b,
  0x0171, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x0119, 0x021b, 0x00ff,
};

/*
 * KOI8-R
 */

/* Specification: RFC 1489 */

static const unsigned short _as_koi8_r_2uni[128] = {
  /* 0x80 */
  0x2500, 0x2502, 0x250c, 0x2510, 0x2514, 0x2518, 0x251c, 0x2524,
  0x252c, 0x2534, 0x253c, 0x2580, 0x2584, 0x2588, 0x258c, 0x2590,
  /* 0x90 */
  0x2591, 0x2592, 0x2593, 0x2320, 0x25a0, 0x2219, 0x221a, 0x2248,
  0x2264, 0x2265, 0x00a0, 0x2321, 0x00b0, 0x00b2, 0x00b7, 0x00f7,
  /* 0xa0 */
  0x2550, 0x2551, 0x2552, 0x0451, 0x2553, 0x2554, 0x2555, 0x2556,
  0x2557, 0x2558, 0x2559, 0x255a, 0x255b, 0x255c, 0x255d, 0x255e,
  /* 0xb0 */
  0x255f, 0x2560, 0x2561, 0x0401, 0x2562, 0x2563, 0x2564, 0x2565,
  0x2566, 0x2567, 0x2568, 0x2569, 0x256a, 0x256b, 0x256c, 0x00a9,
  /* 0xc0 */
  0x044e, 0x0430, 0x0431, 0x0446, 0x0434, 0x0435, 0x0444, 0x0433,
  0x0445, 0x0438, 0x0439, 0x043a, 0x043b, 0x043c, 0x043d, 0x043e,
  /* 0xd0 */
  0x043f, 0x044f, 0x0440, 0x0441, 0x0442, 0x0443, 0x0436, 0x0432,
  0x044c, 0x044b, 0x0437, 0x0448, 0x044d, 0x0449, 0x0447, 0x044a,
  /* 0xe0 */
  0x042e, 0x0410, 0x0411, 0x0426, 0x0414, 0x0415, 0x0424, 0x0413,
  0x0425, 0x0418, 0x0419, 0x041a, 0x041b, 0x041c, 0x041d, 0x041e,
  /* 0xf0 */
  0x041f, 0x042f, 0x0420, 0x0421, 0x0422, 0x0423, 0x0416, 0x0412,
  0x042c, 0x042b, 0x0417, 0x0428, 0x042d, 0x0429, 0x0427, 0x042a,
};

/*
 * KOI8-RU
 */

static const unsigned short _as_koi8_ru_2uni[128] = {
  /* 0x80 */
  0x2500, 0x2502, 0x250c, 0x2510, 0x2514, 0x2518, 0x251c, 0x2524,
  0x252c, 0x2534, 0x253c, 0x2580, 0x2584, 0x2588, 0x258c, 0x2590,
  /* 0x90 */
  0x2591, 0x2592, 0x2593, 0x2320, 0x25a0, 0x2219, 0x221a, 0x2248,
  0x2264, 0x2265, 0x00a0, 0x2321, 0x00b0, 0x00b2, 0x00b7, 0x00f7,
  /* 0xa0 */
  0x2550, 0x2551, 0x2552, 0x0451, 0x0454, 0x2554, 0x0456, 0x0457,
  0x2557, 0x2558, 0x2559, 0x255a, 0x255b, 0x0491, 0x045e, 0x255e,
  /* 0xb0 */
  0x255f, 0x2560, 0x2561, 0x0401, 0x0404, 0x2563, 0x0406, 0x0407,
  0x2566, 0x2567, 0x2568, 0x2569, 0x256a, 0x0490, 0x040e, 0x00a9,
  /* 0xc0 */
  0x044e, 0x0430, 0x0431, 0x0446, 0x0434, 0x0435, 0x0444, 0x0433,
  0x0445, 0x0438, 0x0439, 0x043a, 0x043b, 0x043c, 0x043d, 0x043e,
  /* 0xd0 */
  0x043f, 0x044f, 0x0440, 0x0441, 0x0442, 0x0443, 0x0436, 0x0432,
  0x044c, 0x044b, 0x0437, 0x0448, 0x044d, 0x0449, 0x0447, 0x044a,
  /* 0xe0 */
  0x042e, 0x0410, 0x0411, 0x0426, 0x0414, 0x0415, 0x0424, 0x0413,
  0x0425, 0x0418, 0x0419, 0x041a, 0x041b, 0x041c, 0x041d, 0x041e,
  /* 0xf0 */
  0x041f, 0x042f, 0x0420, 0x0421, 0x0422, 0x0423, 0x0416, 0x0412,
  0x042c, 0x042b, 0x0417, 0x0428, 0x042d, 0x0429, 0x0427, 0x042a,
};

/*
 * KOI8-U
 */

/* Specification: RFC 2319 */

static const unsigned short _as_koi8_u_2uni[128] = {
  /* 0x80 */
  0x2500, 0x2502, 0x250c, 0x2510, 0x2514, 0x2518, 0x251c, 0x2524,
  0x252c, 0x2534, 0x253c, 0x2580, 0x2584, 0x2588, 0x258c, 0x2590,
  /* 0x90 */
  0x2591, 0x2592, 0x2593, 0x2320, 0x25a0, 0x2219, 0x221a, 0x2248,
  0x2264, 0x2265, 0x00a0, 0x2321, 0x00b0, 0x00b2, 0x00b7, 0x00f7,
  /* 0xa0 */
  0x2550, 0x2551, 0x2552, 0x0451, 0x0454, 0x2554, 0x0456, 0x0457,
  0x2557, 0x2558, 0x2559, 0x255a, 0x255b, 0x0491, 0x255d, 0x255e,
  /* 0xb0 */
  0x255f, 0x2560, 0x2561, 0x0401, 0x0404, 0x2563, 0x0406, 0x0407,
  0x2566, 0x2567, 0x2568, 0x2569, 0x256a, 0x0490, 0x256c, 0x00a9,
  /* 0xc0 */
  0x044e, 0x0430, 0x0431, 0x0446, 0x0434, 0x0435, 0x0444, 0x0433,
  0x0445, 0x0438, 0x0439, 0x043a, 0x043b, 0x043c, 0x043d, 0x043e,
  /* 0xd0 */
  0x043f, 0x044f, 0x0440, 0x0441, 0x0442, 0x0443, 0x0436, 0x0432,
  0x044c, 0x044b, 0x0437, 0x0448, 0x044d, 0x0449, 0x0447, 0x044a,
  /* 0xe0 */
  0x042e, 0x0410, 0x0411, 0x0426, 0x0414, 0x0415, 0x0424, 0x0413,
  0x0425, 0x0418, 0x0419, 0x041a, 0x041b, 0x041c, 0x041d, 0x041e,
  /* 0xf0 */
  0x041f, 0x042f, 0x0420, 0x0421, 0x0422, 0x0423, 0x0416, 0x0412,
  0x042c, 0x042b, 0x0417, 0x0428, 0x042d, 0x0429, 0x0427, 0x042a,
};

/*
 * CP1250 Central European
 */

static const unsigned short _as_cp1250_2uni[128] = {
  /* 0x80 */
  0x20ac, 0xfffd, 0x201a, 0xfffd, 0x201e, 0x2026, 0x2020, 0x2021,
  0xfffd, 0x2030, 0x0160, 0x2039, 0x015a, 0x0164, 0x017d, 0x0179,
  /* 0x90 */
  0xfffd, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014,
  0xfffd, 0x2122, 0x0161, 0x203a, 0x015b, 0x0165, 0x017e, 0x017a,
  /* 0xa0 */
  0x00a0, 0x02c7, 0x02d8, 0x0141, 0x00a4, 0x0104, 0x00a6, 0x00a7,
  0x00a8, 0x00a9, 0x015e, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x017b,
  /* 0xb0 */
  0x00b0, 0x00b1, 0x02db, 0x0142, 0x00b4, 0x00b5, 0x00b6, 0x00b7,
  0x00b8, 0x0105, 0x015f, 0x00bb, 0x013d, 0x02dd, 0x013e, 0x017c,
  /* 0xc0 */
  0x0154, 0x00c1, 0x00c2, 0x0102, 0x00c4, 0x0139, 0x0106, 0x00c7,
  0x010c, 0x00c9, 0x0118, 0x00cb, 0x011a, 0x00cd, 0x00ce, 0x010e,
  /* 0xd0 */
  0x0110, 0x0143, 0x0147, 0x00d3, 0x00d4, 0x0150, 0x00d6, 0x00d7,
  0x0158, 0x016e, 0x00da, 0x0170, 0x00dc, 0x00dd, 0x0162, 0x00df,
  /* 0xe0 */
  0x0155, 0x00e1, 0x00e2, 0x0103, 0x00e4, 0x013a, 0x0107, 0x00e7,
  0x010d, 0x00e9, 0x0119, 0x00eb, 0x011b, 0x00ed, 0x00ee, 0x010f,
  /* 0xf0 */
  0x0111, 0x0144, 0x0148, 0x00f3, 0x00f4, 0x0151, 0x00f6, 0x00f7,
  0x0159, 0x016f, 0x00fa, 0x0171, 0x00fc, 0x00fd, 0x0163, 0x02d9,
};

/*
 * CP1251 Cyrillic
 */

static const unsigned short _as_cp1251_2uni[128] = {
  /* 0x80 */
  0x0402, 0x0403, 0x201a, 0x0453, 0x201e, 0x2026, 0x2020, 0x2021,
  0x20ac, 0x2030, 0x0409, 0x2039, 0x040a, 0x040c, 0x040b, 0x040f,
  /* 0x90 */
  0x0452, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014,
  0xfffd, 0x2122, 0x0459, 0x203a, 0x045a, 0x045c, 0x045b, 0x045f,
  /* 0xa0 */
  0x00a0, 0x040e, 0x045e, 0x0408, 0x00a4, 0x0490, 0x00a6, 0x00a7,
  0x0401, 0x00a9, 0x0404, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x0407,
  /* 0xb0 */
  0x00b0, 0x00b1, 0x0406, 0x0456, 0x0491, 0x00b5, 0x00b6, 0x00b7,
  0x0451, 0x2116, 0x0454, 0x00bb, 0x0458, 0x0405, 0x0455, 0x0457,
  /* 0xc0 */
  0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
  0x0418, 0x0419, 0x041a, 0x041b, 0x041c, 0x041d, 0x041e, 0x041f,
  /* 0xd0 */
  0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
  0x0428, 0x0429, 0x042a, 0x042b, 0x042c, 0x042d, 0x042e, 0x042f,
  /* 0xe0 */
  0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
  0x0438, 0x0439, 0x043a, 0x043b, 0x043c, 0x043d, 0x043e, 0x043f,
  /* 0xf0 */
  0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
  0x0448, 0x0449, 0x044a, 0x044b, 0x044c, 0x044d, 0x044e, 0x044f,
};

/*
 * CP1252 - Western European
 */

static const unsigned short _as_cp1252_2uni[128] = {
  /* 0x80 */
  0x20ac, 0xfffd, 0x201a, 0x0192, 0x201e, 0x2026, 0x2020, 0x2021,
  0x02c6, 0x2030, 0x0160, 0x2039, 0x0152, 0xfffd, 0x017d, 0xfffd,
  /* 0x90 */
  0xfffd, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014,
  0x02dc, 0x2122, 0x0161, 0x203a, 0x0153, 0xfffd, 0x017e, 0x0178,
  /* 0xa0 */
  0x00a0, 0x00a1, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x00a7, 
  0x00a8, 0x00a9, 0x00aa, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00af, 
  /* 0xb0 */
  0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x00b6, 0x00b7, 
  0x00b8, 0x00b9, 0x00ba, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0x00bf, 
  /* 0xc0 */
  0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7,   
  0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf,   
  /* 0xd0 */
  0x00d0, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x00d7, 
  0x00d8, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x00dd, 0x00de, 0x00df, 
  /* 0xe0 */
  0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7, 
  0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef, 
  /* 0xf0 */
  0x00f0, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00f7, 
  0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x00ff 
};

static const unsigned short *_as_supported_charsets[SUPPORTED_CHARSETS_NUM] = {
 &_as_iso8859_1_2uni[0], 
 &_as_iso8859_2_2uni[0], 
 &_as_iso8859_3_2uni[0], 
 &_as_iso8859_4_2uni[0], 
 &_as_iso8859_5_2uni[0], 
 &_as_iso8859_6_2uni[0], 
 &_as_iso8859_7_2uni[0], 
 &_as_iso8859_8_2uni[0], 
 &_as_iso8859_9_2uni[0], 
 &_as_iso8859_10_2uni[0],
 &_as_iso8859_13_2uni[0],
 &_as_iso8859_14_2uni[0],
 &_as_iso8859_15_2uni[0],
 &_as_iso8859_16_2uni[0],
 &_as_koi8_r_2uni[0], 
 &_as_koi8_ru_2uni[0],
 &_as_koi8_u_2uni[0], 
 &_as_cp1250_2uni[0], 
 &_as_cp1251_2uni[0], 
 &_as_cp1252_2uni[0]
};

#if 0
static unsigned short *_as_charset_names[SUPPORTED_CHARSETS_NUM][] = {
/* Standard 8-bit encodings */
{"ISO-8859-1", "ISO_8859-1", "ISO_8859-1:1987", "ISO-IR-100", "LATIN1", "L1", "csISOLatin1", 	"ISO8859-1", "ISO8859_1", "CP819", "IBM819", "" },
{"ISO-8859-2", "ISO_8859-2", "ISO_8859-2:1987", "ISO-IR-101", "LATIN2", "L2", "csISOLatin2", 	"ISO8859-2", "ISO8859_2", "" },
{"ISO-8859-3", "ISO_8859-3", "ISO_8859-3:1988", "ISO-IR-109", "LATIN3", "L3", "csISOLatin3", 	"ISO8859-3", "ISO8859_3", "" },
{"ISO-8859-4", "ISO_8859-4", "ISO_8859-4:1988", "ISO-IR-110", "LATIN4", "L4", "csISOLatin4", 	"ISO8859-4", "ISO8859_4", "" },
{"ISO-8859-5", "ISO_8859-5", "ISO_8859-5:1988", "ISO-IR-144", "CYRILLIC", "csISOLatinCyrillic","ISO8859-5", "ISO8859_5", "" },
{"ISO-8859-6", "ISO_8859-6", "ISO_8859-6:1987", "ISO-IR-127", "ARABIC",   "csISOLatinArabic", 	"ISO8859-6", "ISO8859_6", "ECMA-114", "ASMO-708", "" },
{"ISO-8859-7", "ISO_8859-7", "ISO_8859-7:1987", "ISO-IR-126", "GREEK",    "csISOLatinGreek", 	"ISO8859-7", "ISO8859_7", "ECMA-118", "ELOT_928", "GREEK8", "" },
{"ISO-8859-8", "ISO_8859-8", "ISO_8859-8:1988", "ISO-IR-138", "HEBREW",   "csISOLatinHebrew",   "ISO8859-8", "ISO8859_8", "" },
{"ISO-8859-9", "ISO_8859-9", "ISO_8859-9:1989", "ISO-IR-148", "LATIN5", "L5", "csISOLatin5",    "ISO8859-9", "ISO8859_9", "" },
{"ISO-8859-10","ISO_8859-10","ISO_8859-10:1992","ISO-IR-157", "LATIN6", "L6", "csISOLatin6",    "ISO8859-10", "" },
{"ISO-8859-13","ISO_8859-13",                   "ISO-IR-179", "LATIN7", "L7",                   "ISO8859-13", "" },
{"ISO-8859-14","ISO_8859-14","ISO_8859-14:1998","ISO-IR-199", "LATIN8", "L8", 	"ISO-CELTIC",   "ISO8859-14", "" },
{"ISO-8859-15","ISO_8859-15","ISO_8859-15:1998","ISO-IR-203",									"ISO8859-15", "" },
{"ISO-8859-16","ISO_8859-16","ISO_8859-16:2000","ISO-IR-226",					  				"ISO8859-16", "" },
/* Cyrillic 8-bit KOI encodings */
{"KOI8-R",  "csKOI8R", "" },
{"KOI8-U",  "" },
{"KOI8-RU", "" },
/* Windows 8-bit encodings */
{"CP1250", "WINDOWS-1250", "MS-EE",  "" },
{"CP1251", "WINDOWS-1251", "MS-CYRL","" },
{"CP1252", "WINDOWS-1252", "MS-ANSI","" }
};
#endif

#endif


ASSupportedCharsets 
parse_charset_name( const char *name )
{
#ifdef  I18N
	ASSupportedCharsets set = 0;
	if( name == NULL || name[0] == '\0' || name[1] == '\0' ) /* that includes locale "C" */
		return CHARSET_ISO8859_1 ;
     
	if( name[0] == 'L' || name[0] == 'l' ) /* L. or Latin... */
	{
		char latin_n = name[1] ;
		if( strncasecmp( &name[1], "ATIN", 4 ) == 0 ) 
			latin_n = name[5] ;
		switch( latin_n ) 
		{  /* L# latins : */
			case '1' : return CHARSET_ISO8859_1;
			case '2' : return CHARSET_ISO8859_2;
			case '3' : return CHARSET_ISO8859_3;
			case '4' : return CHARSET_ISO8859_4;
			case '5' : return CHARSET_ISO8859_9;
			case '6' : return CHARSET_ISO8859_10;
			case '7' : return CHARSET_ISO8859_13;
			case '8' : return CHARSET_ISO8859_14;
		}
		return CHARSET_ISO8859_1;
	}else if( name[0] == 'I' || name[0] == 'i' ) /* ISO... or IBM819*/
	{
		if( name[1] == 'S' && name[1] == 's' )
			if( name[2] == 'O' && name[2] == 'o' )
			{
				int pos = ( name[3] == '-' || name[3] == '_' )?4:3 ;
				if( name[pos] == '8' )
				{
					if( name[++pos] == '8' )
						if( name[++pos] == '5' )
							if( name[++pos] == '9' )
							{
								pos += 2 ;
								switch( name[pos] ) 
								{
									case '1' : 
										{	switch(name[pos+1] )
											{	case '0' : return CHARSET_ISO8859_10;
												case '1' : 
												case '2' : break;
												case '3' : return CHARSET_ISO8859_13;
												case '4' : return CHARSET_ISO8859_14;
												case '5' : return CHARSET_ISO8859_15;
												case '6' : return CHARSET_ISO8859_16;
											}
										}
										return CHARSET_ISO8859_1;
									case '2' : return CHARSET_ISO8859_2;
									case '3' : return CHARSET_ISO8859_3;
				  					case '4' : return CHARSET_ISO8859_4;
									case '5' : return CHARSET_ISO8859_5;
									case '6' : return CHARSET_ISO8859_6;
									case '7' : return CHARSET_ISO8859_7;
									case '8' : return CHARSET_ISO8859_8;
									case '9' : return CHARSET_ISO8859_9;
								}
							}
				}else if( strncasecmp( &name[pos], "IR-", 3 ) == 0 )
				{
					pos += 3 ;
					switch( name[pos+2] )
					{
						case '0' : if( name[pos+1] == '0' ) break;
							return CHARSET_ISO8859_4;
						case '1' : return CHARSET_ISO8859_2;
						case '2' : break;
						case '3' : return CHARSET_ISO8859_15;
	  					case '4' : return CHARSET_ISO8859_5;
						case '5' : break;
						case '6' : return (name[pos]  =='2')?CHARSET_ISO8859_16:CHARSET_ISO8859_7;
						case '7' : return (name[pos+1]=='2')?CHARSET_ISO8859_6:CHARSET_ISO8859_10;
						case '8' : return (name[pos+1]=='3')?CHARSET_ISO8859_8:CHARSET_ISO8859_9;
						case '9' : return (name[pos+1]=='0')?CHARSET_ISO8859_3:
						                  ((name[pos+1]=='7')?CHARSET_ISO8859_13:CHARSET_ISO8859_14);
					}
				}
			}
		return CHARSET_ISO8859_1;
	}else if( name[0] == 'C' || name[0] == 'c' ) /* cs or CP ... or CYRILLIC*/
	{
		if( name[1] == 'S' || name[1] == 's' )
		{/* cs* */
			if( strncasecmp( &name[2], "KOI8", 4 ) == 0 )
				return CHARSET_KOI8_R ;
			if( strncasecmp( &name[2], "ISOLatin", 8 ) == 0 )
			{
				switch( name[10] ) 
				{
					case '1' : return CHARSET_ISO8859_1;
					case '2' : return CHARSET_ISO8859_2;
					case '3' : return CHARSET_ISO8859_3;
					case '4' : return CHARSET_ISO8859_4;
					case '5' : return CHARSET_ISO8859_9;
					case '6' : return CHARSET_ISO8859_10;
					case '7' : return CHARSET_ISO8859_13;
					case '8' : return CHARSET_ISO8859_14;
				}
				if( name[10] == 'A' || name[10] == 'a' )
					return CHARSET_ISO8859_6;
				if( name[10] == 'C' || name[10] == 'c' )
					return CHARSET_ISO8859_5;
				if( name[10] == 'H' || name[10] == 'h' )
					return CHARSET_ISO8859_8;
				if( name[10] == 'G' || name[10] == 'g' )
					return CHARSET_ISO8859_7;
			}
			return CHARSET_ISO8859_1;
		}else if( name[1] == 'P' || name[1] == 'p' )
		{/* CP- */
			if( strncmp( &name[2], "125", 3 ) == 0 ) 
			{
				if( name[5] == '1')
					return CHARSET_CP1251;
				if( name[5] == '2')	
					return CHARSET_CP1252;
				return CHARSET_CP1250;
			}
			return CHARSET_ISO8859_1;	
		}
		return CHARSET_ISO8859_5 ; /* CYRILLIC */
	}else if( name[0] == 'K' || name[0] == 'k' ) /* KOI... */
	{
		if( strncasecmp( &name[1], "OI8-", 4) == 0 )
		{
			if( name[5] == 'U' || name[5] == 'u' )
				return CHARSET_KOI8_U;
			if( name[5] == 'R' || name[5] == 'r' )
			    if( name[6] == 'U' || name[6] == 'u' )
					return CHARSET_KOI8_RU;
		}
		return CHARSET_KOI8_R ;
	}else if( name[0] == 'E' || name[0] == 'e' ) /* ECMA... */
	{
		if( strncasecmp( &name[1], "CMA-11", 6 ) == 0 ) 
		{
			if( name[7] == '4' )
				return CHARSET_ISO8859_6 ;
		}
		/* ELOT_928 or ECMA-118 */
		return CHARSET_ISO8859_7 ;
	}else if( name[0] == 'M' || name[0] == 'm' ) /* MS-... */
	{
		if( name[1] == 'S' || name[1] == 's' ) /* MS-... */
			if( name[2] == '-' )
			{
				if( name[3] == 'C' || name[3] == 'c' )
					return CHARSET_CP1251 ;
				if( name[3] == 'A' || name[3] == 'a' )
					return CHARSET_CP1252 ;
			}	
		set = CHARSET_CP1250 ;
	}else if( name[0] == 'A' || name[0] == 'a' ) /* ARABIC or ASMO-708 */
	{
		return CHARSET_ISO8859_6 ;
	}else if( name[0] == 'G' || name[0] == 'g' ) /* GREEK or GREEK8 */
	{
		/* if( strncasecmp( &name[1], "REEK", 4 ) == 0 ) */
		return CHARSET_ISO8859_7 ;
	}else if( name[0] == 'H' || name[0] == 'h' ) /* HEBREW */
	{
		/* if( strncasecmp( &name[1], "EBREW", 5 ) == 0 ) */
		return CHARSET_ISO8859_8 ;
	}
	
#if 0
	while( set < SUPPORTED_CHARSETS_NUM )
	{
		char **aliases =&(_as_charset_names[set][0]) ;
		register int i = 0 ;
		char c;
		while( (c = aliases[i][0]) != '\0' )
		{
			if( c == name[1] || tolower(c) == name[0] )
				if( strcasecmp( aliases[i], name ) == 0 )
					return set;
			++i ;
		}
		++set;
	}
#endif
#endif
	return CHARSET_ISO8859_1 ;
}


const unsigned short *as_current_charset = &_as_iso8859_1_2uni[0];

ASSupportedCharsets 
as_set_charset( ASSupportedCharsets new_charset )
{
#ifdef  I18N

	if( new_charset < 0 || new_charset >= SUPPORTED_CHARSETS_NUM ) 
		new_charset = CHARSET_ISO8859_1 ;

	as_current_charset = _as_supported_charsets[new_charset] ;
	return new_charset ;
#else
	return CHARSET_ISO8859_1 ;
#endif
}
