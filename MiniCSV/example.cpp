#include "minicsv.h"
#include <iostream>

using namespace mini;

#define MYASSERT(value, expected) if(value != expected) { std::cerr << value << " and expected:" << expected << " are different" << std::endl; return false;}

bool test(const std::string& name, int qty, bool enable_quote);
bool test_input(const std::string& str, const std::string& expected_str, int expected_qty, bool enable_quote);

int main()
{
	test("Fruits", 100, true);
	test("", 200, true);
	test("Fruits, Vegetable", 300, true);
	test("Fruits", 400, false);
	test("", 500, false);

	test_input("\"He said: \"\"the more, the merrier\"\"\",66", "He said: \"the more, the merrier\"", 66, true);
}

bool test_input(const std::string& str, const std::string& expected_str, int expected_qty, bool enable_quote)
{
	//csv::istringstream is("\"He said: \"\"the more, the merrier\"\"\":22,44,66");
	csv::istringstream is(str.c_str());
	is.set_delimiter(',', "$$");
	is.enable_trim_quote_on_str(enable_quote, '\"');

	std::string dest_name = "";
	int dest_qty = 0;

	while (is.read_line())
	{
		try
		{
			is >> dest_name >> dest_qty;

			MYASSERT(dest_name, expected_str);
			MYASSERT(dest_qty, expected_qty);
		}
		catch (std::runtime_error& e)
		{
			std::cerr << e.what() << std::endl;
		}
	}
}

bool test(const std::string& name, int qty, bool enable_quote)
{
	csv::ostringstream os;
	os.set_delimiter(',', "$$");
	os.enable_surround_quote_on_str(enable_quote, '\"');

	os << name << qty << NEWLINE;
	os << name << qty + 1 << NEWLINE;

	csv::istringstream is(os.get_text().c_str());
	is.set_delimiter(',', "$$");
	is.enable_trim_quote_on_str(enable_quote, '\"');

	std::string dest_name = "";
	int dest_qty = 0;

	int cnt = 0;
	while (is.read_line())
	{
		try
		{
			is >> dest_name >> dest_qty;

			MYASSERT(dest_name, name);
			MYASSERT(dest_qty, qty + cnt);

			++cnt;
		}
		catch (std::runtime_error& e)
		{
			std::cerr << e.what() << std::endl;
		}
	}
	return true;
}
