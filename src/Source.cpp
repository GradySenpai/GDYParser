#include<iostream>
#include"gdyparser.h"

int main()
{
	std::string input;
	std::getline(std::cin, input);
	gdy::parser::statment statment = input.c_str();
	for (int i = 0; i != statment.size(); i++)
	{
		std::cout << '[' << i << ']' <<statment[i] << std::endl;
	}
}