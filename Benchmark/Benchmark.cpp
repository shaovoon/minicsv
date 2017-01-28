// Benchmark.cpp : Defines the entry point for the console application.
//

#include <string>
#include <iomanip>
#include <iostream>
#include <chrono>
#include <vector>
#include "../minicsv/minicsv.h"
#include "minicsv_180.h"

//#define USE_LONG_STRINGS

#ifdef _MSC_VER

#pragma optimize("", off)
template <class T>
void do_not_optimize_away(T&& datum) {
	datum = datum;
}
#pragma optimize("", on)

#else
static void do_not_optimize_away(void* p) {
	asm volatile("" : : "g"(p) : "memory");
}
#endif

class timer
{
public:
	timer() = default;
	void start_timing(const std::string& text_)
	{
		text = text_;
		begin = std::chrono::high_resolution_clock::now();
	}
	void stop_timing()
	{
		auto end = std::chrono::high_resolution_clock::now();
		auto dur = end - begin;
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
		std::cout << std::setw(30) << text << ":" << std::setw(5) << ms << "ms" << std::endl;
	}

private:
	std::string text;
	std::chrono::steady_clock::time_point begin;
};

struct Product
{
	Product() : name(""), qty(0), price(0.0f) {}
	Product(std::string name_, int qty_, float price_) : name(name_), qty(qty_), price(price_) {}
	std::string name;
	int qty;
	float price;
};

void init(std::vector<Product>& vec);

int main(int argc, char *argv[])
{
	const std::string mini_old_file = "F:\\mini_old_products.txt";
	const std::string mini_latest_file = "F:\\mini_latest_products.txt";

	const size_t MAX_LOOP = (argc == 2) ? atoi(argv[1]) : 10000;

	std::vector<Product> vec;
	init(vec);

	std::string result = "";
	const std::string trimChars = "\"";
	timer stopwatch;

	// file stream benchmark
	//=========================
	{
		using namespace mini_180;

		csv::ofstream os(mini_old_file.c_str());
		os.set_delimiter(',', "$$");
		os.enable_surround_quote_on_str(true, '\"');

		if (os.is_open())
		{
			stopwatch.start_timing("mini_180::csv::ofstream");
			for (size_t k = 0; k < MAX_LOOP; ++k)
			{
				for (size_t i = 0; i < vec.size(); ++i)
				{
					const Product& product = vec[i];
					os << product.name << product.qty << product.price << NEWLINE;
					do_not_optimize_away(result.c_str());
				}
			}
			stopwatch.stop_timing();
		}
		os.flush();
		os.close();

		csv::ifstream is(mini_old_file.c_str());
		is.set_delimiter(',', "$$");
		is.enable_trim_quote_on_str(true, '\"');

		if (is.is_open())
		{
			Product product;
			stopwatch.start_timing("mini_180::csv::ifstream");
			while (is.read_line())
			{
				try
				{
					is >> product.name >> product.qty >> product.price;
				}
				catch (std::runtime_error& e)
				{
					fprintf(stderr, "%s\n", e.what());
				}
			}
			stopwatch.stop_timing();
		}
	}
	{
		using namespace mini;

		csv::ofstream os(mini_latest_file.c_str());
		os.set_delimiter(',', "$$");
		os.enable_surround_quote_on_str(true, '\"');

		if (os.is_open())
		{
			stopwatch.start_timing("mini::csv::ofstream");
			for (size_t k = 0; k < MAX_LOOP; ++k)
			{
				for (size_t i = 0; i < vec.size(); ++i)
				{
					const Product& product = vec[i];
					os << product.name << product.qty << product.price << NEWLINE;
					do_not_optimize_away(result.c_str());
				}
			}
			stopwatch.stop_timing();
		}
		os.flush();
		os.close();

		csv::ifstream is(mini_latest_file.c_str());
		is.set_delimiter(',', "$$");
		is.enable_trim_quote_on_str(true, '\"');

		if (is.is_open())
		{
			Product product;
			stopwatch.start_timing("mini::csv::ifstream");
			while (is.read_line())
			{
				try
				{
					is >> product.name >> product.qty >> product.price;
				}
				catch (std::runtime_error& e)
				{
					fprintf(stderr, "%s\n", e.what());
					break;
				}
			}
			stopwatch.stop_timing();
		}
		is.close();
	}
	// string stream benchmark
	//=========================
	{
		using namespace mini_180;

		csv::ostringstream os;
		os.set_delimiter(',', "$$");
		os.enable_surround_quote_on_str(true, '\"');

		stopwatch.start_timing("mini_180::csv::ostringstream");
		for (size_t k = 0; k < MAX_LOOP; ++k)
		{
			for (size_t i = 0; i < vec.size(); ++i)
			{
				const Product& product = vec[i];
				os << product.name << product.qty << product.price << NEWLINE;
				do_not_optimize_away(result.c_str());
			}
		}
		stopwatch.stop_timing();

		csv::istringstream is(os.get_text().c_str());
		is.set_delimiter(',', "$$");
		is.enable_trim_quote_on_str(true, '\"');

		Product product;
		stopwatch.start_timing("mini_180::csv::istringstream");
		while (is.read_line())
		{
			try
			{
				is >> product.name >> product.qty >> product.price;
			}
			catch (std::runtime_error& e)
			{
				fprintf(stderr, "%s\n", e.what());
			}
		}
		stopwatch.stop_timing();
	}
	{
		using namespace mini;

		csv::ostringstream os;
		os.set_delimiter(',', "$$");
		os.enable_surround_quote_on_str(true, '\"');

		stopwatch.start_timing("mini::csv::ostringstream");
		for (size_t k = 0; k < MAX_LOOP; ++k)
		{
			for (size_t i = 0; i < vec.size(); ++i)
			{
				const Product& product = vec[i];
				os << product.name << product.qty << product.price << NEWLINE;
				do_not_optimize_away(result.c_str());
			}
		}
		stopwatch.stop_timing();

		csv::istringstream is(os.get_text().c_str());
		is.set_delimiter(',', "$$");
		is.enable_trim_quote_on_str(true, '\"');

		Product product;
		stopwatch.start_timing("mini::csv::istringstream");
		while (is.read_line())
		{
			try
			{
				is >> product.name >> product.qty >> product.price;
			}
			catch (std::runtime_error& e)
			{
				fprintf(stderr, "%s\n", e.what());
			}
		}
		stopwatch.stop_timing();
	}

	return 0;
}

