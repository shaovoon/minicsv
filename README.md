# C++: Minimalistic CSV Streams

MiniCSV is a header-only, bare minimal CSV stream based on C++ file streams where the stream operator can be overloaded for your custom type. Its delimiter can be changed on the fly to enable parsing text file format with different delimiters. Newline, delmiiter and quote that is encountered in the input are escaped accordingly. Escape and unescape text can be changed.

This is a example on how to write to CSV.

```cpp
#include "minicsv.h"

struct Product
{
    Product() : name(""), qty(0), price(0.0f) {}
    Product(const std::string& name_, int qty_, float price_) 
        : name(name_), qty(qty_), price(price_) {}
    std::string name;
    int qty;
    float price;
};

int main()
{
    csv::ofstream os("products.txt");
	// For version 1.8.5 and above, give empty string 
	// for the escape string(2nd parameter).
	// Text with comma delimiter will be 
	// enclosed with quotes to be
	// compatible with MS Excel CSV format.
    os.set_delimiter(',', "$$");
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
    csv::ifstream is("products.txt");
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
	
	// Common mistake to forgot to call set_delimiter to 
	// set to original delimiter before next line is read.
	is.set_delimiter(',', "$$"); 
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

## Available public member function interface

### Public member functions of istream_base inherited by ifstream and istringstream

```cpp
// Set newline unescaped text. Default is "&newline;"
void set_newline_unescape(std::string const& newline_unescape_);

// Get newline unescaped text.
std::string const& get_newline_unescape() const;

// Set delimiter and its unescaped text, meaning the unescaped text shall be
// replaced with the delimiter if unescaped text is encountered in the input.
void set_delimiter(char delimiter_, std::string const& unescape_str_);

// Get delimiter 
std::string const& get_delimiter() const;

// Get the unescaped text of delimiter
std::string const& get_unescape_str() const;

// Get the current delimited text
const std::string& get_delimited_str();

// Enable trimming on the string input. unescape shall be replaced with quote
// when encountered in the input. Default unescaped text is "&quot;"
void enable_trim_quote_on_str(bool enable, char quote, const std::string& unescape);

// Get the rest of line that still haven't been delimited yet.
std::string get_rest_of_line() const;

// Enable blank line in between. 
void enable_blank_line(bool enable);

// If enabled, the parsing shall terminate
// when blank line is encountered.
void enable_terminate_on_blank_line(bool enable);

// Query if terminate_on_blank_line is enabled
bool is_terminate_on_blank_line() const;

// Returns number of delimiter in the current line.
// Prefers to call after readline()
size_t num_of_delimiter() const;

// Get the original unparsed line
const std::string& get_line() const;
```

#### Public member functions of ifstream (File stream for reading)

```cpp
// Open a text file for reading
void open(const std::string& file);
void open(const char * file);

// Query whether the file is opened successfully.
bool is_open();

// Reset all the member variables
void init();

// Close the file.
void close();

// Skip this line. Used when the line does not contain delimiter you want.
void skip_line();

// Read the next line. Must be called before the << operator is called.
bool read_line();
```

#### Public member functions of istringstream (String stream for reading)

```cpp
// Set new input string for processing.
void set_new_input_string(const std::string& text);

// Reset all the member variables
void reset();

// Skip this line. Used when the line does not contain delimiter you want.
void skip_line();

// Read the next line. Must be called before the << operator is called.
bool read_line();
```

### Public member functions of ostream_base inherited by ofstream and ostringstream

```cpp
// Enable surround the string input with quote. When quote is encountered in 
// the input, it is replaced with escape. Default escaped text is "&quot;".
void enable_surround_quote_on_str(bool enable, char quote, const std::string& escape);

// Set newline escaped text, meaning newline encountered in the output shall
// be replaced with the escape text. Default escape text is "&newline;"
void set_newline_escape(std::string const& newline_escape_);

// Get newline escaped text.
std::string const& get_newline_escape() const;

// Set delimiter and its escaped text, meaning the delimiter shall be
// replaced with this unescaped text if delimiter is encountered in the output.
// For version 1.8.5 and above, give empty string for the escape string(2nd parameter) for
// text with comma delimiter will be enclosed with quotes to be compatible with MS Excel CSV format.
void set_delimiter(char delimiter_, std::string const& escape_str_);

// Get delimiter.
std::string const& get_delimiter() const;
```

#### Public member functions of ofstream (File stream for writing)

```cpp
// Open a text file for writing, if file exists, it will be overwritten.
void open(const std::string& file);
void open(const char * file);

// Query whether the file is opened successfully
bool is_open();

// Flush the contents to the file. To be called before close.
void flush();

// Close the file.
void close();
```

#### Public member functions of ostringstream (String stream for writing)

```cpp
// Get the text that has been writtten with the << operator
std::string get_text();
```

## FAQ
__Why do the reader stream encounter errors for csv with text not enclosed within quotes?__

Ans: To resolve it, Please remember to call enable_trim_quote_on_str with false.

## Common mistakes

Forget to call set_delmiter() for the next line after changing the delmiter on the fly with the sep class.

## Software that make use of minicsv

* [Maplestory](https://www.nexon.co.jp/rule/license.aspx)
* [OpenMX: Extended Structural Equation Modelling](https://rdrr.io/cran/OpenMx/#vignettes)
