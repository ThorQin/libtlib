/************************************************************************
*
*  LibTLib
*  Copyright (C) 2010  Thor Qin
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*
* Author: Thor Qin
* Bug Report: thor.qin@gmail.com
*
**************************************************************************/

#include "../include/tlib/encode.h"
#include "../include/tlib/binary.h"
#include "../include/tlib/strfunc.h"
#include "md5.h"
#include <string.h>

namespace tlib {

static const unsigned char hex_decode_table[256] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
	0,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const unsigned char hex_decode_mask[256] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
	0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


bool parse_hex(const char* str, size_t len, wchar_t& wch) {
	wch = 0;
	size_t i = 0;
	while (i < len) {
		if (hex_decode_mask[(unsigned int) str[i]]) {
			wch <<= 4;
			wch |= hex_decode_table[(unsigned int) str[i]];
			i++;
		} else
			return false;
	}
	return true;
}

wchar_t parse_hex(const char* str, size_t len) {
	wchar_t wch = 0;
	size_t i = 0;
	while (i < len) {
		wch <<= 4;
		wch |= hex_decode_table[(unsigned int) str[i]];
		i++;
	}
	return wch;
}

bool parse_dec(const char* str, size_t len, wchar_t& wch) {
	wch = 0;
	size_t i = 0;
	while (i < len) {
		if (_isdigit(str[i])) {
			wch *= 10;
			wch += hex_decode_table[(unsigned int) str[i]];
			i++;
		} else
			return false;
	}
	return true;
}

wchar_t parse_dec(const char* str, size_t len) {
	wchar_t wch = 0;
	size_t i = 0;
	while (i < len) {
		wch *= 10;
		wch += hex_decode_table[(unsigned int) str[i]];
		i++;
	}
	return wch;
}


namespace base64 {

//Base64 encode table
static const char base64_encode_table[64] = {
	65,  66,  67,  68,  69,  70,  71,  72,		//  00 -  07
	73,  74,  75,  76,  77,  78,  79,  80,		//  08 -  15
	81,  82,  83,  84,  85,  86,  87,  88,		//  16 -  23
	89,  90,  97,  98,  99,  100, 101, 102,		//  24 -  31
	103, 104, 105, 106, 107, 108, 109, 110,		//  32 -  39
	111, 112, 113, 114, 115, 116, 117, 118,		//  40 -  47
	119, 120, 121, 122, 48,  49,  50,  51,		//  48 -  55
	52,  53,  54,  55,  56,  57,  43,  47		//  56 -  63
};

//Base64 decode table
static const unsigned char base64_decode_table[256] = {
	255, 255, 255, 255, 255, 255, 255, 255,		//  00 -  07
	255, 255, 255, 255, 255, 255, 255, 255,		//  08 -  15
	255, 255, 255, 255, 255, 255, 255, 255,		//  16 -  23
	255, 255, 255, 255, 255, 255, 255, 255,		//  24 -  31
	255, 255, 255, 255, 255, 255, 255, 255,		//  32 -  39
	255, 255, 255, 62,  255, 255, 255, 63,		//  40 -  47
	52,  53,  54,  55,  56,  57,  58,  59,		//  48 -  55
	60,  61,  255, 255, 255, 255, 255, 255,		//  56 -  63
	255, 0,   1,   2,   3,   4,   5,   6,		//  64 -  71
	7,   8,   9,   10,  11,  12,  13,  14,		//  72 -  79
	15,  16,  17,  18,  19,  20,  21,  22,		//  80 -  87
	23,  24,  25,  255, 255, 255, 255, 255,		//  88 -  95
	255, 26,  27,  28,  29,  30,  31,  32,		//  96 - 103
	33,  34,  35,  36,  37,  38,  39,  40,		// 104 - 111
	41,  42,  43,  44,  45,  46,  47,  48,		// 112 - 119
	49,  50,  51,  255, 255, 255, 255, 255,		// 120 - 127
	255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255
};

std::string encode(const binary& data, bool column_limited) {
	std::string result;
	if (data.size() == 0) {
		return result;
	}
	size_t out_len = (data.size() + 2) / 3 * 4;
	size_t column_max;
	size_t line;
	if (column_limited) {
		column_max = 76;
		line = out_len / column_max + (out_len % column_max > 0 ? 1 : 0);
	} else {
		column_max = (size_t) -1;
		line = 1;
	}
	out_len += (line * 2);

	result.clear();
	result.reserve(out_len);

	unsigned int value = 0;
	const unsigned char* scan = begin_ptr(data);
	const unsigned char* end = end_ptr(data);
	const unsigned char* safe_end = end - 3;
	unsigned char column = 0;

	while (scan <= safe_end) {
		// Input.
		value = *scan++;
		value <<= 8;
		value |= *scan++;
		value <<= 8;
		value |= *scan++;

		if (column == column_max) {
			result.push_back('\r');
			result.push_back('\n');
			column = 0;
		}
		result.push_back(base64_encode_table[value >> 18]);
		if (++column == column_max) {
			result.push_back('\r');
			result.push_back('\n');
			column = 0;
		}
		result.push_back(base64_encode_table[(value >> 12) & 0x3F]);
		if (++column == column_max) {
			result.push_back('\r');
			result.push_back('\n');
			column = 0;
		}
		result.push_back(base64_encode_table[(value >> 6) & 0x3F]);
		if (++column == column_max) {
			result.push_back('\r');
			result.push_back('\n');
			column = 0;
		}
		result.push_back(base64_encode_table[value & 0x3F]);
		column++;
	}
	switch (end - scan) {
	case 1:
		// Input.
		value = *scan++;
		value <<= 16;

		if (column == column_max) {
			result.push_back('\r');
			result.push_back('\n');
			column = 0;
		}
		result.push_back(base64_encode_table[value >> 18]);
		if (++column == column_max) {
			result.push_back('\r');
			result.push_back('\n');
			column = 0;
		}
		result.push_back(base64_encode_table[(value >> 12) & 0x3F]);
		if (++column == column_max) {
			result.push_back('\r');
			result.push_back('\n');
			column = 0;
		}
		result.push_back('=');
		if (++column == column_max) {
			result.push_back('\r');
			result.push_back('\n');
			column = 0;
		}
		result.push_back('=');
		break;
	case 2:
		// Input.
		value = *scan++;
		value <<= 8;
		value |= *scan++;
		value <<= 8;

		if (column == column_max) {
			result.push_back('\r');
			result.push_back('\n');
			column = 0;
		}
		result.push_back(base64_encode_table[value >> 18]);
		if (++column == column_max) {
			result.push_back('\r');
			result.push_back('\n');
			column = 0;
		}
		result.push_back(base64_encode_table[(value >> 12) & 0x3F]);
		if (++column == column_max) {
			result.push_back('\r');
			result.push_back('\n');
			column = 0;
		}
		result.push_back(base64_encode_table[(value >> 6) & 0x3F]);
		if (++column == column_max) {
			result.push_back('\r');
			result.push_back('\n');
			column = 0;
		}
		result.push_back('=');
		break;
	}
	return result;
}

binary decode(const std::string& src) {
	binary result;
	size_t len = ((src.length() + 3) / 4 + 1) * 3;
	result.reserve(len);

	const char* scan = src.c_str();
	const char* scan_end = scan + src.length();
	unsigned int value = 0;
	unsigned char valid_char = 0;
	unsigned char pos = 0;
	while (scan < scan_end) {
		valid_char = base64_decode_table[(int) *scan++];
		if (valid_char == 0xFF)
			continue;

		value <<= 6;
		value |= valid_char;
		if (++pos == 4) {
			result.push_back((unsigned char) (value >> 16));
			result.push_back((unsigned char) ((value >> 8) & 0xFF));
			result.push_back((unsigned char) (value & 0xFF));
			value = 0;
			pos = 0;
		}
	}

	switch (pos) {
	case 2:
		result.push_back((unsigned char) (value >> 4));
		break;
	case 3:
		result.push_back((unsigned char) (value >> 10));
		result.push_back((unsigned char) ((value >> 2) & 0xFF));
		break;
	}
	return result;
}

} //end namespace base64


namespace qprint {

std::string encode(const binary& data, bool column_limited) {
	std::string result;
	static const char* hex_table = "0123456789ABCDEF";
	const unsigned char* scan = begin_ptr(data);
	const unsigned char* scan_end = end_ptr(data);
	size_t column_max;
	if (column_limited)
		column_max = 76;
	else
		column_max = (size_t) -1;
	result.clear();
	result.reserve(data.size());
	size_t column = 0;
	while (scan < scan_end) {
		if ((*scan >= 33 && *scan <= 60) || (*scan >= 62 && *scan <= 126)) {
			if (column + 1 < column_max) {
				result.push_back(*scan);
				column++;
			} else {
				result.push_back('=');
				result.push_back('\r');
				result.push_back('\n');
				result.push_back(*scan);
				column = 1;
			}
		} else if (*scan == ' ' || *scan == '\t') {
			if (scan == scan_end - 1) {
				if (column + 3 < column_max) {
					result.push_back('=');
					result.push_back(hex_table[*scan >> 4]);
					result.push_back(hex_table[*scan & 0x0F]);
					column += 3;
				} else {
					result.push_back('=');
					result.push_back('\r');
					result.push_back('\n');
					result.push_back('=');
					result.push_back(hex_table[*scan >> 4]);
					result.push_back(hex_table[*scan & 0x0F]);
					column = 3;
				}
			} else {
				if (column + 1 < column_max) {
					result.push_back(*scan);
					column++;
				} else {
					result.push_back('=');
					result.push_back('\r');
					result.push_back('\n');
					result.push_back(*scan);
					column = 1;
				}
			}
		} else {
			if (column + 3 < column_max) {
				result.push_back('=');
				result.push_back(hex_table[*scan >> 4]);
				result.push_back(hex_table[*scan & 0x0F]);
				column += 3;
			} else {
				result.push_back('=');
				result.push_back('\r');
				result.push_back('\n');
				column = 0;
				result.push_back('=');
				result.push_back(hex_table[*scan >> 4]);
				result.push_back(hex_table[*scan & 0x0F]);
				column += 3;
			}
		}
		scan++;
	}
	return result;
}

binary decode(const std::string& src) {
	binary result;
	const char* scan = src.c_str();
	const char* scan_end = scan + src.length();
	char char_value = 0;
	int state = 0;
	result.clear();
	result.reserve(src.length());
	std::string space_text;
	while (scan < scan_end) {
		if (state == 0) {
			if (*scan == '=')
				state = 1;
			else if (isspace(*scan)) {
				space_text.push_back(*scan);
				state = 3;
			} else
				result.push_back(*scan);
		} else if (state == 1) {
			if (hex_decode_mask[(int) *scan]) {
				char_value = hex_decode_table[(int) *scan];
				char_value <<= 4;
				state = 2;
			} else if (*scan == '\n') {
				state = 0;
			} else if (isspace(*scan)) {
			} else {
				result.push_back(*scan);
				state = 0;
			}
		} else if (state == 2) {
			if (hex_decode_mask[(int) *scan]) {
				char_value |= hex_decode_table[(int) *scan];
				result.push_back(char_value);
				state = 0;
			} else {
				result.push_back(*(scan - 1));
				result.push_back(*scan);
				state = 0;
			}
		} else {
			if (*scan == '=') {
				result << space_text;
				space_text.clear();
				state = 1;
			} else if (isspace(*scan)) {
				space_text.push_back(*scan);
			} else {
				result << space_text;
				space_text.clear();
				result.push_back(*scan);
				state = 0;
			}
		}
		scan++;
	}
	return result;
}

} // End namespace qprint


namespace hash {

binary md5(const binary& data) {
	unsigned char buffer[16];
	MD5_CTX context;
	MD5Init(&context);
	MD5Update(&context, begin_ptr(data), (unsigned int) data.size());
	MD5Final(buffer, &context);
	return binary(buffer, buffer + 16);
}

binary hmac_md5(const binary& data, const binary& key) {
	unsigned char inner_pad[65];
	unsigned char outer_pad[65];
	unsigned char buffer[16];
	memset(inner_pad, 0, 65);
	memset(outer_pad, 0, 65);
	if (key.size() > 64) {
		binary key_md5 = md5(key);
		memcpy(inner_pad, begin_ptr(key_md5), key_md5.size());
		memcpy(outer_pad, begin_ptr(key_md5), key_md5.size());
	} else {
		memcpy(inner_pad, begin_ptr(key), key.size());
		memcpy(outer_pad, begin_ptr(key), key.size());
	}
	for (int i = 0; i < 64; i++) {
		inner_pad[i] ^= 0x36;
		outer_pad[i] ^= 0x5c;
	}
	MD5_CTX context;
	MD5Init(&context);
	MD5Update(&context, inner_pad, 64);
	MD5Update(&context, begin_ptr(data), (unsigned int) data.size());
	MD5Final(buffer, &context);

	MD5Init(&context);
	MD5Update(&context, outer_pad, 64);
	MD5Update(&context, buffer, 16);
	MD5Final(buffer, &context);

	return binary(buffer, buffer + 16);
}

} // End of namespace hash

} // End of namespace tlib
