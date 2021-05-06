// The MIT License (MIT)
// Minimalistic CSV Streams 1.8.6
// Copyright (C) 2014 - 2021, by Wong Shao Voon (shaovoon@yahoo.com)
//
// http://opensource.org/licenses/MIT
//
// version 1.2   : make use of make_shared
// version 1.3   : fixed: to when reading the last line and it does not have linefeed
//                 added: skip_1st_line and skip_line functions to ifstream class
// version 1.4   : Removed the use of smart ptr.
// version 1.5   : Performance increase on writing without flushing every line.
// version 1.6   : Add string streams
// version 1.7   : You MUST specify the escape/unescape string when calling set_delimiter. Option to surround/trim string with quotes
// version 1.7.1 : Add stream operator overload usage in example.cpp
//                 Disable the surround/trim quote on text by default
// version 1.7.2 : Stream operator overload for const char*
// version 1.7.3 : Add num_of_delimiter method to ifstream and istringstream
//                 Fix g++ compilation errors
// version 1.7.4 : Add get_rest_of_line
// version 1.7.5 : Add terminate_on_blank_line variable. Set to false if your file format has blank lines in between.
// version 1.7.6 : Ignore delimiters within quotes during reading when enable_trim_quote_on_str is true;
// version 1.7.7 : Fixed multiple symbol linkage errors
// version 1.7.8 : Add quote escape/unescape. Default is "&quot;"
// version 1.7.9 : Reading UTF-8 BOM
// version 1.7.10 : separator class for the stream, so that no need to call set_delimiter repeatedly if delimiter keep changing
// version 1.7.11 : Fixed num_of_delimiters function: do not count delimiter within quotes
// version 1.8.0  : Add meaningful error message for data conversion during reading
// version 1.8.1  : Put under the mini namespace
// version 1.8.2  : Optimize input stream.
// version 1.8.3  : Add unit test and enable to use 2 quotes to escape 1 quote
// version 1.8.4  : Add NChar class and fix error of no delimiter written for char type.
// version 1.8.5  : Can have an unescaped delimiter which will be enclosed by quote automatically
// version 1.8.5b : Compilation error fix for NCHar
// version 1.8.5c : Change from _WIN32 to _MSC_VER for the macro check for MY_FUNC_SIG
// version 1.8.5d : Unescape quote infinite loop fix
// version 1.8.5e : Put common functions in the base class, hereby reducing 160 LOC
// version 1.8.6  : Escape newlines when detected in the string input.
//#define USE_BOOST_LEXICAL_CAST

#ifndef MiniCSV_H
	#define MiniCSV_H

#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <stdexcept>

#ifdef USE_BOOST_LEXICAL_CAST
#	include <boost/lexical_cast.hpp>
#endif

#define NEWLINE '\n'

#ifdef _MSC_VER
	#define MY_FUNC_SIG __FUNCSIG__
#else
	#define MY_FUNC_SIG __PRETTY_FUNCTION__
#endif

namespace mini
{

	namespace csv
	{
		struct NChar
		{
			explicit NChar(char& ch_) : ch(ch_) {}
			const char& getChar() const { return ch; }
			char& getChar() { return ch; }
			void setChar(char ch_) { ch = ch_; }
		private:
			char& ch;
		};

		inline std::string const & replace(std::string & src, std::string const & to_find, std::string const & to_replace)
		{
			size_t pos = 0;
			while (std::string::npos != pos)
			{
				pos = src.find(to_find, pos);

				if (std::string::npos != pos)
				{
					src.erase(pos, to_find.size());
					src.insert(pos, to_replace);
					pos += to_replace.size();
				}
			}

			return src;
		}

		class sep // separator class for the stream, so that no need to call set_delimiter
		{
		public:
			sep(const char delimiter_, const std::string& escape_) : delimiter(delimiter_), escape(escape_) {}

			const char get_delimiter() const { return delimiter; }
			const std::string& get_escape() const { return escape; }
		private:
			const char delimiter;
			const std::string escape;
		};

