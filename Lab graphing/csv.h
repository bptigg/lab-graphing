#pragma once
#include "data.h"
#include <iostream>
#include <filesystem>
#include <fstream>

namespace CSV
{

	struct data
	{
		std::string directory;
		int num_sheets;
		std::vector<std::string> sheets;
		std::vector<graph> sheet_data;
	};

	int select_data();
	int Load_sheet(int index);
	void print_sheet(int index);


	bool is_float(std::string input);

}