void init(std::vector<Product>& vec)
{
#ifdef USE_LONG_STRINGS
	// long strings
	vec.push_back(Product("Apples and Oranges and Watermelon", 5, 2.5f));
	vec.push_back(Product("Shampoo and Conditioners", 125, 12.5f));
	vec.push_back(Product("Soap / body foam / Towel", 250, 30.25f));
	vec.push_back(Product("Instant Ramen / Udon", 125, 12.5f));
	vec.push_back(Product("Canned Sardines / Baked beans", 20, 50.7f));
	vec.push_back(Product("Color Book / Color pencils / Drawing Paper", 10, 10.5f));
	vec.push_back(Product("Audio Speakers / Subwoofer", 40, 1089.75f));
	vec.push_back(Product("Camera and Photo Holder", 34, 56.85f));
	vec.push_back(Product("Classic and Modern Microphone", 12, 572.43f));
	vec.push_back(Product("Story books / Novels / Periodicals", 8, 1250.62f));
#else
	// short strings
	vec.push_back(Product("Apples", 5, 2.5f));
	vec.push_back(Product("Shampoo", 125, 12.5f));
	vec.push_back(Product("Soap", 250, 30.25f));
	vec.push_back(Product("Instant Ramen", 125, 12.5f));
	vec.push_back(Product("Canned Sardines", 20, 50.7f));
	vec.push_back(Product("Color Book", 10, 10.5f));
	vec.push_back(Product("Speakers", 40, 1089.75f));
	vec.push_back(Product("Photo Holder", 34, 56.85f));
	vec.push_back(Product("Microphone", 12, 572.43f));
	vec.push_back(Product("Sound card", 8, 1250.62f));
#endif
}