		class istream_base
		{
		public:
			istream_base()
				: pos(0)
				, delimiter(",")
				, unescape_str("##")
				, trim_quote_on_str(false)
				, trim_quote('\"')
				, trim_quote_str(1, trim_quote)
				, terminate_on_blank_line(true)
				, quote_unescape("&quot;")
				, newline_unescape("&newline;")
				, line_num(0)
				, token_num(0)
				, allow_blank_line(false)
			{
			}
			void set_newline_unescape(std::string const& newline_unescape_)
			{
				newline_unescape = newline_unescape_;
			}
			std::string const& get_newline_unescape() const
			{
				return newline_unescape;
			}
			// eof is replaced by read_line
			//bool eof() const
			void set_delimiter(char delimiter_, std::string const& unescape_str_)
			{
				delimiter = delimiter_;
				unescape_str = unescape_str_;
			}
			std::string const& get_delimiter() const
			{
				return delimiter;
			}
			const std::string& get_delimited_str()
			{
				token = "";
				char ch = '\0';
				bool within_quote = false;
				do
				{
					if (pos >= this->str.size())
					{
						this->str = "";

						++token_num;
						token = unescape(token);
						return token;
					}

					ch = this->str[pos];
					//if (trim_quote_on_str)
					{
						if (within_quote && ch == trim_quote && this->str[pos + 1] == trim_quote)
						{
							token += ch;
							pos += 2;
							continue;
						}

						if (within_quote == false && ch == trim_quote && ((pos > 0 && this->str[pos - 1] == delimiter[0]) || pos == 0))
							within_quote = true;
						else if (within_quote && ch == trim_quote)
							within_quote = false;
					}

					++(pos);

					if (ch == delimiter[0] && within_quote == false)
						break;
					if (ch == '\r' || ch == '\n')
						break;

					token += ch;
				} while (true);

				++token_num;
				token = unescape(token);
				return token;
			}
			void enable_trim_quote_on_str(bool enable, char quote, const std::string& unescape = "&quot;")
			{
				trim_quote_on_str = enable;
				trim_quote = quote;
				trim_quote_str = std::string(1, trim_quote);
				quote_unescape = unescape;
			}
			std::string get_rest_of_line() const
			{
				return str.substr(pos);
			}

		protected:
			void enable_blank_line(bool enable)
			{
				allow_blank_line = enable;
			}
			std::string const& get_unescape_str() const
			{
				return unescape_str;
			}
			std::string unescape(std::string& src)
			{
				src = unescape_str.empty() ? src : replace(src, unescape_str, delimiter);

				//if (trim_quote_on_str)
				{
					if (!src.empty() && (src[0] == trim_quote && src[src.size() - 1] == trim_quote))
					{
						src = src.substr(1, src.size() - 2);
					}

					if (!newline_unescape.empty() && std::string::npos != src.find(newline_unescape, 0))
					{
						src = replace(src, newline_unescape, "\n");
					}

					if (!quote_unescape.empty() && std::string::npos != src.find(quote_unescape, 0))
					{
						src = replace(src, quote_unescape, trim_quote_str);
					}
				}

				return src;
			}
			size_t num_of_delimiter() const
			{
				if (delimiter.size() == 0)
					return 0;

				size_t cnt = 0;
				//if (trim_quote_on_str)
				{
					bool inside_quote = false;
					for (size_t i = 0; i < str.size(); ++i)
					{
						if (str[i] == trim_quote)
							inside_quote = !inside_quote;

						if (!inside_quote)
						{
							if (str[i] == delimiter[0])
								++cnt;
						}
					}
				}
				return cnt;
			}
			const std::string& get_line() const
			{
				return str;
			}
			void enable_terminate_on_blank_line(bool enable)
			{
				terminate_on_blank_line = enable;
			}
			bool is_terminate_on_blank_line() const
			{
				return terminate_on_blank_line;
			}
		public:
			std::string error_line(const std::string& token, const std::string& function_site)
			{
				std::ostringstream is;
				is << "csv::istream_base Conversion error at line no.:" << line_num
					<< ", filename:" << filename << ", token position:" << token_num
					<< ", token:" << token << ", function:" << function_site;

				return is.str();
			}

