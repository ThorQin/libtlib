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


/* This file is a tool and only for initialize build xmlsax lexical parser. */

#include "../../tlib.h"
#include <locale.h>
#include <cstdio>
#ifdef __MSVC__
#define PATH_MAX 256
#pragma warning(disable : 4996)
#elif defined(__GNUC__)
#include <dirent.h>
#endif

using namespace std;
using namespace tlib;


void show_error()
{
	fprintf(stderr, "Invalid parameters. \n"
			"Please try \"genlex --help\""
			" to get more informations.\n\n");
}

void show_usage()
{
	printf("Usage: lexgen [options] <input file>\n" \
			"  Make binary lexical file by input lexical text file.\n");
	printf("Options:\n" \
			"  -o, --out <NAME>   Output file, default output is \"<input name>.bc\"\n"
			"                     in current working directory.\n"
			"  -w, --wchar        Use wchar_t as internal character type.\n"
			"  -u, --unsigned     Use unsigned char as internal character type.\n"
			"  -h, --help         Show this usage information and leave.\n\n");
}


int main (int argc, char *argv[])
{
	setlocale(LC_ALL, "");
	char *input = NULL;
	char *output = NULL;
	int type = 0;
	for (int i = 1; i < argc; ++i)
	{
		if (*(argv[i]) != '-')
		{
			if (i != argc - 1)
			{
				show_error();
				return -1;
			}
			input = argv[i];
		}
		else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
		{
			show_usage();
			return 0;
		}
		else if (strcmp(argv[i], "--wchar") == 0 || strcmp(argv[i], "-w") == 0)
		{
			type = 2;
		}
		else if (strcmp(argv[i], "--unsigned") == 0 || strcmp(argv[i], "-u") == 0)
		{
			type = 1;
		}
		else if (strcmp(argv[i], "--out") == 0 || strcmp(argv[i], "-o") == 0)
		{
			if (i >= argc - 2 || *(argv[i + 1]) == '-')
			{
				show_error();
				return -1;
			}
			output = argv[i + 1];
			++i;
		}
	}
	if (input == NULL)
	{
		show_error();
		return -1;
	}
	if (strlen(input) > PATH_MAX - 5)
	{
		fprintf(stderr, "Generate failed: pathname too lang.\n");
		return -1;
	}
	string out;
	if (output == NULL)
	{
		out = input;
		if (out.rfind('.') != out.npos)
			out = out.substr(0, out.rfind('.'));
		out += ".bc";
	}
	else
		out = output;


	try
	{
		cout << "Generate binary lexical file ..." << endl;
		if (type == 2)
		{
			lex::Lexical<wchar_t>::LexicalPtr lex =
					lex::Lexical<wchar_t>::create_by_file(input);
			lex->save_bc(out);
		}
		else if (type == 1)
		{
			lex::Lexical<unsigned char>::LexicalPtr lex =
					lex::Lexical<unsigned char>::create_by_file(input);
			lex->save_bc(out);
		}
		else
		{
			lex::Lexical<char>::LexicalPtr lex =
					lex::Lexical<char>::create_by_file(input);
			lex->save_bc(out);
		}
		cout << "Succeeded!!" << endl;
	}
	catch (const std::exception& e)
	{
		cerr << "Failed: " << e.what() << endl;
		return -1;
	}
	return 0;
}
