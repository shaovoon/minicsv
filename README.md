# C++: Minimalistic CSV Streams

Bare minimal CSV stream based on C++ file streams where the stream operator can be overloaded for your custom type.

This is a example on how to write to CSV.

```cpp
#include "minicsv.h"

struct Product
{
    Product() : name(""), qty(0), price(0.0f) {}
    Product(std::string name_, int qty_, float price_) 
        : name(name_), qty(qty_), price(price_) {}
    std::string name;
    int qty;
    float price;
};

int main()
{
    csv::ofstream os("products.txt", std::ios_base::out);
	// For version 1.8.5 and above, give empty string 
	// for the escape string(2nd parameter).
	// Text with comma delimiter will be 
	// enclosed with quotes to be
	// compatible with MS Excel CSV format.
    os.set_delimiter(',', "");
    if(os.is_open())
    {
        Product product("Shampoo", 200, 15.0f);
        os << product.name << product.qty << product.price << NEWLINE;
        Product product2("Towel, Soap, Shower Foam", 300, 6.0f);
        os << product2.name << product2.qty << product2.price << NEWLINE;
    }
    os.flush();
    return 0;
}
```

This is a example on how to read from the same CSV.

```cpp
#include "minicsv.h"
#include <iostream>

int main()
{
    csv::ifstream is("products.txt", std::ios_base::in);
    is.set_delimiter(',', "$$");
    if(is.is_open())
    {
        Product temp;
        while(is.read_line())
        {
            is >> temp.name >> temp.qty >> temp.price;
            // display the read items
            std::cout << temp.name << "|" << temp.qty << "|" << temp.price << std::endl;
        }
    }
    return 0;
}
```

The file contents is shown below.

```
Shampoo,200,15
Towel$$ Soap$$ Shower Foam,300,6
```

The console output is shown below.

```
Shampoo|200|15
Towel, Soap, Shower Foam|300|6
```

Here is on how to overload the operator for your custom type.

```cpp
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
    // test string streams using overloaded stream operators for Product
	csv::ostringstream os;
	// For version 1.8.5 and above, give empty string 
	// for the escape string(2nd parameter).
	// Text with comma delimiter will be 
	// enclosed with quotes to be
	// compatible with MS Excel CSV format.
	os.set_delimiter(',', "");
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
    return 0;
}
```

**Version 1.7.10** added separator class for the stream, so that delimiter can be changed on the fly.

```cpp
csv::istringstream is("vt:33,44,66");
is.set_delimiter(',', "$$");
csv::sep colon(':', "<colon>");
csv::sep comma(',', "<comma>");
while (is.read_line())
{
    std::string type;
    int r = 0, b = 0, g = 0;
    is >> colon >> type >> comma >> r >> b >> g;
    // display the read items
    std::cout << type << "|" << r << "|" << b << "|" << g << std::endl;
}
```

**Version 1.8.4** fixed some char output problems and added NChar (char wrapper) class to write to numeric value[-127..128] to char variables.

```cpp
bool test_nchar(bool enable_quote)
{
    csv::ostringstream os;
    os.set_delimiter(',', "$$");
    os.enable_surround_quote_on_str(enable_quote, '\"');

    os << "Wallet" << 56 << NEWLINE;

    csv::istringstream is(os.get_text().c_str());
    is.set_delimiter(',', "$$");
    is.enable_trim_quote_on_str(enable_quote, '\"');

    while (is.read_line())
    {
        try
        {
            std::string dest_name = "";
            char dest_char = 0;

            is >> dest_name >> csv::NChar(dest_char);

            std::cout << dest_name << ", " << (int)dest_char << std::endl;
        }
        catch (std::runtime_error& e)
        {
            std::cerr << __FUNCTION__ << e.what() << std::endl;
        }
    }
    return true;
}
```

Output

```
Wallet, 56
```

[CodeProject Tutorial](http://www.codeproject.com/Articles/741183/Minimalistic-CSV-Streams)
[Maplestory make use of MiniCSV library](https://www.nexon.co.jp/rule/license.aspx)
