#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<fstream>

namespace gdy
{
	namespace parser
	{
		struct settings
		{
			bool space_seperator = false, noempty = false;
			std::vector<std::string> values;
			std::string special, quote, seperator;
		};

		class basic_statment
		{
		public:
			std::vector<std::string> words;
			basic_statment(std::string str)
			{
				std::string word;
				for (auto c : str)
				{
					if (c == ' ') 
					{
						words.push_back(word);
						word.clear();
					}
					else
					{
						word += c;
					}
				}
				words.push_back(word);
			}
			std::string inline operator[](unsigned index) { return words[index]; }
		};

		//return -1 if special and -2 if quote. else it returns values type
		int char_type(settings& setting, char c)
		{
			for (char _c : setting.special)
				if (c == _c) return -1;

			for (char _c : setting.quote)
				if (c == _c) return -2;

			for (char _c : setting.seperator)
				if (c == _c) return -3;
			if (setting.space_seperator && c == ' ') return -4;

			for (int x = 0; x != setting.values.size(); x++)
				for (int y = 0; y != setting.values[x].size(); y++)
					if (c == setting.values[x][y]) return x;
			return 0;
		}

		settings load_settings(std::string file_name)
		{
			settings setting;
			std::ifstream in;
			std::string line;
			in.open(file_name, std::ios::in);
			while (std::getline(in, line))
			{
				basic_statment stat = line;
				if (stat[0] == "special:")
					setting.special = stat[1];
				else if (stat[0] == "literal:")
					setting.quote = stat[1];
				else if (stat[0] == "seperator:")
					setting.seperator = stat[1];
				else if (stat[0] == "space_seperator")
					setting.space_seperator = true;
				else if (stat[0] == "noempty")
					setting.noempty = true;
				else
					setting.values.push_back(line);
			}
			return setting;
		}

		std::vector<std::string> parse_str(settings setting, std::string str)
		{
			std::string word;
			std::vector<std::string> ret;

			int current_type = char_type(setting, str[0]);
			for (int i = 0; i != str.size(); i++)
			{
				int type = char_type(setting, str[i]);
				if (type == -1)
				{
					if (!word.empty()) {
						ret.push_back(word);
						word.clear();
					}
					word = str[i];
					ret.push_back(word);
					word.clear();
					if (i + 1 < str.size())
						current_type = char_type(setting, str[i + 1]);
				}
				else if (type == -3 || type == -4)
				{
					if (setting.noempty && word.empty())
					{
						if (i + 1 < str.size())
							current_type = char_type(setting, str[i + 1]);
						continue;
					}
					ret.push_back(word);
					current_type = char_type(setting, str[i + 1]);
					word.clear();
				}
				else if (current_type != type) {
					ret.push_back(word);
					word.clear();
					current_type = type;
					word = str[i];
				}
				else
				{
					word += str[i];
				}
			}
			if (!word.empty())
			ret.push_back(word);
			return ret;
		}

		class statment
		{
		private:
			settings setting;
			std::vector<std::string> words;
		public:
			statment(const char* str)
			{
				setting = load_settings("standard.init");
				words = parse_str(setting, str);
			}

			std::string inline operator[](unsigned index) { return words[index]; }
			size_t size() { return words.size(); }
		};
	}
}