		protected:
			std::string str;
			size_t pos;
			std::string delimiter;
			std::string unescape_str;
			bool trim_quote_on_str;
			char trim_quote;
			std::string trim_quote_str;
			bool terminate_on_blank_line;
			std::string quote_unescape;
			std::string newline_unescape;
			std::string filename;
			size_t line_num;
			size_t token_num;
			std::string token;
			bool allow_blank_line;
		};
		class ifstream : public istream_base
		{
		public:
			ifstream(const std::string& file="")
				: istream_base()
				, has_bom(false)
				, first_line_read(false)
			{
				open(file);
			}
			ifstream(const char * file)
				: istream_base()
			{
				open(file);
			}
			void open(const std::string& file)
			{
				if (!file.empty())
					open(file.c_str());
			}
			void open(const char * file)
			{
				init();
				filename = file;
				istm.open(file, std::ios_base::in);
				read_bom();
			}
			void read_bom()
			{
				char tt[3] = { 0, 0, 0 };

				istm.read(tt, sizeof(tt));

				if (tt[0] == (char)0xEF || tt[1] == (char)0xBB || tt[2] == (char)0xBF) // not the correct BOM, so reset the pos to beginning (file might not have BOM)
					has_bom = true;

				istm.seekg(0, istm.beg);
			}
			void init()
			{
				str = "";
				pos = 0;
				delimiter = ',';
				unescape_str = "##";
				trim_quote_on_str = false;
				trim_quote = '\"';
				trim_quote_str = std::string(1, trim_quote);
				terminate_on_blank_line = true;
				has_bom = false;
				first_line_read = false;
				filename = "";
				line_num = 0;
				token_num = 0;
				allow_blank_line = false;
			}
			void close()
			{
				istm.close();
			}
			bool is_open()
			{
				return istm.is_open();
			}
			void skip_line()
			{
				if (!istm.eof())
				{
					std::getline(istm, str);
					pos = 0;

					if (first_line_read == false)
					{
						first_line_read = true;
					}
				}
			}
			bool read_line()
			{
				this->str = "";
				while (!istm.eof())
				{
					std::getline(istm, this->str);
					pos = 0;

					if (first_line_read == false)
					{
						first_line_read = true;
						if (has_bom)
						{
							this->str = this->str.substr(3);
						}
					}

					if (this->str.empty())
					{
						if (terminate_on_blank_line)
							break;
						else if (allow_blank_line == false)
							continue;
					}

					++line_num;
					token_num = 0;
					return true;
				}
				return false;
			}

		private:
			std::ifstream istm;
			bool has_bom;
			bool first_line_read;
			std::string filename;
		};
		class ostream_base
		{
		public:
			ostream_base()
				: after_newline(true)
				, delimiter(",")
				, escape_str("##")
				, surround_quote_on_str(false)
				, surround_quote('\"')
				, quote_escape("&quot;")
				, newline_escape("&newline;")
			{
			}
		public:
			void enable_surround_quote_on_str(bool enable, char quote, const std::string& escape = "&quot;")
			{
				surround_quote_on_str = enable;
				surround_quote = quote;
				quote_escape = escape;
			}
			void set_newline_escape(std::string const& newline_escape_)
			{
				newline_escape = newline_escape_;
			}
			std::string const& get_newline_escape() const
			{
				return newline_escape;
			}
			void set_delimiter(char delimiter_, std::string const& escape_str_)
			{
				delimiter = delimiter_;
				escape_str = escape_str_;
			}
			std::string const& get_delimiter() const
			{
				return delimiter;
			}
			void set_after_newline(bool after_newline_)
			{
				after_newline = after_newline_;
			}
			bool get_after_newline() const
			{
				return after_newline;
			}
		protected:
			std::string const& get_escape_str() const
			{
				return escape_str;
			}
		protected:
			bool after_newline;
			std::string delimiter;
			std::string escape_str;
			bool surround_quote_on_str;
			char surround_quote;
			std::string quote_escape;
			std::string newline_escape;
		};
		class ofstream : public ostream_base
		{
		public:

			ofstream(const std::string& file = "")
				: ostream_base()
			{
				open(file);
			}
			ofstream(const char * file)
				: ostream_base()
			{
				open(file);
			}
			void open(const std::string& file)
			{
				if (!file.empty())
					open(file.c_str());
			}
			void open(const char * file)
			{
				init();
				ostm.open(file, std::ios_base::out);
			}
			void init()
			{
				after_newline = true;
				delimiter = ',';
				escape_str = "##";
				surround_quote_on_str = false;
				surround_quote = '\"';
				quote_escape = "&quot;";
			}
			void flush()
			{
				ostm.flush();
			}
			void close()
			{
				ostm.close();
			}
			bool is_open()
			{
				return ostm.is_open();
			}
			std::ofstream& get_ofstream()
			{
				return ostm;
			}
			void escape_and_output(std::string src)
			{
				ostm << ((escape_str.empty()) ? src : replace(src, delimiter, escape_str));
			}
			void escape_str_and_output(std::string src)
			{
				src = ((escape_str.empty()) ? src : replace(src, delimiter, escape_str));
				if (!newline_escape.empty())
				{
					src = replace(src, std::string(1, '\n'), newline_escape);
				}
				if (surround_quote_on_str || src.find(delimiter) != std::string::npos)
				{
					if (!quote_escape.empty())
					{
						src = replace(src, std::string(1, surround_quote), quote_escape);
					}
					ostm << surround_quote << src << surround_quote;
				}
				else
				{
					ostm << src;
				}
			}
		private:
			std::ofstream ostm;
		};


	} // ns csv
} // ns mini

template<typename T>
mini::csv::ifstream& operator >> (mini::csv::ifstream& istm, T& val)
{
	const std::string& str = istm.get_delimited_str();

#ifdef USE_BOOST_LEXICAL_CAST
	try 
	{
		val = boost::lexical_cast<T>(str);
	}
	catch (boost::bad_lexical_cast& e)
	{
		throw std::runtime_error(istm.error_line(str, MY_FUNC_SIG).c_str());
	}
#else
	std::istringstream is(str);
	is >> val;
	if (!(bool)is)
	{
		throw std::runtime_error(istm.error_line(str, MY_FUNC_SIG).c_str());
	}
#endif

	return istm;
}
template<>
inline mini::csv::ifstream& operator >> (mini::csv::ifstream& istm, std::string& val)
{
	val = istm.get_delimited_str();

	return istm;
}

template<>
inline mini::csv::ifstream& operator >> (mini::csv::ifstream& istm, mini::csv::sep& val)
{
	istm.set_delimiter(val.get_delimiter(), val.get_escape());

	return istm;
}

inline mini::csv::ifstream& operator >> (mini::csv::ifstream& istm, mini::csv::NChar val)
{
	const std::string& str = istm.get_delimited_str();

	int n = 0;
#ifdef USE_BOOST_LEXICAL_CAST
	try
	{
		n = boost::lexical_cast<int>(str);
	}
	catch (boost::bad_lexical_cast& e)
	{
		throw std::runtime_error(istm.error_line(str, MY_FUNC_SIG).c_str());
	}
#else
	std::istringstream is(str);
	is >> n;
	if (!(bool)is)
	{
		throw std::runtime_error(istm.error_line(str, MY_FUNC_SIG).c_str());
	}
#endif

	if (n > 127 || n < -128)
	{
		throw std::runtime_error(istm.error_line(str, MY_FUNC_SIG).c_str());
	}

	char temp = static_cast<char>(n);
	val.setChar(temp);

	return istm;
}

template<>
inline mini::csv::ifstream& operator >> (mini::csv::ifstream& istm, char& val)
{
	const std::string& src = istm.get_delimited_str();

	if (src.empty())
	{
		throw std::runtime_error(istm.error_line(src, MY_FUNC_SIG).c_str());
	}

	val = src[0];

	return istm;
}

template<typename T>
mini::csv::ofstream& operator << (mini::csv::ofstream& ostm, const T& val)
{
	if(!ostm.get_after_newline())
		ostm.get_ofstream() << ostm.get_delimiter();

	std::ostringstream os_temp;

	os_temp << val;

	ostm.escape_and_output(os_temp.str());

	ostm.set_after_newline(false);

	return ostm;
}

template<typename T>
mini::csv::ofstream& operator << (mini::csv::ofstream& ostm, const T* val)
{
	if (!ostm.get_after_newline())
		ostm.get_ofstream() << ostm.get_delimiter();

	std::ostringstream os_temp;

	os_temp << *val;

	ostm.escape_and_output(os_temp.str());

	ostm.set_after_newline(false);

	return ostm;
}

template<>
inline mini::csv::ofstream& operator << (mini::csv::ofstream& ostm, const std::string& val)
{
	if (!ostm.get_after_newline())
		ostm.get_ofstream() << ostm.get_delimiter();

	std::string temp = val;
	ostm.escape_str_and_output(temp);

	ostm.set_after_newline(false);

	return ostm;
}

