#include "minicsv.h"
#include <iostream>

using namespace mini;

#define MYASSERT(func, value, expected) if(value != expected) { std::cerr << func << "() received:" << value << " and expected:" << expected << " are different" << std::endl; return false;}

bool test(const std::string& name, int qty, bool enable_quote);
bool test_char(const std::string& name, char ch, bool enable_quote);
bool test_nchar(const std::string& name, char ch, bool enable_quote);
bool test_file(const std::string& file, const std::string& name, int qty, bool enable_quote);
bool test_input(const std::string& str, const std::string& expected_str, int expected_qty, bool enable_quote);
bool test_get_rest_of_line(const std::string& name);
bool test_file_get_rest_of_line(const std::string& name);
bool test_write_double_quotes(const std::string& name, int qty, bool enable_quote);
bool test_file_write_double_quotes(const std::string& name, int qty, bool enable_quote);

int main()
{
	test("Fruits", 100, true);
	test("", 200, true);
	test("Fruits, Vegetable", 300, true);
	test("Fruits", 400, false);
	test("", 500, false);

	test_char("Fruits", 'A', true);
	test_char("", 'B', true);
	test_char("Fruits, Vegetable", 'G', true);
	test_char("Fruits", 'H', false);
	test_char("", 'J', false);

	test_nchar("Fruits", 65, true);
	test_nchar("", 66, true);
	test_nchar("Fruits, Vegetable", 67, true);
	test_nchar("Fruits", 68, false);
	test_nchar("", 69, false);

	test_file("test_file1.txt", "Fruits", 100, true);
	test_file("test_file2.txt", "", 200, true);
	test_file("test_file3.txt", "Fruits, Vegetable", 300, true);
	test_file("test_file4.txt", "Fruits", 400, false);
	test_file("test_file5.txt", "", 500, false);

	test_input("\"He said: \"\"the more, the merrier\"\"\",66", "He said: \"the more, the merrier\"", 66, true);
	test_write_double_quotes("He said: \"the more, the merrier\"", 66, true);
	test_file_write_double_quotes("He said: \"the more, the merrier\"", 66, true);

	test_get_rest_of_line("Fruits");
	test_file_get_rest_of_line("Fruits");

	std::cout << "All test done!" << std::endl;
}

bool test_input(const std::string& str, const std::string& expected_str, int expected_qty, bool enable_quote)
{
	csv::istringstream is(str.c_str());
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

			MYASSERT(__FUNCTION__, dest_name, expected_str);
			MYASSERT(__FUNCTION__, dest_qty, expected_qty);
			++cnt;
		}
		catch (std::runtime_error& e)
		{
			std::cerr << __FUNCTION__ << e.what() << std::endl;
		}
	}
	MYASSERT(__FUNCTION__, cnt, 1);
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

			MYASSERT(__FUNCTION__, dest_name, name);
			MYASSERT(__FUNCTION__, dest_qty, qty + cnt);

			++cnt;
		}
		catch (std::runtime_error& e)
		{
			std::cerr << __FUNCTION__ << e.what() << std::endl;
		}
	}
	MYASSERT(__FUNCTION__, cnt, 2);
	return true;
}

bool test_char(const std::string& name, char ch, bool enable_quote)
{
	csv::ostringstream os;
	os.set_delimiter(',', "$$");
	os.enable_surround_quote_on_str(enable_quote, '\"');

	os << name << ch << NEWLINE;

	csv::istringstream is(os.get_text().c_str());
	is.set_delimiter(',', "$$");
	is.enable_trim_quote_on_str(enable_quote, '\"');

	std::string dest_name = "";
	char dest_char = 0;

	int cnt = 0;
	while (is.read_line())
	{
		try
		{
			is >> dest_name >> dest_char;

			MYASSERT(__FUNCTION__, dest_name, name);
			MYASSERT(__FUNCTION__, dest_char, ch);

			++cnt;
		}
		catch (std::runtime_error& e)
		{
			std::cerr << __FUNCTION__ << e.what() << std::endl;
		}
	}
	MYASSERT(__FUNCTION__, cnt, 1);
	return true;
}

bool test_nchar(const std::string& name, char ch, bool enable_quote)
{
	csv::ostringstream os;
	os.set_delimiter(',', "$$");
	os.enable_surround_quote_on_str(enable_quote, '\"');

	os << name << csv::NChar(ch) << NEWLINE;

	csv::istringstream is(os.get_text().c_str());
	is.set_delimiter(',', "$$");
	is.enable_trim_quote_on_str(enable_quote, '\"');

	std::string dest_name = "";
	char dest_char = 0;

	int cnt = 0;
	while (is.read_line())
	{
		try
		{
			is >> dest_name >> csv::NChar(dest_char);

			MYASSERT(__FUNCTION__, dest_name, name);
			MYASSERT(__FUNCTION__, dest_char, ch);

			++cnt;
		}
		catch (std::runtime_error& e)
		{
			std::cerr << __FUNCTION__ << e.what() << std::endl;
		}
	}
	MYASSERT(__FUNCTION__, cnt, 1);
	return true;
}

