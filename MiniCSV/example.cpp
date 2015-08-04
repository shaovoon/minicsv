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

int main()
{
	// test file streams
	{
		csv::ofstream os("products.txt");
		os.set_delimiter('\t');
		if (os.is_open())
		{
			Product product("Shampoo", 200, 15.0f);
			os << product.name << product.qty << product.price << NEWLINE;
			Product product2("Soap", 300, 6.0f);
			os << product2.name << product2.qty << product2.price << NEWLINE;
		}
		os.flush();
		os.close();

		csv::ifstream is("products.txt");
		is.set_delimiter('\t');
		if (is.is_open())
		{
			Product temp;
			while (is.read_line())
			{
				is >> temp.name >> temp.qty >> temp.price;
				// display the read items
				std::cout << temp.name << "," << temp.qty << "," << temp.price << std::endl;
			}
		}
	}

	// test string streams
	{
		csv::ostringstream os;
		os.set_delimiter('\t');
		Product product("Shampoo", 200, 15.0f);
		os << product.name << product.qty << product.price << NEWLINE;
		Product product2("Soap", 300, 6.0f);
		os << product2.name << product2.qty << product2.price << NEWLINE;

		csv::istringstream is(os.get_text().c_str());
		is.set_delimiter('\t');
		Product temp;
		while (is.read_line())
		{
			is >> temp.name >> temp.qty >> temp.price;
			// display the read items
			std::cout << temp.name << "," << temp.qty << "," << temp.price << std::endl;
		}
	}
	return 0;
}