template<>
inline mini::csv::ofstream& operator << (mini::csv::ofstream& ostm, const mini::csv::sep& val)
{
	ostm.set_delimiter(val.get_delimiter(), val.get_escape());

	return ostm;
}

template<>
inline mini::csv::ofstream& operator << (mini::csv::ofstream& ostm, const char& val)
{
	if(val==NEWLINE)
	{
		ostm.get_ofstream() << NEWLINE;

		ostm.set_after_newline(true);
	}
	else
	{
		if (!ostm.get_after_newline())
			ostm.get_ofstream() << ostm.get_delimiter();

		std::string temp = "";
		temp += val;
		ostm.escape_str_and_output(temp);

		ostm.set_after_newline(false);
	}

	return ostm;
}

inline mini::csv::ofstream& operator << (mini::csv::ofstream& ostm, mini::csv::NChar val)
{
	if (!ostm.get_after_newline())
		ostm.get_ofstream() << ostm.get_delimiter();

	std::ostringstream os_temp;

	os_temp << static_cast<int>(val.getChar());

	ostm.escape_and_output(os_temp.str());

	ostm.set_after_newline(false);

	return ostm;
}

template<>
inline mini::csv::ofstream& operator << (mini::csv::ofstream& ostm, const char* val)
{
	const std::string temp = val;

	ostm << temp;

	return ostm;
}

namespace mini
{
	namespace csv
	{
		class istringstream : public istream_base
		{
		public:
			istringstream(const char * text)
				: istream_base()
			{
				set_new_input_string(text);
			}
			istringstream(const std::string& text)
				: istream_base()
			{
				set_new_input_string(text);
			}
			void set_new_input_string(const std::string& text)
			{
				reset();
				istm.str(text);
				istm.clear();
			}
			void reset()
			{
				str = "";
				pos = 0;
				delimiter = ",";
				unescape_str = "##";
				trim_quote_on_str = false;
				trim_quote = '\"';
				trim_quote_str = std::string(1, trim_quote);
				terminate_on_blank_line = true;
				quote_unescape = "&quot;";
				line_num = 0;
				token_num = 0;
				allow_blank_line = false;
			}
			void skip_line()
			{
				std::getline(istm, str);
				pos = 0;
			}
			bool read_line()
			{
				this->str = "";
				while (!istm.eof())
				{
					std::getline(istm, this->str);
					pos = 0;

					if (this->str.empty())
					{
						if (terminate_on_blank_line)
							break;
						else if (allow_blank_line == false)
							continue;
					}

					++line_num;
					token_num = 0;
					return true;
				}
				return false;
			}

		private:
			std::istringstream istm;
		};

		class ostringstream : public ostream_base
		{
		public:
			ostringstream()
				: ostream_base()
			{
			}
			std::ostringstream& get_ostringstream()
			{
				return ostm;
			}
			std::string get_text()
			{
				return ostm.str();
			}
			void escape_and_output(std::string src)
			{
				ostm << ((escape_str.empty()) ? src : replace(src, delimiter, escape_str));
			}
			void escape_str_and_output(std::string src)
			{
				src = ((escape_str.empty()) ? src : replace(src, delimiter, escape_str));
				if (!newline_escape.empty())
				{
					src = replace(src, std::string(1, '\n'), newline_escape);
				}
				if (surround_quote_on_str || src.find(delimiter) != std::string::npos)
				{
					if (!quote_escape.empty())
					{
						src = replace(src, std::string(1, surround_quote), quote_escape);
					}
					ostm << surround_quote << src << surround_quote;
				}
				else
				{
					ostm << src;
				}
			}

		private:
			std::ostringstream ostm;
		};


	} // ns csv
} // ns mini

template<typename T>
mini::csv::istringstream& operator >> (mini::csv::istringstream& istm, T& val)
{
	const std::string& str = istm.get_delimited_str();

#ifdef USE_BOOST_LEXICAL_CAST
	try
	{
		val = boost::lexical_cast<T>(str);
	}
	catch (boost::bad_lexical_cast& e)
	{
		throw std::runtime_error(istm.error_line(str, MY_FUNC_SIG).c_str());
	}
#else
	std::istringstream is(str);
	is >> val;
	if (!(bool)is)
	{
		throw std::runtime_error(istm.error_line(str, MY_FUNC_SIG).c_str());
	}
#endif

	return istm;
}
template<>
inline mini::csv::istringstream& operator >> (mini::csv::istringstream& istm, std::string& val)
{
	val = istm.get_delimited_str();

	return istm;
}

