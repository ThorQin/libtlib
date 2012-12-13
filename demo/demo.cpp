#include "../tlib.h"
#include <list>


using namespace tlib;
using namespace std;

void test_string_to_number()
{
	short a = wstrto<short>(L"12345");
	unsigned int b = strto<unsigned int>("-4x46");
	long c = strto<long>("9888888888888888881234567890");
	unsigned long d = strto<unsigned long>(" -1");
	printf("%d, %u, %ld, %lu\n", a, b, c, d);
}

void test_number_to_string()
{
	string a = strfrom<int>(1234);
	string b = strfrom<long>(-654321);
	string c = strfrom<unsigned long>(-3);
	cout << a << ", " << b << ", " << c << endl;
}

void test_lex()
{
	try
	{
		lex::Lexical<char>::LexicalPtr lexical =
				lex::Lexical<char>::create_by_file("demo.lex");
		ifstream infile("demo.cpp", ios::in);
		filebuf* buf = infile.rdbuf();
		char buffer[4096];
		buf->pubsetbuf(buffer, 4096);
		// lexical->best_match = false; // default is FALSE
		lexical->parse(infile);
		lex::Token<char> token;
		while (lexical->fetch_next(token))
		{
			cout << token.name << ": " << token.str << endl;
		}
		infile.close();
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
	}
}

void print(int level, const std::wstring& msg)
{
	for (int i = 0; i < level; i++)
	{
		wcout << L"    ";
	}
	wcout << msg << endl;
}

using namespace xml::xpath;

const wchar_t* predicate_type(Predicate::ExpType t)
{
	switch (t)
	{
	case Predicate::E_INTEGER:
		return L"integer";
	case Predicate::E_FLOAT:
		return L"float";
	case Predicate::E_STRING:
		return L"string";
	case Predicate::E_PATH:
		return L"path";
	case Predicate::E_AND:
		return L"&&";
	case Predicate::E_OR:
		return L"||";
	case Predicate::E_LESS:
		return L"<";
	case Predicate::E_LESS_EQUAL:
		return L"<=";
	case Predicate::E_MORE:
		return L">";
	case Predicate::E_MORE_EQUAL:
		return L">=";
	case Predicate::E_EQUAL:
		return L"=";
	case Predicate::E_NOT_EQUAL:
		return L"!=";
	default:
		return L"error";
	}
}

void print_xpath(const XPath& path, int level = 0);

void print_predicate(PredicatePtr predicate, int level)
{
	print(level++, L"[");
	print(level, predicate_type(predicate->type) + wstring(L":"));
	if (predicate->type == Predicate::E_INTEGER)
	{
		PredicateIntegerValuePtr p =
				PredicateIntegerValuePtr::cast_dynamic(predicate);
		print(level, tlib::wstrfrom<int>(p->value));
	}
	else if (predicate->type == Predicate::E_FLOAT)
	{
		PredicateFloatValuePtr p =
				PredicateFloatValuePtr::cast_dynamic(predicate);
		print(level, tlib::wstrfrom<float>(p->value));
	}
	else if (predicate->type == Predicate::E_STRING)
	{
		PredicateStringValuePtr p =
				PredicateStringValuePtr::cast_dynamic(predicate);
		print(level, p->value);
	}
	else if (predicate->type == Predicate::E_PATH)
	{
		PredicatePathValuePtr p =
				PredicatePathValuePtr::cast_dynamic(predicate);
		print_xpath(p->value, level);
	}
	else if (predicate->type >= Predicate::E_AND
			&& predicate->type <= Predicate::E_NOT_EQUAL)
	{
		PredicateExpPtr p =
						PredicateExpPtr::cast_dynamic(predicate);
		print_predicate(p->left, level);
		print_predicate(p->right, level);
	}
	else
		print(level, L"ERROR!!!");
	print(--level, L"]");
}

const wchar_t* axis_type(Step::AxisType t)
{
	switch (t)
	{
	case Step::AXIS_CURRENT:
		return L"current";
	case Step::AXIS_DIRECT:
		return L"direct";
	case Step::AXIS_INDIRECT:
		return L"indirect";
	default:
		return L"error";
	}
}

void print_xpath(const XPath& path, int level)
{
	for (Paths::const_iterator it_path = path.paths.begin();
			it_path != path.paths.end(); it_path++)
	{
		print(level++, L"{");
		for (Path::const_iterator it_step = it_path->begin();
				it_step != it_path->end() ; it_step++)
		{
			print(level, wstring(L"prefix:") + axis_type(it_step->axis));
			print(level, L"step:" + it_step->name);
			if (it_step->predicate)
				print_predicate(it_step->predicate, level);
		}
		print(--level, L"}");
	}
}

using namespace std;
int main(int argc, char **argv)
{
	wstring cmd = L"/test/abc[(@dd='sss'||(@dd =../@aa||@dd>3))&&abc!=\"Test\\'\"]/@a";
	tlib::init_loc();

	XPath xpath(cmd);

	print_xpath(xpath);

//	test_string_to_number();
//	test_number_to_string();
//	test_lex();
	return 0;
}
