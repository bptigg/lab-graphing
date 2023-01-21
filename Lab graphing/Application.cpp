#include <iostream>
#include <fstream>
#include <string>

#include "csv.h"
#include "Matrix.h"

int main()
{

	Matrix mat_1(4, 100, 2.0f);
	
	for (int i = 0; i < 4; i++)
	{
		for (int e = 0; e < 100; e++)
		{
			mat_1(i, e) = i + e;
		}
	}

	Matrix mat_2(100, 4, 4.0f);

	for (int i = 0; i < 100; i++)
	{
		for (int e = 0; e < 4; e++)
		{
			mat_2(i, e) = i * e + (i + e);
		}
	}

	std::cout << mat_1 << std::endl;
	std::cout << mat_2 << std::endl;	
	std::cout << mat_1 * mat_2 << std::endl;

	CSV::select_data();
	CSV::Load_sheet(0);
	CSV::print_sheet(0);

	//std::ifstream csv_file;
	//std::string read_string;
	//csv_file.open("Excel/Diffraction patterns.csv");
	//
	//while (!csv_file.eof())
	//{
	//	std::getline(csv_file, read_string, ',');
	//	std::cout << read_string << std::endl;
	//}
}