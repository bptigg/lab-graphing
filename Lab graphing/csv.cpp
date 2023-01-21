#include "csv.h"

static CSV::data s_data;

int CSV::select_data()
{
	std::string folder_name;
	std::cout << "Folder to read from: ";
	std::cin >> folder_name;

	s_data.directory = "Excel/" + folder_name;

	if (std::filesystem::is_directory(s_data.directory) == false)
	{
		std::cout << "Directory doesn't exist" << std::endl;
		return -1;
	}

	int number = 1;
	for (const auto& entry : std::filesystem::directory_iterator(s_data.directory))
	{
		std::string path = entry.path().string();
		char* c_path = new char[path.length() + 1];
		strcpy_s(c_path, path.length() + 1,path.c_str());
		
		char* new_path = (char*)malloc(1);
		if (new_path == NULL)
		{
			std::cout << "Memory error" << std::endl;
			return -1;
		}
		new_path[0] = '\0';
		
		int index = 0;
		char* previous_chr = nullptr;
		bool append = false;
		size_t size = 0;
		for (int i = 0; i < path.length(); i++)
		{
			if (c_path[i] == '\\')
			{
				append = true;
				size = (path.length() + 1 - i) * sizeof(char);
				if (size > 0)
				{
					char* tmp = (char*)realloc(new_path, size);

					if (tmp == nullptr)
					{
						std::cout << "Memory error" << std::endl;
						return -1;
					}
					else
					{
						new_path = tmp;
						new_path[size - 1] = '\0';
					}
				}

				for (int i = 0; i < size-1; i++)
				{
					new_path[i] = ' ';
				}
				continue;
			}
			if (append == true)
			{
				new_path[index] = c_path[i];
				index++;
			}
			
		}

		path = "";
		size_t char_size = strlen(new_path);
		for (size_t i = 0; i < char_size-1 ; i++)
		{
			path = path + new_path[i];
		}

		bool b_option = 0;
		std::string option;
		std::cout << number << ") " << path << " : Load yes(1) or no(0): ";
		std::cin >> option;
		b_option = std::stoi(option);
		s_data.sheets.push_back(path);
		graph sheet;
		s_data.sheet_data.push_back(sheet);

		delete[] c_path;
		free(new_path);
	}
	return 0;
}

int CSV::Load_sheet(int index)
{
	std::string filepath = s_data.directory + "/" + s_data.sheets[index];
	if (std::filesystem::exists(filepath) == false)
	{
		std::cout << "Sheet doesn't exsit" << std::endl;
		return -1;
	}

	std::ifstream sheet;
	std::string read_string;
	sheet.open(filepath);
	int columns = 0;
	int column = 0;
	int positions[4];
	bool initial_row = true;

	bool x = false;
	bool y = false;
	bool x_u = false;
	bool y_u = false;

	while (initial_row)
	{
		column++;
		std::getline(sheet, read_string, ',');

		char* read_chr = new char[read_string.length() + 1];
		strcpy_s(read_chr, read_string.length() + 1, read_string.c_str());

		char sub_string_x[] = "X";
		if (strstr(read_chr, sub_string_x) && !x)
		{
			positions[0] = column;
			x = true;
		}

		char sub_string_y[] = "Y";
		if (strstr(read_chr, sub_string_y) && !y)
		{
			positions[1] = column;
			y = true;
		}


		char sub_string_x_u[] = "X_U";
		if (strstr(read_chr, sub_string_x_u) && !x_u)
		{
			positions[2] = column;
			x_u = true;
		}


		char sub_string_y_u[] = "Y_U";
		if (strstr(read_chr, sub_string_y_u) && !y_u)
		{
			positions[3] = column;
			y_u = true;
		}

		if (strchr(read_chr, '\n') && columns == 0)
		{
			columns = column;
			column = 0;
			if (x && y && x_u && y_u)
			{
				initial_row = false;
			}
			else
			{
				std::cout << "Incorrect file layout" << std::endl;
				return -1;
			}
			continue;
		}
	}

	sheet.clear();
	sheet.seekg(0);
	column = 0;

	while (!sheet.eof())
	{
		column++;
		if (column == columns)
		{
			std::getline(sheet, read_string, '\n');
		}
		else
		{
			std::getline(sheet, read_string, ',');
		}

		if (read_string == "")
		{
			continue;
		}

		if (column == positions[0])
		{
			if (is_float(read_string))
			{
				s_data.sheet_data[index].data.x.push_back(std::stof(read_string));
			}
		}
		else if(column == positions[1])
		{
			if (is_float(read_string))
			{
				s_data.sheet_data[index].data.y.push_back(std::stof(read_string));
			}
		}
		else if (column == positions[2])
		{
			if (is_float(read_string))
			{
				s_data.sheet_data[index].data.x_u.push_back(std::stof(read_string));
			}
		}
		else if (column == positions[3])
		{
			if (is_float(read_string))
			{
				s_data.sheet_data[index].data.y_u.push_back(std::stof(read_string));
			}
		}

		if (column == columns)
		{
			column = 0;
		}
	}

	
}

bool CSV::is_float(std::string input)
{
	bool dot = false;
	bool is_number = true;
	bool negative = false;

	char* number = new char[input.length() + 1];
	strcpy_s(number, input.length() + 1, input.c_str());

	for (int i = 0; i < input.length(); i++)
	{
		if ((int)number[i] > 0)
		{
			if (isdigit(number[i]))
			{
				continue;
			}
		}
		if (number[i] == '.' && dot == false)
		{
			dot = true;
			continue;
		}
		if (number[i] == '-' && negative == false)
		{
			negative = true;
			continue;
		}
		is_number = false;
		break;
	}
	return is_number;
}

void CSV::print_sheet(int index)
{
	std::cout << "\tX\t" << "|\t" << " Y\t" << "|\t" << " X_u\t" << "|\t" << "Y\n";
	for (int i = 0; i < s_data.sheet_data[index].data.x.size(); i++)
	{
		std::cout << "\t" <<s_data.sheet_data[index].data.x[i] << "\t|";
		std::cout << "\t" <<s_data.sheet_data[index].data.y[i] << "\t|";
		std::cout << "\t" <<s_data.sheet_data[index].data.x_u[i] << "\t|";
		std::cout << "\t" <<s_data.sheet_data[index].data.y_u[i] << "\n";
	}
}