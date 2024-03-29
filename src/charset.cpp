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

#include "../include/tlib/strfunc.h"
#include "../include/tlib/charset.h"
#include "../include/tlib/binary.h"

#ifdef __GNUC__
#  include <glib.h>
#elif defined(__MSVC__)
#  ifndef _WINSOCKAPI_
#    define _WINSOCKAPI_
#  endif
#  define _WIN32_WINNT 0x0500
#  define WINVER 0x0500
#  include <windows.h>
#  pragma warning( disable : 4290 4996 4267)
#endif

namespace tlib {

std::locale __tlib_c_locale("C");

typedef struct _codepage_charset {
	unsigned int code_page;
	const char* charset;
} CodepageCharset;

typedef struct _charset_codepage {
	const char* charset;
	unsigned int code_page;
} CharsetCodepage;

inline bool operator <(const CodepageCharset& l, const CodepageCharset& r);

static const int g_codepage_number = 148;
static const CodepageCharset codepage_map[g_codepage_number] = {
	{ 37, "ibm037"},
	{ 437, "ibm437"},
	{ 500, "ibm500"},
	{ 708, "asmo-708"},
	{ 720, "dos-720"},
	{ 737, "ibm737"},
	{ 775, "ibm775"},
	{ 850, "ibm850"},
	{ 852, "ibm852"},
	{ 855, "ibm855"},
	{ 857, "ibm857"},
	{ 858, "ibm00858"},
	{ 860, "ibm860"},
	{ 861, "ibm861"},
	{ 862, "dos-862"},
	{ 863, "ibm863"},
	{ 864, "ibm864"},
	{ 865, "ibm865"},
	{ 866, "cp866"},
	{ 869, "ibm869"},
	{ 870, "ibm870"},
	{ 874, "windows-874"},
	{ 875, "cp875"},
	{ 932, "shift_jis"},
	{ 936, "gbk"},
	{ 949, "ks_c_5601-1987"},
	{ 950, "big5"},
	{ 1026, "ibm1026"},
	{ 1047, "ibm01047"},
	{ 1140, "ibm01140"},
	{ 1141, "ibm01141"},
	{ 1142, "ibm01142"},
	{ 1143, "ibm01143"},
	{ 1144, "ibm01144"},
	{ 1145, "ibm01145"},
	{ 1146, "ibm01146"},
	{ 1147, "ibm01147"},
	{ 1148, "ibm01148"},
	{ 1149, "ibm01149"},
	{ 1200, "utf-16"},
	{ 1201, "unicodefffe"},
	{ 1250, "windows-1250"},
	{ 1251, "windows-1251"},
	{ 1252, "windows-1252"},
	{ 1253, "windows-1253"},
	{ 1254, "windows-1254"},
	{ 1255, "windows-1255"},
	{ 1256, "windows-1256"},
	{ 1257, "windows-1257"},
	{ 1258, "windows-1258"},
	{ 1361, "johab"},
	{ 10000, "macintosh"},
	{ 10001, "x-mac-japanese"},
	{ 10002, "x-mac-chinesetrad"},
	{ 10003, "x-mac-korean"},
	{ 10004, "x-mac-arabic"},
	{ 10005, "x-mac-hebrew"},
	{ 10006, "x-mac-greek"},
	{ 10007, "x-mac-cyrillic"},
	{ 10008, "x-mac-chinesesimp"},
	{ 10010, "x-mac-romanian"},
	{ 10017, "x-mac-ukrainian"},
	{ 10021, "x-mac-thai"},
	{ 10029, "x-mac-ce"},
	{ 10079, "x-mac-icelandic"},
	{ 10081, "x-mac-turkish"},
	{ 10082, "x-mac-croatian"},
	{ 12000, "utf-32"},
	{ 12001, "utf-32be"},
	{ 20000, "x-chinese_cns"},
	{ 20001, "x-cp20001"},
	{ 20002, "x_chinese-eten"},
	{ 20003, "x-cp20003"},
	{ 20004, "x-cp20004"},
	{ 20005, "x-cp20005"},
	{ 20105, "x-ia5"},
	{ 20106, "x-ia5-german"},
	{ 20107, "x-ia5-swedish"},
	{ 20108, "x-ia5-norwegian"},
	{ 20127, "us-ascii"},
	{ 20261, "x-cp20261"},
	{ 20269, "x-cp20269"},
	{ 20273, "ibm273"},
	{ 20277, "ibm277"},
	{ 20278, "ibm278"},
	{ 20280, "ibm280"},
	{ 20284, "ibm284"},
	{ 20285, "ibm285"},
	{ 20290, "ibm290"},
	{ 20297, "ibm297"},
	{ 20420, "ibm420"},
	{ 20423, "ibm423"},
	{ 20424, "ibm424"},
	{ 20833, "x-ebcdic-koreanextended"},
	{ 20838, "ibm-thai"},
	{ 20866, "koi8-r"},
	{ 20871, "ibm871"},
	{ 20880, "ibm880"},
	{ 20905, "ibm905"},
	{ 20924, "ibm00924"},
	{ 20932, "euc-jp"},
	{ 20936, "x-cp20936"},
	{ 20949, "x-cp20949"},
	{ 21025, "cp1025"},
	{ 21866, "koi8-u"},
	{ 28591, "iso-8859-1"},
	{ 28592, "iso-8859-2"},
	{ 28593, "iso-8859-3"},
	{ 28594, "iso-8859-4"},
	{ 28595, "iso-8859-5"},
	{ 28596, "iso-8859-6"},
	{ 28597, "iso-8859-7"},
	{ 28598, "iso-8859-8"},
	{ 28599, "iso-8859-9"},
	{ 28603, "iso-8859-13"},
	{ 28605, "iso-8859-15"},
	{ 29001, "x-europa"},
	{ 38598, "iso-8859-8-i"},
	{ 50220, "iso-2022-jp"},
	{ 50221, "csiso2022jp"},
	{ 50222, "iso-2022-jp"},
	{ 50225, "iso-2022-kr"},
	{ 50227, "x-cp50227"},
	{ 50229, "iso-2022-cn-ext"},
	{ 50930, "x-ebcdic-japaneseandkana"},
	{ 50931, "x-ebcdic-japaneseanduscanada"},
	{ 50933, "x-ebcdic-koreanandkoreanextended"},
	{ 50935, "x-ebcdic-simplifiedchinese"},
	{ 50936, "x-ebcdic-simplifiedchinese"},
	{ 50937, "x-ebcdic-traditionalchinese"},
	{ 50939, "x-ebcdic-japaneseandjapaneselatin"},
	{ 51932, "euc-jp"},
	{ 51936, "euc-cn"},
	{ 51949, "euc-kr"},
	{ 52936, "hz-gb-2312"},
	{ 54936, "gb18030"},
	{ 57002, "x-iscii-de"},
	{ 57003, "x-iscii-be"},
	{ 57004, "x-iscii-ta"},
	{ 57005, "x-iscii-te"},
	{ 57006, "x-iscii-as"},
	{ 57007, "x-iscii-or"},
	{ 57008, "x-iscii-ka"},
	{ 57009, "x-iscii-ma"},
	{ 57010, "x-iscii-gu"},
	{ 57011, "x-iscii-pa"},
	{ 65000, "utf-7"},
	{ 65001, "utf-8"}
};
static const int g_charset_number = 547;
static const CharsetCodepage charset_map[g_charset_number] = {
	{ "437", 437 },
	{ "850", 850 },
	{ "852", 852 },
	{ "855", 855 },
	{ "857", 857 },
	{ "860", 860 },
	{ "861", 861 },
	{ "862", 862 },
	{ "863", 863 },
	{ "865", 865 },
	{ "866", 866 },
	{ "869", 869 },
	{ "_iso-2022-jp", 50221 },
	{ "_iso-2022-jp$sio", 50222 },
	{ "ansi_x3.4-1968", 20127 },
	{ "ansi_x3.4-1986", 20127 },
	{ "arabic", 28596 },
	{ "arabic7", 709 },
	{ "ascii", 20127 },
	{ "asmo-708", 708 },
	{ "asmo_449", 709 },
	{ "big5", 950 },
	{ "big5-hkscs", 950 },
	{ "ccsid00858", 858 },
	{ "ccsid00924", 20924 },
	{ "ccsid01140", 1140 },
	{ "ccsid01141", 1141 },
	{ "ccsid01142", 1142 },
	{ "ccsid01143", 1143 },
	{ "ccsid01144", 1144 },
	{ "ccsid01145", 1145 },
	{ "ccsid01146", 1146 },
	{ "ccsid01147", 1147 },
	{ "ccsid01148", 1148 },
	{ "ccsid01149", 1149 },
	{ "chinese", 936 },
	{ "cn-big5", 950 },
	{ "cn-gb", 936 },
	{ "codepage437", 437 },
	{ "cp-gr", 869 },
	{ "cp-is", 861 },
	{ "cp00858", 858 },
	{ "cp00924", 20924 },
	{ "cp01140", 1140 },
	{ "cp01141", 1141 },
	{ "cp01142", 1142 },
	{ "cp01143", 1143 },
	{ "cp01144", 1144 },
	{ "cp01145", 1145 },
	{ "cp01146", 1146 },
	{ "cp01147", 1147 },
	{ "cp01148", 1148 },
	{ "cp01149", 1149 },
	{ "cp037", 37 },
	{ "cp1025", 21025 },
	{ "cp1026", 1026 },
	{ "cp1125", 1125 },
	{ "cp1133", 1133 },
	{ "cp1200", 1200 },
	{ "cp12000", 12000 },
	{ "cp12001", 12001 },
	{ "cp1201", 1201 },
	{ "cp1250", 1250 },
	{ "cp1251", 1251 },
	{ "cp1252", 1252 },
	{ "cp1253", 1253 },
	{ "cp1254", 1254 },
	{ "cp1255", 1255 },
	{ "cp1256", 1256 },
	{ "cp1257", 1257 },
	{ "cp1258", 1258 },
	{ "cp1361", 1361 },
	{ "cp154", 154 },
	{ "cp273", 20273 },
	{ "cp278", 20278 },
	{ "cp280", 20280 },
	{ "cp284", 20284 },
	{ "cp285", 20285 },
	{ "cp290", 20290 },
	{ "cp297", 20297 },
	{ "cp367", 20127 },
	{ "cp420", 20420 },
	{ "cp423", 20423 },
	{ "cp424", 20424 },
	{ "cp437", 437 },
	{ "cp500", 775 },
	{ "cp50221", 50221 },
	{ "cp51932", 51932 },
	{ "cp65001", 65001 },
	{ "cp737", 737 },
	{ "cp775", 775 },
	{ "cp819", 28591 },
	{ "cp850", 850 },
	{ "cp852", 852 },
	{ "cp853", 853 },
	{ "cp855", 855 },
	{ "cp857", 857 },
	{ "cp858", 858 },
	{ "cp860", 860 },
	{ "cp861", 861 },
	{ "cp862", 862 },
	{ "cp863", 863 },
	{ "cp864", 864 },
	{ "cp865", 865 },
	{ "cp866", 866 },
	{ "cp869", 869 },
	{ "cp870", 870 },
	{ "cp871", 20871 },
	{ "cp874", 874 },
	{ "cp875", 875 },
	{ "cp880", 20880 },
	{ "cp905", 20905 },
	{ "cp932", 932 },
	{ "cp936", 936 },
	{ "cp949", 949 },
	{ "cp950", 950 },
	{ "csascii", 20127 },
	{ "csbig5", 950 },
	{ "cseuckr", 51949 },
	{ "cseucpkdfmtjapanese", 20932 },
	{ "csgb2312", 936 },
	{ "csgb231280", 936 },
	{ "csibm037", 37 },
	{ "csibm1026", 1026 },
	{ "csibm273", 20273 },
	{ "csibm277", 20277 },
	{ "csibm278", 20278 },
	{ "csibm280", 20280 },
	{ "csibm284", 20284 },
	{ "csibm285", 20285 },
	{ "csibm290", 20290 },
	{ "csibm297", 20297 },
	{ "csibm420", 20420 },
	{ "csibm423", 20423 },
	{ "csibm424", 20424 },
	{ "csibm500", 775 },
	{ "csibm855", 855 },
	{ "csibm857", 857 },
	{ "csibm860", 860 },
	{ "csibm861", 861 },
	{ "csibm863", 863 },
	{ "csibm864", 864 },
	{ "csibm865", 865 },
	{ "csibm866", 866 },
	{ "csibm869", 869 },
	{ "csibm870", 870 },
	{ "csibm871", 20871 },
	{ "csibm880", 20880 },
	{ "csibm905", 20905 },
	{ "csibmthai", 20838 },
	{ "csiso2022jp", 50221 },
	{ "csiso2022kr", 50225 },
	{ "csiso58gb231280", 936 },
	{ "csiso89asmo449", 709 },
	{ "csisolatin1", 28591 },
	{ "csisolatin2", 28592 },
	{ "csisolatin3", 28593 },
	{ "csisolatin4", 28594 },
	{ "csisolatin5", 28595 },
	{ "csisolatinarabic", 28596 },
	{ "csisolatincyrillic", 28595 },
	{ "csisolatingreek", 28597 },
	{ "csisolatinhebrew", 28598 },
	{ "cskoi8r", 20866 },
	{ "csksc56011987", 949 },
	{ "csmacintosh", 10000 },
	{ "cspc775baltic", 775 },
	{ "cspc8", 437 },
	{ "cspc850multilingual", 850 },
	{ "cspc862latinhebrew", 862 },
	{ "cspc8codepage437", 437 },
	{ "cspcp852", 852 },
	{ "csptcp154", 154 },
	{ "csshiftjis", 932 },
	{ "csunicode11utf7", 65000 },
	{ "cswindows31j", 932 },
	{ "cyrillic", 28595 },
	{ "cyrillic-asian", 154 },
	{ "dos-720", 720 },
	{ "dos-862", 862 },
	{ "dos-874", 874 },
	{ "ebcdic-cp-ar1", 20420 },
	{ "ebcdic-cp-be", 775 },
	{ "ebcdic-cp-ca", 37 },
	{ "ebcdic-cp-ch", 775 },
	{ "ebcdic-cp-dk", 20277 },
	{ "ebcdic-cp-es", 20284 },
	{ "ebcdic-cp-fi", 20278 },
	{ "ebcdic-cp-fr", 20297 },
	{ "ebcdic-cp-gb", 20285 },
	{ "ebcdic-cp-gr", 20423 },
	{ "ebcdic-cp-he", 20424 },
	{ "ebcdic-cp-is", 20871 },
	{ "ebcdic-cp-it", 20280 },
	{ "ebcdic-cp-nl", 37 },
	{ "ebcdic-cp-no", 20277 },
	{ "ebcdic-cp-roece", 870 },
	{ "ebcdic-cp-se", 20278 },
	{ "ebcdic-cp-tr", 20905 },
	{ "ebcdic-cp-us", 37 },
	{ "ebcdic-cp-wt", 37 },
	{ "ebcdic-cp-yu", 870 },
	{ "ebcdic-cyrillic", 20880 },
	{ "ebcdic-de-273+euro", 1141 },
	{ "ebcdic-dk-277+euro", 1142 },
	{ "ebcdic-es-284+euro", 1145 },
	{ "ebcdic-fi-278+euro", 1143 },
	{ "ebcdic-fr-297+euro", 1147 },
	{ "ebcdic-gb-285+euro", 1146 },
	{ "ebcdic-international-500+euro", 1148 },
	{ "ebcdic-is-871+euro", 1149 },
	{ "ebcdic-it-280+euro", 1144 },
	{ "ebcdic-jp-kana", 20290 },
	{ "ebcdic-latin9--euro", 20924 },
	{ "ebcdic-no-277+euro", 1142 },
	{ "ebcdic-se-278+euro", 1143 },
	{ "ebcdic-us-37+euro", 1140 },
	{ "ecma-114", 28596 },
	{ "ecma-118", 28597 },
	{ "elot_928", 28597 },
	{ "euc-cn", 51936 },
	{ "euc-jp", 20932 },
	{ "euc-kr", 51949 },
	{ "extended_unix_code_packed_format_for_japanese", 20932 },
	{ "gb18030", 54936 },
	{ "gb2312", 936 },
	{ "gb2312-80", 936 },
	{ "gb231280", 936 },
	{ "gb_2312-80", 936 },
	{ "gbk", 936 },
	{ "greek", 28597 },
	{ "greek8", 28597 },
	{ "hebrew", 28598 },
	{ "hz-gb-2312", 52936 },
	{ "ibm-cp1133", 1133 },
	{ "ibm-thai", 20838 },
	{ "ibm00858", 858 },
	{ "ibm00924", 20924 },
	{ "ibm01047", 1047 },
	{ "ibm01140", 1140 },
	{ "ibm01141", 1141 },
	{ "ibm01142", 1142 },
	{ "ibm01143", 1143 },
	{ "ibm01144", 1144 },
	{ "ibm01145", 1145 },
	{ "ibm01146", 1146 },
	{ "ibm01147", 1147 },
	{ "ibm01148", 1148 },
	{ "ibm01149", 1149 },
	{ "ibm037", 37 },
	{ "ibm1026", 1026 },
	{ "ibm273", 20273 },
	{ "ibm277", 20277 },
	{ "ibm278", 20278 },
	{ "ibm280", 20280 },
	{ "ibm284", 20284 },
	{ "ibm285", 20285 },
	{ "ibm290", 20290 },
	{ "ibm297", 20297 },
	{ "ibm367", 20127 },
	{ "ibm420", 20420 },
	{ "ibm423", 20423 },
	{ "ibm424", 20424 },
	{ "ibm437", 437 },
	{ "ibm500", 500 },
	{ "ibm737", 737 },
	{ "ibm775", 775 },
	{ "ibm819", 28591 },
	{ "ibm850", 850 },
	{ "ibm852", 852 },
	{ "ibm855", 855 },
	{ "ibm857", 857 },
	{ "ibm860", 860 },
	{ "ibm861", 861 },
	{ "ibm862", 862 },
	{ "ibm863", 863 },
	{ "ibm864", 864 },
	{ "ibm865", 865 },
	{ "ibm866", 866 },
	{ "ibm869", 869 },
	{ "ibm870", 870 },
	{ "ibm871", 20871 },
	{ "ibm880", 20880 },
	{ "ibm905", 20905 },
	{ "iso-2022-cn", 50229 },
	{ "iso-2022-jp", 50220 },
	{ "iso-2022-jp-ms", 50221 },
	{ "iso-2022-kr", 50225 },
	{ "iso-4873:1986", 54936 },
	{ "iso-8859-1", 28591 },
	{ "iso-8859-11", 874 },
	{ "iso-8859-13", 28603 },
	{ "iso-8859-15", 28605 },
	{ "iso-8859-2", 28592 },
	{ "iso-8859-3", 28593 },
	{ "iso-8859-4", 28594 },
	{ "iso-8859-5", 28595 },
	{ "iso-8859-6", 28596 },
	{ "iso-8859-7", 28597 },
	{ "iso-8859-8", 28598 },
	{ "iso-8859-8-i", 38598 },
	{ "iso-8859-9", 28599 },
	{ "iso-ir-100", 28591 },
	{ "iso-ir-101", 28592 },
	{ "iso-ir-109", 28593 },
	{ "iso-ir-110", 28594 },
	{ "iso-ir-126", 28597 },
	{ "iso-ir-127", 28596 },
	{ "iso-ir-138", 28598 },
	{ "iso-ir-144", 28595 },
	{ "iso-ir-148", 28599 },
	{ "iso-ir-149", 949 },
	{ "iso-ir-58", 936 },
	{ "iso-ir-6", 20127 },
	{ "iso-ir-6us", 20127 },
	{ "iso-ir-89", 709 },
	{ "iso2022-jp-ms", 50221 },
	{ "iso2022-kr", 50225 },
	{ "iso646-us", 20127 },
	{ "iso8859-1", 28591 },
	{ "iso8859-13", 28603 },
	{ "iso8859-15", 28605 },
	{ "iso8859-2", 28592 },
	{ "iso8859-3", 28593 },
	{ "iso8859-4", 28594 },
	{ "iso8859-5", 28595 },
	{ "iso8859-6", 28596 },
	{ "iso8859-7", 28597 },
	{ "iso8859-8", 28598 },
	{ "iso8859-8-i", 38598 },
	{ "iso8859-9", 28599 },
	{ "iso_646.irv:1991", 20127 },
	{ "iso_8859-1", 28591 },
	{ "iso_8859-15", 28605 },
	{ "iso_8859-1:1987", 28591 },
	{ "iso_8859-2", 28592 },
	{ "iso_8859-2:1987", 28592 },
	{ "iso_8859-3", 28593 },
	{ "iso_8859-3:1988", 28593 },
	{ "iso_8859-4", 28594 },
	{ "iso_8859-4:1988", 28594 },
	{ "iso_8859-5", 28595 },
	{ "iso_8859-5:1988", 28595 },
	{ "iso_8859-6", 28596 },
	{ "iso_8859-6:1987", 28596 },
	{ "iso_8859-7", 28597 },
	{ "iso_8859-7:1987", 28597 },
	{ "iso_8859-8", 28598 },
	{ "iso_8859-8:1988", 28598 },
	{ "iso_8859-9", 28599 },
	{ "iso_8859-9:1989", 28599 },
	{ "iso_9036", 709 },
	{ "johab", 1361 },
	{ "koi", 20866 },
	{ "koi8", 20866 },
	{ "koi8-r", 20866 },
	{ "koi8-ru", 21866 },
	{ "koi8-u", 21866 },
	{ "koi8r", 20866 },
	{ "korean", 949 },
	{ "ks-c-5601", 949 },
	{ "ks-c5601", 949 },
	{ "ks_c_5601", 949 },
	{ "ks_c_5601-1987", 949 },
	{ "ks_c_5601-1989", 949 },
	{ "ks_c_5601_1987", 949 },
	{ "ksc5601", 949 },
	{ "ksc_5601", 949 },
	{ "l1", 28591 },
	{ "l2", 28592 },
	{ "l3", 28593 },
	{ "l4", 28594 },
	{ "l5", 28599 },
	{ "l9", 28605 },
	{ "latin-9", 28605 },
	{ "latin1", 28591 },
	{ "latin2", 28592 },
	{ "latin3", 28593 },
	{ "latin4", 28594 },
	{ "latin5", 28599 },
	{ "latin9", 28605 },
	{ "logical", 38598 },
	{ "mac", 10000 },
	{ "macintosh", 10000 },
	{ "ms-ansi", 1252 },
	{ "ms-arab", 1256 },
	{ "ms-cyrl", 1251 },
	{ "ms-ee", 1250 },
	{ "ms-greek", 1253 },
	{ "ms-hebr", 1255 },
	{ "ms-turk", 1254 },
	{ "ms50221", 50221 },
	{ "ms51932", 51932 },
	{ "ms932", 932 },
	{ "ms936", 936 },
	{ "ms_kanji", 932 },
	{ "pc-multilingual-850+euro", 858 },
	{ "pt154", 154 },
	{ "ptcp154", 154 },
	{ "shifft_jis", 932 },
	{ "shifft_jis-ms", 932 },
	{ "shift-jis", 932 },
	{ "shift_jis", 932 },
	{ "sjis", 932 },
	{ "sjis-ms", 932 },
	{ "sjis-open", 932 },
	{ "sjis-win", 932 },
	{ "tis-620", 874 },
	{ "ucs-2", 1200 },
	{ "ucs-2be", 1201 },
	{ "ucs-2le", 1200 },
	{ "ucs-4", 12000 },
	{ "ucs-4be", 12001 },
	{ "ucs-4le", 12000 },
	{ "ucs2", 1200 },
	{ "ucs2be", 1201 },
	{ "ucs4", 12000 },
	{ "ucs4be", 12001 },
	{ "ucs4le", 12000 },
	{ "uhc", 949 },
	{ "unicode", 1200 },
	{ "unicode-1-1-utf-7", 65000 },
	{ "unicode-1-1-utf-8", 65001 },
	{ "unicode-2-0-utf-8", 65001 },
	{ "unicodefffe", 1201 },
	{ "us", 20127 },
	{ "us-ascii", 20127 },
	{ "utf-16", 1200 },
	{ "utf-16be", 1201 },
	{ "utf-16le", 1200 },
	{ "utf-32", 12000 },
	{ "utf-32be", 12001 },
	{ "utf-32le", 12000 },
	{ "utf-7", 65000 },
	{ "utf-8", 65001 },
	{ "utf16", 1200 },
	{ "utf16be", 1201 },
	{ "utf16le", 1200 },
	{ "utf32", 12000 },
	{ "utf32be", 12001 },
	{ "utf32le", 12000 },
	{ "utf8", 65001 },
	{ "visual", 28598 },
	{ "winbaltrim", 1257 },
	{ "windows-1250", 1250 },
	{ "windows-1251", 1251 },
	{ "windows-1252", 1252 },
	{ "windows-1253", 1253 },
	{ "windows-1254", 1254 },
	{ "windows-1255", 1255 },
	{ "windows-1256", 1256 },
	{ "windows-1257", 1257 },
	{ "windows-1258", 1258 },
	{ "windows-31j", 932 },
	{ "windows-50221", 50221 },
	{ "windows-51932", 51932 },
	{ "windows-874", 874 },
	{ "windows-932", 932 },
	{ "windows-936", 936 },
	{ "x-ansi", 1252 },
	{ "x-chinese-cns", 20000 },
	{ "x-chinese-eten", 20002 },
	{ "x-chinese_cns", 20000 },
	{ "x-cp1250", 1250 },
	{ "x-cp1251", 1251 },
	{ "x-cp20001", 20001 },
	{ "x-cp20003", 20003 },
	{ "x-cp20004", 20004 },
	{ "x-cp20005", 20005 },
	{ "x-cp20261", 20261 },
	{ "x-cp20269", 20269 },
	{ "x-cp20936", 20936 },
	{ "x-cp20949", 20949 },
	{ "x-cp50227", 50227 },
	{ "x-ebcdic-arabic", 420 },
	{ "x-ebcdic-cp-us-euro", 1140 },
	{ "x-ebcdic-cyrillicrussian", 20880 },
	{ "x-ebcdic-cyrillicserbianbulgarian", 21025 },
	{ "x-ebcdic-denmarknorway", 20277 },
	{ "x-ebcdic-denmarknorway-euro", 1142 },
	{ "x-ebcdic-finlandsweden", 20278 },
	{ "x-ebcdic-finlandsweden-euro", 1143 },
	{ "x-ebcdic-france", 1143 },
	{ "x-ebcdic-france-euro", 1147 },
	{ "x-ebcdic-germany", 20273 },
	{ "x-ebcdic-germany-euro", 1141 },
	{ "x-ebcdic-greek", 20423 },
	{ "x-ebcdic-greekmodern", 875 },
	{ "x-ebcdic-hebrew", 20424 },
	{ "x-ebcdic-icelandic", 20871 },
	{ "x-ebcdic-icelandic-euro", 1149 },
	{ "x-ebcdic-international-euro", 1148 },
	{ "x-ebcdic-italy", 20280 },
	{ "x-ebcdic-italy-euro", 1144 },
	{ "x-ebcdic-japaneseandjapaneselatin", 50939 },
	{ "x-ebcdic-japaneseandkana", 50930 },
	{ "x-ebcdic-japaneseanduscanada", 50931 },
	{ "x-ebcdic-japanesekatakana", 20290 },
	{ "x-ebcdic-koreanandkoreanextended", 50933 },
	{ "x-ebcdic-koreanextended", 20833 },
	{ "x-ebcdic-simplifiedchinese", 50935 },
	{ "x-ebcdic-spain", 20284 },
	{ "x-ebcdic-spain-euro", 1145 },
	{ "x-ebcdic-thai", 20838 },
	{ "x-ebcdic-traditionalchinese", 50937 },
	{ "x-ebcdic-turkish", 20905 },
	{ "x-ebcdic-uk", 20285 },
	{ "x-ebcdic-uk-euro", 1146 },
	{ "x-euc", 51932 },
	{ "x-euc-cn", 51936 },
	{ "x-euc-jp", 51932 },
	{ "x-europa", 29001 },
	{ "x-ia5", 20105 },
	{ "x-ia5-german", 20106 },
	{ "x-ia5-norwegian", 20108 },
	{ "x-ia5-swedish", 20107 },
	{ "x-iscii-as", 57006 },
	{ "x-iscii-be", 57003 },
	{ "x-iscii-de", 57002 },
	{ "x-iscii-gu", 57010 },
	{ "x-iscii-ka", 57008 },
	{ "x-iscii-ma", 57009 },
	{ "x-iscii-or", 57007 },
	{ "x-iscii-pa", 57011 },
	{ "x-iscii-ta", 57004 },
	{ "x-iscii-te", 57005 },
	{ "x-mac-arabic", 10004 },
	{ "x-mac-ce", 10029 },
	{ "x-mac-chinesesimp", 10008 },
	{ "x-mac-chinesetrad", 10002 },
	{ "x-mac-croatian", 10082 },
	{ "x-mac-cyrillic", 10007 },
	{ "x-mac-greek", 10006 },
	{ "x-mac-hebrew", 10005 },
	{ "x-mac-icelandic", 10079 },
	{ "x-mac-japanese", 10001 },
	{ "x-mac-korean", 10003 },
	{ "x-mac-romanian", 10010 },
	{ "x-mac-thai", 10021 },
	{ "x-mac-turkish", 10081 },
	{ "x-mac-ukrainian", 10017 },
	{ "x-ms-cp932", 932 },
	{ "x-sjis", 932 },
	{ "x-unicode-2-0-utf-7", 65000 },
	{ "x-unicode-2-0-utf-8", 65001 },
	{ "x-x-big5", 950 },
	{ "x_chinese-eten", 20002 }
};

inline bool operator <(const CodepageCharset& l, const CodepageCharset& r) {
	return l.code_page < r.code_page;
}

const char* codepage_to_charset(unsigned int codepage) {
	CodepageCharset tmp = { codepage, 0 };
	const CodepageCharset* cp = std::lower_bound(codepage_map + 0,
			codepage_map + g_codepage_number, tmp,
			std::less<CodepageCharset>());
	if (cp != codepage_map + g_codepage_number && cp->code_page == codepage)
		return cp->charset;
	else
		return "us-ascii";
}

inline bool operator <(const CharsetCodepage& l, const CharsetCodepage& r) {
	return strcasecmp(l.charset, r.charset) < 0;
}

unsigned int charset_to_codepage(const char* charset) {
	CharsetCodepage tmp = { charset, 0 };
	const CharsetCodepage* cs = std::lower_bound(charset_map + 0,
			charset_map + g_charset_number, tmp, std::less<CharsetCodepage>());
	if (cs != charset_map + g_charset_number
			&& strcasecmp(cs->charset, charset) == 0)
		return cs->code_page;
	else
		return 0;
}

void init_locale() {
	setlocale(LC_ALL, "");
}

void init_locale(const std::string& loc_name) {
	setlocale(LC_ALL, loc_name.c_str());
}

const char* get_locale_charset() {
#ifdef __MSVC__
	const char* loc = setlocale(LC_CTYPE,0);
	if (stricmp(loc, "C") == 0)
		return "us-ascii";
	else {
		std::vector<std::string> part;
		split<char>(loc, ".", part);
		if (part.size() != 2) {
			return "us-ascii";
		} else {
			unsigned int lcid = strto<unsigned int>(part[1]);
			return codepage_to_charset(lcid);
		}
	}
#elif defined(__GNUC__)
	const char* charset = 0;
	g_get_charset(&charset);
	return charset;
#else
#error Not implement.
#endif
}


std::wstring locale_to_wstring(const std::string& src) {
#ifdef __MSVC__
	std::wstring result;
# ifndef USE_CONVERT_API
	size_t len = 0;
	mbstowcs_s(&len, 0, 0, src.c_str(), 0);
	result.resize(len - 1);
	errno_t err = mbstowcs_s(0, result.begin().operator ->(), len, src.c_str(),
			len - 1);
	if (err != 0)
	result.clear();
	return result;
# else
	unsigned int codepage = charset_to_codepage(get_locale_charset());
	int len = MultiByteToWideChar(codepage, 0, src.c_str(), src.length(), 0, 0);
	if (len <= 0) {
		result.clear();
		return result;
	}
	result.resize(len);
	len = MultiByteToWideChar(codepage, 0, src.c_str(), src.length(),
			result.begin().operator ->(), len);
	if (len <= 0) {
		result.clear();
	}
	return result;
# endif
#else
	return utf8_to_wstring(locale_to_utf8(src));
#endif
}

std::string wstring_to_locale(const std::wstring &src) {
#ifdef __MSVC__
	std::string result;
# ifndef USE_CONVERT_API
	size_t len = 0;
	wcstombs_s(&len, 0, 0, src.c_str(), 0);
	result.resize(len - 1);
	errno_t err = wcstombs_s(0, result.begin().operator ->(), len, src.c_str(),
			len - 1);
	if (err != 0)
	result.clear();
	return result;
# else
	unsigned int codepage = charset_to_codepage(get_locale_charset());
	int len = WideCharToMultiByte(codepage, 0, src.c_str(), src.length(),
			0, 0, 0, 0);
	if (len <= 0) {
		result.clear();
		return result;
	}
	result.resize(len);
	len = WideCharToMultiByte(codepage, 0, src.c_str(), src.length(),
			result.begin().operator ->(), len, 0, 0);
	if (len <= 0) {
		result.clear();
	}
	return result;
# endif
#else
	return utf8_to_locale(wstring_to_utf8(src));
#endif
}

std::wstring utf8_to_wstring(const std::string& src) {
	std::wstring result;
#ifdef __MSVC__
	int len = MultiByteToWideChar(CP_UTF8, 0, src.c_str(), src.length(), 0, 0);
	if (len <= 0) {
		result.clear();
		return result;
	}
	result.resize(len);
	len = MultiByteToWideChar(CP_UTF8, 0, src.c_str(), src.length(),
			result.begin().operator ->(), len);
	if (len <= 0) {
		result.clear();
	}
	return result;
#elif defined(__GNUC__)
	glong size = 0;
	gunichar* newstr = g_utf8_to_ucs4_fast(src.c_str(), src.length(), &size);
	if (newstr) {
		result.assign((wchar_t*) newstr, size);
		g_free(newstr);
	}
	return result;
#else
#error "not implement."
#endif
}

std::string wstring_to_utf8(const std::wstring &src) {
	std::string result;
#ifdef __MSVC__
	int len = WideCharToMultiByte(CP_UTF8, 0, src.c_str(), src.length(),
			0, 0, 0, 0);
	if (len <= 0) {
		result.clear();
		return result;
	}
	result.resize(len);
	len = WideCharToMultiByte(CP_UTF8, 0, src.c_str(), src.length(),
			result.begin().operator ->(), len, 0, 0);
	if (len <= 0) {
		result.clear();
	}
	return result;
#elif defined(__GNUC__)
	glong size = 0;
	char* newstr = g_ucs4_to_utf8(
			(const gunichar*) src.c_str(),
			(glong)src.length(),
			(glong*)0,
			&size,
			(GError**)0);
	if (newstr) {
		result.assign(newstr, size);
		g_free(newstr);
	}
	return result;
#else
#error "not implement."
#endif
}

std::string utf8_to_locale(const std::string& src) {
#ifdef __MSVC__
	const char* charset = get_locale_charset();
	if (strcasecmp(charset, "utf8") == 0 ||
			strcasecmp(charset, "utf-8") == 0) {
		return src;
	}
	return wstring_to_locale(utf8_to_wstring(src));
#elif defined(__GNUC__)
	std::string result;
	gsize size = 0;
	char* newstr = g_locale_from_utf8((const gchar*) src.c_str(), src.length(),
			0, &size, 0);
	if (newstr) {
		result.assign(newstr, size);
		g_free(newstr);
	}
	return result;
#else
#error Not implement.
#endif
}

std::string locale_to_utf8(const std::string& src) {
#ifdef __MSVC__
	const char* charset = get_locale_charset();
	if (strcasecmp(charset, "utf8") == 0 ||
			strcasecmp(charset, "utf-8") == 0) {
		return src;
	}
	return wstring_to_utf8(locale_to_wstring(src));
#elif defined(__GNUC__)
	std::string result;
	gsize size = 0;
	char* newstr = g_locale_to_utf8(src.c_str(), src.length(), 0, &size, 0);
	if (newstr) {
		result.assign(newstr, size);
		g_free(newstr);
	}
	return result;
#else
#error Not implement.
#endif
}

std::wstring charset_to_wstring(const std::string& src,
		const std::string& charset) {
	std::wstring result;
#ifdef __MSVC__
	unsigned int codepage = tlib::convert_charset_to_codepage(charset.c_str());
	if (codepage == 1200) {
		result.assign((wchar_t*)src.c_str(), src.length() / sizeof(wchar_t));
		return result;
	} else if (codepage == 1201) {
		const char* scan = src.c_str();
		const char* scan_end = scan + src.length();
		result.reserve((scan_end - scan) / 2);
		while (scan < scan_end - 1) {
			wchar_t ch = *(scan + 1);
			ch <<= 8;
			ch |= *scan;
			result.push_back(ch);
			scan += 2;
		}
		return result;
	}

	int len = MultiByteToWideChar(codepage, 0, src.c_str(), src.size(), 0, 0);
	if (len <= 0) {
		result.clear();
		return result;
	}
	result.resize(len);
	len = MultiByteToWideChar(codepage, 0, src.c_str(), src.size(),
			result.begin().operator ->(), len);
	if (len <= 0) {
		result.clear();
	}
	return result;
#elif defined(__GNUC__)
	gsize size = 0;
	gchar* newstr = g_convert(src.c_str(), src.size(), "WCHAR_T",
			charset.c_str(), 0, &size, 0);
	if (newstr) {
		result.assign((wchar_t*) newstr, size / sizeof(wchar_t));
		g_free(newstr);
	}
	return result;
#else
#error Not implement.
#endif
}

std::string wstring_to_charset(const std::wstring& src,
		const std::string& charset) {
	std::string result;
#ifdef __MSVC__
	unsigned int codepage = tlib::convert_charset_to_codepage(charset.c_str());
	if (codepage == 1200) {
		result.assign((char*)src.c_str(), src.length() * sizeof(wchar_t));
		return result;
	} else if (codepage == 1201) {
		result.reserve(src.length() * 2);
		const char* scan = (const char*)src.c_str();
		const char* scan_end = scan + src.length() * 2;
		while (scan < scan_end - 1) {
			result.push_back(*(scan + 1));
			result.push_back(*scan);
			scan += 2;
		}
		return result;
	}
	int len = WideCharToMultiByte(codepage, 0, src.c_str(), src.size(), 0, 0, 0, 0);
	if (len <= 0) {
		result.clear();
		return result;
	}
	result.resize(len);
	len = WideCharToMultiByte(codepage, 0, src.c_str(), src.size(),
			result.begin().operator ->(), len, 0, 0);
	if (len <= 0) {
		result.clear();
	}
	return result;
#elif defined(__GNUC__)
	gsize size = 0;
	gchar* newstr = g_convert((gchar*) src.c_str(),
			src.size() * sizeof(wchar_t), charset.c_str(), "WCHAR_T", 0, &size,
			0);
	if (newstr) {
		result.assign(newstr, size);
		g_free(newstr);
	}
	return result;
#else
#error Not implement.
#endif
}


const char* test_charset(const binary& data, bool& bom)
{
	const unsigned char* scan = begin_ptr(data);
	if (data.size() >= 3)
	{
		if (scan[0] == 0xEF && scan[1] == 0xBB && scan[2] == 0xBF)
		{
			bom = true;
			return "utf-8";
		}
		else if (scan[0] == 0xFF && scan[1] == 0xFE)
		{
			bom = true;
			return "utf-16";
		}
		else if (scan[0] == 0xFE && scan[1] == 0xFF)
		{
			bom = true;
			return "utf-16be";
		}
	}
	else if (data.size() >= 2)
	{
		if (scan[0] == 0xFF && scan[1] == 0xFE)
		{
			bom = true;
			return "utf-16";
		}
		else if (scan[0] == 0xFE && scan[1] == 0xFF)
		{
			bom = true;
			return "utf-16be";
		}
	}
	bom = false;
	// Has no BOM.
	// so, determine whether it is en UTF-8 charset.
	size_t len = data.size();
	bool utf8 = true;
	bool pure_ansi = true;
	for (size_t i = 0; i < len;)
	{
		if (scan[i] < 128)
		{
			i++;
		}
		else if (i < len - 1 &&
			(scan[i] & 0xE0) == 0xC0 &&
			(scan[i + 1] & 0xC0) == 0x80)
		{
			i += 2;
			pure_ansi = false;
		}
		else if (i < len - 2 &&
			(scan[i] & 0xF0) == 0xE0 &&
			(scan[i + 1] & 0xC0) == 0x80 &&
			(scan[i + 2] & 0xC0) == 0x80)
		{
			i += 3;
			pure_ansi = false;
		}
		else
		{
			utf8 = false;
			pure_ansi = false;
			break;
		}
	}
	if (utf8)
		return "utf-8";
	else if (pure_ansi)
		return "us-ascii";
	else
		return get_locale_charset();
}

} // End of namesapce tlib