template<>
inline mini::csv::istringstream& operator >> (mini::csv::istringstream& istm, mini::csv::sep& val)
{
	istm.set_delimiter(val.get_delimiter(), val.get_escape());

	return istm;
}

inline mini::csv::istringstream& operator >> (mini::csv::istringstream& istm, mini::csv::NChar val)
{
	const std::string& str = istm.get_delimited_str();

	int n = 0;
#ifdef USE_BOOST_LEXICAL_CAST
	try
	{
		n = boost::lexical_cast<int>(str);
	}
	catch (boost::bad_lexical_cast& e)
	{
		throw std::runtime_error(istm.error_line(str, MY_FUNC_SIG).c_str());
	}
#else
	std::istringstream is(str);
	is >> n;
	if (!(bool)is)
	{
		throw std::runtime_error(istm.error_line(str, MY_FUNC_SIG).c_str());
	}
#endif

	if (n > 127 || n < -128)
	{
		throw std::runtime_error(istm.error_line(str, MY_FUNC_SIG).c_str());
	}

	char temp = static_cast<char>(n);
	val.setChar(temp);

	return istm;
}

template<>
inline mini::csv::istringstream& operator >> (mini::csv::istringstream& istm, char& val)
{
	const std::string& src = istm.get_delimited_str();

	if (src.empty())
	{
		throw std::runtime_error(istm.error_line(src, MY_FUNC_SIG).c_str());
	}

	val = src[0];

	return istm;
}

template<typename T>
mini::csv::ostringstream& operator << (mini::csv::ostringstream& ostm, const T& val)
{
	if (!ostm.get_after_newline())
		ostm.get_ostringstream() << ostm.get_delimiter();

	std::ostringstream os_temp;

	os_temp << val;

	ostm.escape_and_output(os_temp.str());

	ostm.set_after_newline(false);

	return ostm;
}
template<typename T>
mini::csv::ostringstream& operator << (mini::csv::ostringstream& ostm, const T* val)
{
	if (!ostm.get_after_newline())
		ostm.get_ostringstream() << ostm.get_delimiter();

	std::ostringstream os_temp;

	os_temp << *val;

	ostm.escape_and_output(os_temp.str());

	ostm.set_after_newline(false);

	return ostm;
}
template<>
inline mini::csv::ostringstream& operator << (mini::csv::ostringstream& ostm, const std::string& val)
{
	if (!ostm.get_after_newline())
		ostm.get_ostringstream() << ostm.get_delimiter();

	std::string temp = val;
	ostm.escape_str_and_output(temp);

	ostm.set_after_newline(false);

	return ostm;
}
template<>
inline mini::csv::ostringstream& operator << (mini::csv::ostringstream& ostm, const mini::csv::sep& val)
{
	ostm.set_delimiter(val.get_delimiter(), val.get_escape());

	return ostm;
}

template<>
inline mini::csv::ostringstream& operator << (mini::csv::ostringstream& ostm, const char& val)
{
	if (val == NEWLINE)
	{
		ostm.get_ostringstream() << NEWLINE;

		ostm.set_after_newline(true);
	}
	else
	{
		if (!ostm.get_after_newline())
			ostm.get_ostringstream() << ostm.get_delimiter();

		std::string temp = "";
		temp += val;
		ostm.escape_str_and_output(temp);

		ostm.set_after_newline(false);
	}

	return ostm;
}

inline mini::csv::ostringstream& operator << (mini::csv::ostringstream& ostm, mini::csv::NChar val)
{
	if (!ostm.get_after_newline())
		ostm.get_ostringstream() << ostm.get_delimiter();

	std::ostringstream os_temp;

	os_temp << static_cast<int>(val.getChar());

	ostm.escape_and_output(os_temp.str());

	ostm.set_after_newline(false);

	return ostm;
}

template<>
inline mini::csv::ostringstream& operator << (mini::csv::ostringstream& ostm, const char* val)
{
	const std::string temp = val;

	ostm << temp;

	return ostm;
}

#endif // MiniCSV_H