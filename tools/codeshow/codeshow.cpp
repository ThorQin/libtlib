/************************************************************************
*
*  CodeShow - a simple source code rendering tools.
*  Copyright (C) 2010  Thor Qin
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*  Author: Thor Qin
*  Bug Report: thor.qin@gmail.com
*
**************************************************************************/

#include "../../tlib.h"
#include <iostream>

using namespace tlib;
using namespace std;

void show_help()
{
	cout << "Usage: codeshow [option] ... <input file>\n"
		<< "Options:\n\t-d, --define: -d <filename>\n\t\tSpecified the lexical define filename, "
		<< "\n\t\tdefault is 'in.lex' in current directory.\n"
		<< "\t-t, --template: -t <filename>\n\t\tSpecified the template filename, "
		<< "\n\t\tdefalt is 'in.template' in current directory.\n"
		<< "\t-l, --line-number: \n\t\tWhether to display line numbers."
		<< "\n\t\tdefalt is not display.\n"
		<< "\t-c, --gen-cpp: \n\t\tGenerate CPP define and template file in current directory.\n"
		<< "\t-h, --help: \n\t\tShow this help message.\n"
		<< endl;
	exit(0);
}


void show_error(const char* option)
{
	if (option)
		cerr << "Unknow option: " << option << "\n";
	else
		cerr << "Invalid parameters.\n";
	cerr << "Use \"codeshow --help\""
		<< " to show more informations.\n"
		<< endl;
	exit(-1);
}


int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "");
	const char* input_file = 0;
	const char* define_file = 0;
	const char* template_file = 0;
	bool line_number = false;
	bool cpp = false;

	int i = 1;
	for (; i < argc; ++i)
	{
		if (*(argv[i]) != '-')
		{
			if (i != argc - 1)
			{
				show_error(0);
			}
			input_file = argv[i];
		}
		else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
		{
			show_help();
		}
		else if (strcmp(argv[i], "--define") == 0 || strcmp(argv[i], "-d") == 0)
		{
			if (i >= argc - 1 || *(argv[i + 1]) == '-')
			{
				show_error(argv[i]);
			}
			define_file = argv[i + 1];
			++i;
		}
		else if (strcmp(argv[i], "--template") == 0 || strcmp(argv[i], "-t") == 0)
		{
			if (i >= argc - 1 || *(argv[i + 1]) == '-')
			{
				show_error(argv[i]);
			}
			template_file = argv[i + 1];
			++i;
		}
		else if (strcmp(argv[i], "--gen-cpp") == 0 || strcmp(argv[i], "-c") == 0)
		{
			// then show line number
			cpp = true;
		}
		else if (strcmp(argv[i], "--line-number") == 0 || strcmp(argv[i], "-l") == 0)
		{
			// then show line number
			line_number = true;
		}
		else
		{
			show_error(argv[i]);
		}
	}
	if (input_file == 0)
	{
		show_error(0);
	}
	if (define_file == 0)
	{
		define_file = "in.lex";
	}
	if (template_file == 0)
	{
		template_file = "in.template";
	}

	if (cpp)
	{
		// Generate CPP define and template file in current working directory to use.
		ofstream lex_file(define_file, ios::out|ios::binary);
		if (!lex_file.good())
		{
			cerr << "Create lexical define file failed"
				<< ".\n" << endl;
			exit(-1);
		}
		try
		{
			extern const unsigned char res_lex[];
			extern const unsigned int res_lex_length;

			lex_file.write((const char*)res_lex, res_lex_length);
		}
		catch (const std::exception& e)
		{
			cerr << "Create lexical define file failed";
			if (e.what())
				cerr << ": " << e.what() << "\n" << endl;
			else
				cerr << ".\n" << endl;
			exit(-1);
		}
		lex_file.close();

		ofstream tmp_file(template_file, ios::out|ios::binary);
		if (!tmp_file.good())
		{
			cerr << "Create template file failed"
				<< ".\n" << endl;
			exit(-1);
		}
		try
		{
			extern const unsigned char res_template[];
			extern const unsigned int res_template_length;

			tmp_file.write((const char*)res_template, res_template_length);
		}
		catch (const std::exception& e)
		{
			cerr << "Create template file failed";
			if (e.what())
				cerr << ": " << e.what() << "\n" << endl;
			else
				cerr << ".\n" << endl;
			exit(-1);
		}
		tmp_file.close();
	}


	// Using lexical parser to extract token.
	lex::Lexical<char>::LexicalPtr lex;
	try
	{
		lex = lex::Lexical<char>::create_by_file(define_file);
	}
	catch (const std::exception& e)
	{
		cerr << "Create lexical parser failed";
		if (e.what())
			cerr << ": " << e.what() << "\n" << endl;
		else
			cerr << ".\n" << endl;
		exit(-1);
	}

	try
	{
		string temp = tlib::load_text_file_to_string(template_file);
		string define = tlib::load_text_file_to_string(input_file);
		istringstream instr_stream(define);
		lex->parse(instr_stream);

		string code;
		if (line_number)
			code = "<span class=\"line_number\">1.</span> ";
		int line = 2;
		lex::Token<char> token;
		while (lex->fetch_next(token))
		{
			if (token.action)
			{
				string result(token.str);
				tlib::replace<char>(result, "\r", "");
				tlib::replace<char>(result, "<", "&lt;");
				tlib::replace<char>(result, ">", "&gt;");
				tlib::replace<char>(result, " ", "&nbsp;");
				tlib::replace<char>(result, "\t", "&nbsp;&nbsp;&nbsp;&nbsp;");
				if (line_number)
				{
					while (result.find('\n') != result.npos)
					{
						string line_temp = "<br><span class=\"line_number\">";
						line_temp += strfrom<int>(line);
						line_temp += ".</span> ";
						tlib::replace<char>(result, "\n", line_temp, 1);
						line++;
					}
				}
				else
					tlib::replace<char>(result, "\n", "<br>");
				code += "<span class=\"";
				code += token.name;
				code += "\">";
				code += result;
				code += "</span>";
			}
			else
			{
				string result(token.str);
				tlib::replace<char>(result, "\r", "");
				tlib::replace<char>(result, "<", "&lt;");
				tlib::replace<char>(result, ">", "&gt;");
				tlib::replace<char>(result, " ", "&nbsp;");
				tlib::replace<char>(result, "\t", "&nbsp;&nbsp;&nbsp;&nbsp;");
				if (line_number)
				{
					while (result.find('\n') != result.npos)
					{
						string line_temp = "<br><span class=\"line_number\">";
						line_temp += strfrom<int>(line);
						line_temp += ".</span> ";
						tlib::replace<char>(result, "\n", line_temp, 1);
						line++;
					}
				}
				else
					tlib::replace<char>(result, "\n", "<br>");

				code += result;
			}
		}
		tlib::replace<char>(temp, "<!--%CODE%-->", code);
		cout << temp;
	}
	catch (const std::exception& e)
	{
		cerr << "Parse failed";
		if (e.what())
			cerr << ": " << e.what() << "\n" << endl;
		else
			cerr << ".\n" << endl;
		exit(-1);
	}
	return 0;
}

