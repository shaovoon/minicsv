// The MIT License (MIT)
// Minimalistic CSV Streams
// Copyright (C) 2014, by Wong Shao Voon (shaovoon@yahoo.com)
//
// http://opensource.org/licenses/MIT

#pragma once

#ifndef MiniCSV_H
	#define MiniCSV_H

#include <string>
#include <sstream>
#include <fstream>
#include <memory>

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
		ifstream()
		{
			m_ptr = std::shared_ptr<istruct>(new istruct());
		}
		ifstream(const char * file, std::ios_base::openmode mode)
		{
			m_ptr = std::shared_ptr<istruct>(new istruct());
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
		std::shared_ptr<istruct> m_ptr;

	};
	template<typename T>
	ifstream& operator >> (ifstream& istm, T& val)
	{
		std::string str = istm.get_delimited_str();
		std::istringstream is(str);
		
		is >> val;

		return istm;
	}
	template<>
	ifstream& operator >> (ifstream& istm, std::string& val)
	{
		val = istm.get_delimited_str();

		return istm;
	}

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
		ofstream()
		{
			m_ptr = std::shared_ptr<ostruct>(new ostruct());
		}
		ofstream(const char * file, std::ios_base::openmode mode)
		{
			m_ptr = std::shared_ptr<ostruct>(new ostruct());
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
		std::shared_ptr<ostruct>& get_ptr()
		{
			return m_ptr;
		}
	private:
		std::shared_ptr<ostruct> m_ptr;
	};

	template<typename T>
	ofstream& operator << (ofstream& ostm, const T& val)
	{
		if(!ostm.get_ptr()->after_newline)
			ostm.get_ptr()->ostm << ostm.get_ptr()->delimiter;

		ostm.get_ptr()->ostm << val;

		ostm.get_ptr()->after_newline = false;

		return ostm;
	}
	template<>
	ofstream& operator << (ofstream& ostm, const char& val)
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

}

#endif // MiniCSV_H