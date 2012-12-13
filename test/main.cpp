/*
 * test_main.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: thor
 */


#include "../include/tlib/tlib.h"
#include <iostream>

using namespace std;
using namespace tlib;

void test_string_charset() {
	// string to wstring
	wstring ws = str<wchar_t>("this is a test 搜索 C++0x.");
	string s = str<char>(ws);
	string gbk = wstring_to_charset(ws, "gb18030");
	cout << convert_charset(gbk, "gb18030", "utf-8") << endl;
}

void test_string_split() {
	string s = "1,2,3,abc,搜索,def,4,5,6";
	auto ss = split(s, ",");

	for (string &x : ss) {
		cout << x << endl;
	}
	cout << endl;
	for_each(ss.rbegin(), ss.rend(), [](string& x){
		cout << x << endl;
	});
}

void test_string_join() {
	string s = "1,2,3,abc,搜索,def,4,5,6";
	auto ss = split(str<wchar_t>(s), L",");
	wstring ws = join(ss, L",");
	cout << str<char>(ws) << endl;

	ws = join(ss.rbegin(), ss.rend(), L",");
	cout << str<char>(ws) << endl;
}

void test_string_case_convert() {
	string s = "abcdefg,搜索";
	string big = to_upper(s);
	cout << big << endl;
	cout << to_lower(big) << endl;
}

void test_string_trim() {
	string s = "   \nabc\t\r   ";
	trim(s);
	cout << ">" << s << "<" << endl;
	s = trim_copy<char>(" def ");
	cout << ">" << s << "<" << endl;
}

void test_string_replace() {
	string s = "ab ba ab dc";
	replace(s, "ab", "xx");
	cout << ">" << s << "<" << endl;
	s = replace_copy(s, "xx", "@@@@");
	cout << ">" << s << "<" << endl;
}

void test_string_convert() {
	string s = strfrom<int>(123);
	long l = strto<long>(s);
	cout << "data type convert: " << l << endl;
}

void test_os_functionS() {
	cout << get_host_name() << endl;
	cout << get_current_path() << endl;
	cout << get_exec_filename() << endl;
	cout << get_home_path() << endl;
}

void test_time() {
	Time t;
	cout << "Time: " << t.to_string(TimeFormat::format_mail) << endl;
	t = parse_time("2020-12-22", TimeFormat::format_date);
	cout << "Time parsed: " << t.to_string(TimeFormat::format_standard) << endl;
	t = parse_time("1:2:3", TimeFormat::format_time);
	cout << "Time parsed: " << t.to_string(TimeFormat::format_standard) << endl;

	TimeSpan a(2*60*60, SpanUnit::hours);
	cout << "span: " << a.get_hours() << "(hours)" << endl;
	t -= a;
	cout << "Time: " << t.to_string(TimeFormat::format_standard) << endl;
}

void test_binary() {
	binary b = strto<binary>("test");
	cout << "out put binary" << endl;
	cout << b << endl;
	b = wstrto<binary>(L"WIDE");
	cout << hex_str(b) << endl;
	b = strto<binary>("TEXT");
	cout << b.size() << ":" << b << endl;
	wchar_t buff[10];
	wcscpy(buff, L"123456789");
	b << buff << "some text" << "WIDE" << 123;

	cout << hex_str(b) << endl;

	int i = 0;
	extract_back(b, i);
	cout << i << endl;
	string s;
	s.resize(4);
	extract_back(b, s);
	cout << s << endl;
	s.resize(9);
	extract_back(b, s);
	cout << s << endl;
	wchar_t ws[10];
	extract_back(b, ws);
	cout << str<char>(ws) << endl;
	s.resize(4);
	extract_back(b, s);
	cout << s << endl;
}

void test_encode() {
	binary b = to_binary("1234567890搜索C++0x.");
	string s = base64::encode(b);
	cout << s << endl;
	cout << base64::decode(s) << endl;

	s = qprint::encode(b);
	cout << s << endl;
	cout << qprint::decode(s) << endl;

	cout << "MD5 hash:" << hex_str(tlib::hash::md5(b)) << endl;

}

int main(int argc, char* argv[]) {
	init_locale();

	test_string_charset();
	test_string_split();
	test_string_join();
	test_string_case_convert();
	test_string_trim();
	test_string_replace();
	test_string_convert();
	test_time();
	test_os_functionS();
	test_binary();
	test_encode();

	return 0;
}

