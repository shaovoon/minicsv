// The MIT License (MIT)
// Minimalistic CSV Streams 1.2
// Copyright (C) 2014, by Wong Shao Voon (shaovoon@yahoo.com)
//
// http://opensource.org/licenses/MIT
//
// version 1.2 : make use of make_shared

#pragma once

//#define USE_BOOST_PTR
//#define USE_BOOST_LEXICAL_CAST

#ifndef MiniCSV_H
	#define MiniCSV_H

#include <string>
#include <sstream>
#include <fstream>
#ifdef USE_BOOST_PTR
#	include <boost/shared_ptr.hpp>
#	include <boost/make_shared.hpp>
#else
#	include <memory>
#endif

#ifdef USE_BOOST_LEXICAL_CAST
#	include <boost/lexical_cast.hpp>
#endif

#define NEWLINE '\n'

namespace csv
{

	struct istruct
	{
		istruct() : str(""), pos(0), delimiter(',') {}
		std::ifstream istm;
		std::string str;
		size_t pos;
		char delimiter;
	};
	class ifstream
	{
	public:
#ifdef USE_BOOST_PTR
	typedef boost::shared_ptr<istruct> istruct_ptr;
#else
	typedef std::shared_ptr<istruct> istruct_ptr;
#endif
		ifstream()
		{
#ifdef USE_BOOST_PTR
			m_ptr = boost::make_shared<istruct>();
#else
			m_ptr = std::make_shared<istruct>();
#endif
		}
		ifstream(const char * file, std::ios_base::openmode mode)
		{
#ifdef USE_BOOST_PTR
			m_ptr = boost::make_shared<istruct>();
#else
			m_ptr = std::make_shared<istruct>();
#endif
			m_ptr->istm.open(file, mode);
		}
		ifstream(ifstream& other)
		{
			m_ptr = other.m_ptr;
		}
		void open(const char * file, std::ios_base::openmode mode)
		{
			m_ptr->istm.open(file, mode);
		}
		void close()
		{
			m_ptr->istm.close();
		}
		bool is_open()
		{
			return m_ptr->istm.is_open();
		}
		bool eof() const
		{
			return m_ptr->istm.eof();
		}
		void set_delimiter(char delimiter)
		{
			m_ptr->delimiter = delimiter;
		}
		char get_delimiter() const
		{
			return m_ptr->delimiter;
		}
		std::string get_delimited_str()
		{
			std::string str = "";
			char ch = '\0';
			do
			{
				if(m_ptr->pos>=m_ptr->str.size())
				{
					if(!m_ptr->istm.eof())
					{
						std::getline(m_ptr->istm, m_ptr->str);
						m_ptr->pos = 0;
					}
					else
						break;

					if(!str.empty())
						return str;
				}

				ch = m_ptr->str[m_ptr->pos];
				++(m_ptr->pos);
				if(ch==m_ptr->delimiter||ch=='\r'||ch=='\n')
					break;

				str += ch;
			}
			while(true);

			return str;
		}
	private:
		istruct_ptr m_ptr;

	};

	struct ostruct
	{
		ostruct() : after_newline(true), delimiter(',') {}
		std::ofstream ostm;
		bool after_newline;
		char delimiter;
	};
	class ofstream
	{
	public:
#ifdef USE_BOOST_PTR
	typedef boost::shared_ptr<ostruct> ostruct_ptr;
#else
	typedef std::shared_ptr<ostruct> ostruct_ptr;
#endif

		ofstream()
		{
#ifdef USE_BOOST_PTR
			m_ptr = boost::make_shared<ostruct>();
#else
			m_ptr = std::make_shared<ostruct>();
#endif
		}
		ofstream(const char * file, std::ios_base::openmode mode)
		{
#ifdef USE_BOOST_PTR
			m_ptr = boost::make_shared<ostruct>();
#else
			m_ptr = std::make_shared<ostruct>();
#endif
			m_ptr->ostm.open(file, mode);
		}
		ofstream(ofstream& other)
		{
			m_ptr = other.m_ptr;
		}
		void open(const char * file, std::ios_base::openmode mode)
		{
			m_ptr->ostm.open(file, mode);
		}
		void flush()
		{
			m_ptr->ostm.flush();
		}
		void close()
		{
			m_ptr->ostm.close();
		}
		bool is_open()
		{
			return m_ptr->ostm.is_open();
		}
		void set_delimiter(char delimiter)
		{
			m_ptr->delimiter = delimiter;
		}
		char get_delimiter() const
		{
			return m_ptr->delimiter;
		}
		ostruct_ptr& get_ptr()
		{
			return m_ptr;
		}
	private:
		ostruct_ptr m_ptr;
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
csv::ifstream& operator >> (csv::ifstream& istm, std::string& val)
{
	val = istm.get_delimited_str();

	return istm;
}

template<typename T>
csv::ofstream& operator << (csv::ofstream& ostm, const T& val)
{
	if(!ostm.get_ptr()->after_newline)
		ostm.get_ptr()->ostm << ostm.get_ptr()->delimiter;

	ostm.get_ptr()->ostm << val;

	ostm.get_ptr()->after_newline = false;

	return ostm;
}
template<>
csv::ofstream& operator << (csv::ofstream& ostm, const char& val)
{
	if(val==NEWLINE)
	{
		ostm.get_ptr()->ostm << std::endl;

		ostm.get_ptr()->after_newline = true;
	}
	else
		ostm.get_ptr()->ostm << val;

	return ostm;
}


#endif // MiniCSV_H