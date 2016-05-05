#include "minicsv.h"
#include <iostream>

struct Product
{
	Product() : name(""), qty(0), price(0.0f) {}
	Product(std::string name_, int qty_, float price_) : name(name_), qty(qty_), price(price_) {}
	std::string name;
	int qty;
	float price;
};

template<>
inline csv::istringstream& operator >> (csv::istringstream& istm, Product& val)
{
	return istm >> val.name >> val.qty >> val.price;
}

template<>
inline csv::ostringstream& operator << (csv::ostringstream& ostm, const Product& val)
{
	return ostm << val.name << val.qty << val.price;
}


int main()
{
	// test file streams
	{
		csv::ofstream os("products.txt");
		os.set_delimiter(',', "$$");
		os.enable_surround_quote_on_str(true, '\"');
		if (os.is_open())
		{
			Product product("Shampoo", 200, 15.0f);
			os << product.name << product.qty << product.price << NEWLINE;
			Product product2("Towel, Soap, \"Shower Foam\"", 300, 6.0f);
			os << product2.name << product2.qty << product2.price << NEWLINE;
		}
		os.flush();
		os.close();

		csv::ifstream is("products.txt");
		is.set_delimiter(',', "$$");
		is.enable_trim_quote_on_str(true, '\"');

		if (is.is_open())
		{
			Product product;
			while (is.read_line())
			{
				is >> product.name >> product.qty >> product.price;
				// display the read items
				std::cout << product.name << "," << product.qty << "," << product.price << std::endl;
			}
		}
	}
	
	// test string streams using overloaded stream operators for Product
	{
		csv::ostringstream os;
		os.set_delimiter(',', "$$");
		Product product("Shampoo", 200, 15.0f);
		os << product << NEWLINE;
		Product product2("Towel, Soap, Shower Foam", 300, 6.0f);
		os << product2 << NEWLINE;

		csv::istringstream is(os.get_text().c_str());
		is.set_delimiter(',', "$$");
		Product prod;
		while (is.read_line())
		{
			is >> prod;
			// display the read items
			std::cout << prod.name << "|" << prod.qty << "|" << prod.price << std::endl;
		}
	}
	
	return 0;
}