bool test_file(const std::string& file, const std::string& name, int qty, bool enable_quote)
{
	csv::ofstream os(file.c_str());
	os.set_delimiter(',', "$$");
	os.enable_surround_quote_on_str(enable_quote, '\"');

	os << name << qty << NEWLINE;
	os << name << qty + 1 << NEWLINE;

	os.flush();
	os.close();

	csv::ifstream is(file.c_str());
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

			MYASSERT(__FUNCTION__, dest_name, name);
			MYASSERT(__FUNCTION__, dest_qty, qty + cnt);

			++cnt;
		}
		catch (std::runtime_error& e)
		{
			std::cerr << __FUNCTION__ << e.what() << std::endl;
		}
	}
	MYASSERT(__FUNCTION__, cnt, 2);
	return true;
}

bool test_get_rest_of_line(const std::string& name)
{
	csv::ostringstream os;
	os.set_delimiter(',', "$$");
	os.enable_surround_quote_on_str(false, '\"');

	os << name << NEWLINE;
	os << name + "1" << NEWLINE;

	csv::istringstream is(os.get_text().c_str());
	is.set_delimiter(',', "$$");
	is.enable_trim_quote_on_str(false, '\"');

	std::string dest_name = "";

	int cnt = 0;
	try
	{
		if (is.read_line())
		{
			dest_name = is.get_rest_of_line();

			MYASSERT(__FUNCTION__, dest_name, name);

			++cnt;
		}
		if (is.read_line())
		{
			dest_name = is.get_rest_of_line();

			MYASSERT(__FUNCTION__, dest_name, name + "1");

			++cnt;
		}
	}
	catch (std::runtime_error& e)
	{
		std::cerr << __FUNCTION__ << e.what() << std::endl;
	}
	MYASSERT(__FUNCTION__, cnt, 2);
	return true;
}

bool test_file_get_rest_of_line(const std::string& name)
{
	csv::ofstream os("test_file_get_rest_of_line.txt");

	os.set_delimiter(',', "$$");
	os.enable_surround_quote_on_str(false, '\"');

	os << name << NEWLINE;
	os << name + "1" << NEWLINE;

	os.flush();
	os.close();

	csv::ifstream is("test_file_get_rest_of_line.txt");

	is.set_delimiter(',', "$$");
	is.enable_trim_quote_on_str(false, '\"');

	std::string dest_name = "";

	int cnt = 0;
	try
	{
		if (is.read_line())
		{
			dest_name = is.get_rest_of_line();

			MYASSERT(__FUNCTION__, dest_name, name);

			++cnt;
		}
		if (is.read_line())
		{
			dest_name = is.get_rest_of_line();

			MYASSERT(__FUNCTION__, dest_name, name + "1");

			++cnt;
		}
	}
	catch (std::runtime_error& e)
	{
		std::cerr << __FUNCTION__ << e.what() << std::endl;
	}
	MYASSERT(__FUNCTION__, cnt, 2);

	return true;
}

bool test_write_double_quotes(const std::string& name, int qty, bool enable_quote)
{
	csv::ostringstream os;
	os.set_delimiter(',', "$$");
	os.enable_surround_quote_on_str(enable_quote, '\"', "\"\"");

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

			MYASSERT(__FUNCTION__, dest_name, name);
			MYASSERT(__FUNCTION__, dest_qty, qty + cnt);

			++cnt;
		}
		catch (std::runtime_error& e)
		{
			std::cerr << __FUNCTION__ << e.what() << std::endl;
		}
	}
	MYASSERT(__FUNCTION__, cnt, 2);
	return true;
}

bool test_file_write_double_quotes(const std::string& name, int qty, bool enable_quote)
{
	csv::ofstream os("test_file_write_double_quotes.txt");
	os.set_delimiter(',', "$$");
	os.enable_surround_quote_on_str(enable_quote, '\"', "\"\"");

	os << name << qty << NEWLINE;
	os << name << qty + 1 << NEWLINE;

	os.flush();
	os.close();

	csv::ifstream is("test_file_write_double_quotes.txt");
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

			MYASSERT(__FUNCTION__, dest_name, name);
			MYASSERT(__FUNCTION__, dest_qty, qty + cnt);

			++cnt;
		}
		catch (std::runtime_error& e)
		{
			std::cerr << __FUNCTION__ << e.what() << std::endl;
		}
	}
	MYASSERT(__FUNCTION__, cnt, 2);
	return true;
}

