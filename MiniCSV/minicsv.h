// The MIT License (MIT)
// Minimalistic CSV Streams 1.6
// Copyright (C) 2014, by Wong Shao Voon (shaovoon@yahoo.com)
//
// http://opensource.org/licenses/MIT
//
// version 1.2 : make use of make_shared
// version 1.3 : fixed: to when reading the last line and it does not have linefeed
//               added: skip_1st_line and skip_line functions to ifstream class
// version 1.4 : Removed the use of smart ptr.
// version 1.5 : Performance increase on writing without flushing every line.
// version 1.6 : Add string streams

//#define USE_BOOST_LEXICAL_CAST

#ifndef MiniCSV_H
	#define MiniCSV_H

#include <string>
#include <sstream>
#include <fstream>

#ifdef USE_BOOST_LEXICAL_CAST
#	include <boost/lexical_cast.hpp>
#endif

#define NEWLINE '\n'

namespace csv
{

	class ifstream
	{
	public:
		ifstream() : str(""), pos(0), delimiter(',')
		{
		}
		ifstream(const char * file)
		{
			open(file);
		}
		void open(const char * file)
		{
			init();
			istm.open(file, std::ios_base::in);
		}
		void init()
		{
			str = "";
			pos = 0;
			delimiter = ',';
		}
		void close()
		{
			istm.close();
		}
		bool is_open()
		{
			return istm.is_open();
		}
		// eof is replaced by read_line
		//bool eof() const
		void set_delimiter(char delimiter_)
		{
			delimiter = delimiter_;
		}
		char get_delimiter() const
		{
			return delimiter;
		}
		void skip_line()
		{
			if(!istm.eof())
			{
				std::getline(istm, str);
				pos = 0;
			}
		}
		bool read_line()
		{
			this->str = "";
			if(!istm.eof())
			{
				std::getline(istm, this->str);
				pos = 0;

				if(this->str.empty())
					return false;

				return true;
			}
			return false;
		}
		std::string get_delimited_str()
		{
			std::string str = "";
			char ch = '\0';
			do
			{
				if(pos>=this->str.size())
				{
					this->str = "";

					return str;
				}

				ch = this->str[pos];
				++(pos);
				if(ch==delimiter||ch=='\r'||ch=='\n')
					break;

				str += ch;
			}
			while(true);

			return str;
		}
	private:
		std::ifstream istm;
		std::string str;
		size_t pos;
		char delimiter;
	};

	class ofstream
	{
	public:

		ofstream() : after_newline(true), delimiter(',')
		{
		}
		ofstream(const char * file)
		{
			open(file);
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
		void set_delimiter(char delimiter_)
		{
			delimiter = delimiter_;
		}
		char get_delimiter() const
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
		std::ofstream& get_ofstream()
		{
			return ostm;
		}
	private:
		std::ofstream ostm;
		bool after_newline;
		char delimiter;
	};


} // ns csv

template<typename T>
csv::ifstream& operator >> (csv::ifstream& istm, T& val)
{
	std::string str = istm.get_delimited_str();
	
#ifdef USE_BOOST_LEXICAL_CAST
	val = boost::lexical_cast<T>(str);
#else
	std::istringstream is(str);
	is >> val;
#endif

	return istm;
}
template<>
inline csv::ifstream& operator >> (csv::ifstream& istm, std::string& val)
{
	val = istm.get_delimited_str();

	return istm;
}

template<typename T>
csv::ofstream& operator << (csv::ofstream& ostm, const T& val)
{
	if(!ostm.get_after_newline())
		ostm.get_ofstream() << ostm.get_delimiter();

	ostm.get_ofstream() << val;

	ostm.set_after_newline(false);

	return ostm;
}
template<>
inline csv::ofstream& operator << (csv::ofstream& ostm, const char& val)
{
	if(val==NEWLINE)
	{
		ostm.get_ofstream() << NEWLINE;

		ostm.set_after_newline(true);
	}
	else
		ostm.get_ofstream() << val;

	return ostm;
}

namespace csv
{

class istringstream
{
public:
	istringstream(const char * text)
		: str("")
		, pos(0)
		, delimiter(',')
	{
		istm.str(text);
	}
	void set_delimiter(char delimiter_)
	{
		delimiter = delimiter_;
	}
	char get_delimiter() const
	{
		return delimiter;
	}
	void skip_line()
	{
		std::getline(istm, str);
		pos = 0;
	}
	bool read_line()
	{
		this->str = "";
		if (!istm.eof())
		{
			std::getline(istm, this->str);
			pos = 0;

			if (this->str.empty())
				return false;

			return true;
		}
		return false;
	}
	std::string get_delimited_str()
	{
		std::string str = "";
		char ch = '\0';
		do
		{
			if (pos >= this->str.size())
			{
				this->str = "";

				return str;
			}

			ch = this->str[pos];
			++(pos);
			if (ch == delimiter || ch == '\r' || ch == '\n')
				break;

			str += ch;
		} while (true);

		return str;
	}
private:
	std::istringstream istm;
	std::string str;
	size_t pos;
	char delimiter;
};

class ostringstream
{
public:

	ostringstream() : after_newline(true), delimiter(',')
	{
	}
	void set_delimiter(char delimiter_)
	{
		delimiter = delimiter_;
	}
	char get_delimiter() const
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
	std::ostringstream& get_ostringstream()
	{
		return ostm;
	}
	std::string get_text()
	{
		return ostm.str();
	}

private:
	std::ostringstream ostm;
	bool after_newline;
	char delimiter;
};


} // ns csv

template<typename T>
csv::istringstream& operator >> (csv::istringstream& istm, T& val)
{
	std::string str = istm.get_delimited_str();

#ifdef USE_BOOST_LEXICAL_CAST
	val = boost::lexical_cast<T>(str);
#else
	std::istringstream is(str);
	is >> val;
#endif

	return istm;
}
template<>
inline csv::istringstream& operator >> (csv::istringstream& istm, std::string& val)
{
	val = istm.get_delimited_str();

	return istm;
}

template<typename T>
csv::ostringstream& operator << (csv::ostringstream& ostm, const T& val)
{
	if (!ostm.get_after_newline())
		ostm.get_ostringstream() << ostm.get_delimiter();

	ostm.get_ostringstream() << val;

	ostm.set_after_newline(false);

	return ostm;
}
template<>
inline csv::ostringstream& operator << (csv::ostringstream& ostm, const char& val)
{
	if (val == NEWLINE)
	{
		ostm.get_ostringstream() << NEWLINE;

		ostm.set_after_newline(true);
	}
	else
		ostm.get_ostringstream() << val;

	return ostm;
}


#endif // MiniCSV_H