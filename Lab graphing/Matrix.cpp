#include "Matrix.h"

#include <thread>
#include <memory>
#include <array>

Matrix::Matrix(unsigned int row_num, unsigned int col_num, float value)
	:m_rowsize(row_num), m_colsize(col_num)
{
	matrix.resize(row_num);
	for (int i = 0; i < matrix.size(); i++)
	{
		matrix[i].resize(col_num, value);
	}
}

Matrix::Matrix(const Matrix& rhs)
{
	matrix = rhs.matrix;
	m_rowsize = rhs.get_row_size();
	m_colsize = rhs.get_col_size();
}

void Matrix::resize_matrix(int row_size, int col_num)
{
	matrix.resize(row_size);
	for (int i = 0; i < matrix.size(); i++)
	{
		matrix[i].resize(col_num, 0);
	}
}

Matrix Matrix::multiply(const Matrix& lhs, const Matrix& rhs)
{

	if (lhs.get_row_size() == 2)
	{
		return Strassen(lhs, rhs);
	}
	else if (lhs.get_row_size() == 3)
	{
		return Laderman(lhs, rhs);
	}

	auto qaudrants_l = qaudrants(lhs);
	auto quadrants_r = qaudrants(rhs);

	auto AE = multiply(*qaudrants_l[0], *quadrants_r[0]);
	auto AF = multiply(*qaudrants_l[0], *quadrants_r[1]);
	auto BG = multiply(*qaudrants_l[1], *quadrants_r[2]);
	auto BH = multiply(*qaudrants_l[1], *quadrants_r[3]);
	auto CE = multiply(*qaudrants_l[2], *quadrants_r[0]);
	auto CF = multiply(*qaudrants_l[2], *quadrants_r[1]);
	auto DG = multiply(*qaudrants_l[3], *quadrants_r[2]);
	auto DH = multiply(*qaudrants_l[3], *quadrants_r[3]);

	std::vector<Matrix> minor_mat;

	auto q1 = AE + BG;
	minor_mat.push_back(q1);
	auto q2 = AF + BH;
	minor_mat.push_back(q2);
	auto q3 = CE + DG;
	minor_mat.push_back(q3);
	auto q4 = CF + DH;
	minor_mat.push_back(q4);

	Matrix return_matrix(lhs.get_row_size(), lhs.get_row_size(), 0);
	
	int active_quad = 0;
	int i2 = 0;
	int e2 = 0;
	for (int i = 0; i < lhs.get_row_size(); i++)
	{
		for (int e = 0; e < lhs.get_row_size(); e++)
		{
			return_matrix(i, e) = (minor_mat[active_quad])(i2, e2);

			e2++;
			if (e == minor_mat[active_quad].get_row_size() - 1)
			{
				active_quad++;
				e2 = 0;
			}
		}

		i2++;
		e2 = 0;
		if (i == minor_mat[active_quad].get_row_size() - 1)
		{
			active_quad++;
			i2 = 0;

		}
		else
		{
			active_quad = active_quad - 1;
		}
	}

	return return_matrix;
}

std::vector<std::shared_ptr<Matrix>> Matrix::qaudrants(const Matrix& mat)
{
	int quadrant_size = mat.get_row_size() * 0.5;

	std::vector<std::shared_ptr<Matrix>> quadrants;
	for (int i = 0; i < 4; i++)
	{
		auto mat = std::make_shared<Matrix>(quadrant_size, quadrant_size, 0);
		quadrants.push_back(mat);
	}

	int active_quadrant = 0;
	int i2 = 0;
	int e2 = 0;
	for (int i = 0; i < mat.get_row_size(); i++)
	{
		for (int e = 0; e < mat.get_row_size(); e++)
		{
			(*quadrants[active_quadrant])(i2, e2) = mat(i, e);

			e2++;
			if (e == quadrant_size - 1)
			{
				active_quadrant++;
				e2 = 0;
			}
		}

		i2++;
		e2 = 0;
		if (i == quadrant_size - 1)
		{
			active_quadrant++;
			i2 = 0;

		}
		else
		{
			active_quadrant = active_quadrant - 1;
		}
	}

	return quadrants;
}

Matrix Matrix::Laderman(const Matrix& A, const Matrix& B)
{
	Matrix return_matrix(3,3,0);
	std::array<float, 24> components;

	components[1]	= (A(0,0) + A(0,1) + A(0,2) - A(1,0) - A(1,1) - A(2,1) - A(2,2)) * B(1,1);
	components[2]	= (A(0,0) - A(1,0)) * (-B(0,1) + B(1,1));
	components[3]	= A(1,1) * (-B(0,0) + B(0,1) + B(1,0) - B(1,1) - B(1,2) - B(2,0) + B(2,2));
	components[4]	= (-A(0,0) + A(1,0) + A(1,1)) * (B(0,0) - B(0,1) + B(1,1));
	components[5]	= (A(1,0) + A(1,1)) * (-B(0,0) + B(0,1));
	components[6]	= A(0,0) * B(0,0);
	components[7]	= (-A(0,0) + A(2,0) + A(2,1)) * (B(0,0) - B(0,2) + B(1,2));
	components[8]	= (-A(0,0) + A(2,0)) * (B(0,2) - B(1,2));
	components[9]	= (A(2,0) + A(2,1)) * (-B(0,0) + B(0,2));
	components[10]	= (A(0,0) + A(0,1) + A(0,2) - A(1,1) - A(1,2) - A(2,0) - A(2,1)) * B(1,2);
	components[11]	= A(2,1) * (-B(0,0) + B(0,2) + B(1,0) - B(1,1) - B(1,2) - B(2,0) + B(2,1));
	components[12]	= (-A(0,2) + A(2,1) + A(2,2)) * (B(1,1) + B(2,0) - B(2,1));
	components[13]	= (A(0,2) - A(2,2)) * (B(1,1) - B(2,1));
	components[14]	= A(0,2) * B(2,0);
	components[15]	= (A(2,1) + A(2,2)) * (-B(2,0) + B(2,1));
	components[16]	= (-A(0,2) + A(1,1) + A(1,2)) * (B(1,2) + B(2,0) - B(2,2));
	components[17]	= (A(0,2) - A(1,2)) * (B(1,2) - B(2,2));
	components[18]	= (A(1,1) + A(1,2)) * (-B(2,0) + B(2,2));
	components[19]	= A(0,1) * B(1,0);
	components[20]	= A(1,2) * B(2,1);
	components[21]	= A(1,0) * B(0,2);
	components[22]	= A(2,0) * B(0,1);
	components[23]	= A(2,2) * B(2,2);

	return_matrix(0,0) = components[6] + components[14] + components[19];
	return_matrix(0,1) = components[1] + components[4] + components[5] + components[6] + components[12] + components[14] + components[15];
	return_matrix(0,2) = components[6] + components[7] + components[9] + components[10] + components[14] + components[16] + components[18];
	return_matrix(1,0) = components[2] + components[3] + components[4] + components[6] + components[14] + components[16] + components[17];
	return_matrix(1,1) = components[2] + components[4] + components[5] + components[6] + components[20];
	return_matrix(1,2) = components[14] + components[16] + components[17] + components[18] + components[21];
	return_matrix(2,0) = components[6] + components[7] + components[8] + components[11] + components[12] + components[13] + components[14];
	return_matrix(2,1) = components[12] + components[13] + components[14] + components[15] + components[22];
	return_matrix(2,2) = components[6] + components[7] + components[8] + components[9] + components[23];


	return return_matrix;
}

Matrix Matrix::Strassen(const Matrix& A, const Matrix& B)
{
	Matrix return_matrix(2, 2, 0);

	std::array<float, 7> components;
	components[0] = (A(0, 0) + A(1, 1)) * (B(0, 0) + B(1, 1));
	components[1] = (A(1, 0) + A(1, 1)) * B(0, 0);
	components[2] = A(0, 0) * (B(0, 1) - B(1, 1));
	components[3] = A(1, 1) * (B(1, 0) - B(0, 0));
	components[4] = (A(0, 0) + A(0, 1)) * B(1, 1);
	components[5] = (A(1, 0) - A(0, 0)) * (B(0, 0) + B(0, 1));
	components[6] = (A(0, 1) - A(1, 1)) * (B(1, 0) + B(1, 1));

	return_matrix(0, 0) = components[0] + components[3] - components[4] + components[6];
	return_matrix(0, 1) = components[2] + components[4];
	return_matrix(1, 0) = components[1] + components[3];
	return_matrix(1, 1) = components[0] - components[1] + components[2] + components[5];

	return return_matrix;
}

int Matrix::find_value(int rowsize)
{
	bool six = false;
	bool two = false;
	int m6 = rowsize % 6;
	int m2 = rowsize % 2;

	int temp6 = rowsize + 6 - m6; 
	if(((temp6/6) & ((temp6/6) - 1)) == 0)
	{
		six = true;
	}

	int temp2 = rowsize + 2 - m2;
	if ((temp2 & (temp2 - 1)) == 0)
	{
		two = true;
	}

	if (two && six)
	{
		if (temp2 > temp6)
		{
			return temp6;
		}
		else
		{
			return temp2;
		}
	}
	else if (two)
	{
		return temp2;
	}
	else if (six)
	{
		return temp6;
	}
	else
	{
		find_value(rowsize + 2);
	}
}

Matrix Matrix::operator=(const Matrix& rhs)
{
	if (&rhs == this)
	{
		return *this;
	}

	unsigned int new_rows = rhs.get_row_size();
	unsigned int new_cols = rhs.get_col_size();

	matrix.resize(new_rows);
	for (unsigned int i = 0; i < matrix.size(); i++)
	{
		matrix[i].resize(new_cols);
	}

	for (unsigned int i = 0; i < new_rows; i++)
	{
		for (unsigned int j = 0; j < new_cols; j++)
		{
			matrix[i][j] = rhs(i, j);
		}
	}

	m_rowsize = new_rows;
	m_colsize = new_cols;

	return *this;
}

Matrix Matrix::operator+(const Matrix& rhs)
{
	Matrix return_matrix(m_rowsize, m_colsize);

	for (unsigned int i = 0; i < m_rowsize; i++)
	{
		for (unsigned int j = 0; j < m_colsize; j++)
		{
			return_matrix(i, j) = this->matrix[i][j] + rhs(i, j);
		}
	}

	return return_matrix;
}

Matrix Matrix::operator-(const Matrix& rhs)
{
	Matrix return_matrix(m_rowsize, m_colsize);

	for (unsigned int i = 0; i < m_rowsize; i++)
	{
		for (unsigned int j = 0; j < m_colsize; j++)
		{
			return_matrix(i, j) = this->matrix[i][j] - rhs(i, j);
		}
	}

	return return_matrix;
}

Matrix Matrix::operator*(const Matrix& rhs)
{
	//sets up the matrix for tiling by making bot sqaure and have a even number of rows and columns 
	Matrix temp_1(0, 0, 0);
	bool row = true;

	if (m_colsize != rhs.m_rowsize)
	{
		std::cout << "Matracies don't multiply" << std::endl;
		return Matrix(0, 0, 0);
	}

	if (m_rowsize != m_colsize)
	{
		if (m_rowsize > m_colsize)
		{
			temp_1.resize_matrix(m_rowsize, m_rowsize);
			temp_1.m_rowsize = m_rowsize;
			temp_1.m_colsize = m_rowsize;
		}
		else
		{
			temp_1.resize_matrix(m_colsize, m_colsize);
			temp_1.m_rowsize = m_colsize;
			temp_1.m_colsize = m_colsize;
			row = false;
		}
	}

	Matrix temp_2(0, 0, 0);

	if (rhs.get_row_size() != rhs.get_col_size())
	{
		if (rhs.get_row_size() > rhs.get_col_size())
		{
			temp_2.resize_matrix(rhs.get_row_size(), rhs.get_row_size());
			temp_2.m_rowsize = rhs.m_rowsize;
			temp_2.m_colsize = rhs.m_rowsize;

		}
		else
		{
			temp_2.resize_matrix(rhs.get_col_size(), rhs.get_col_size());
			temp_2.m_rowsize = rhs.m_colsize;
			temp_2.m_colsize = rhs.m_colsize;
		}
	}

	int new_rowsize = 0;
	if (temp_1.m_rowsize < temp_2.m_rowsize)
	{
		new_rowsize = find_value(temp_2.m_rowsize);
	}
	else
	{
		new_rowsize = find_value(temp_1.m_rowsize);
	}

	temp_1.resize_matrix(new_rowsize, new_rowsize);
	temp_2.resize_matrix(new_rowsize, new_rowsize);
	temp_1.m_rowsize = new_rowsize;
	temp_1.m_colsize = new_rowsize;
	temp_2.m_rowsize = new_rowsize;
	temp_2.m_colsize = new_rowsize;

	for (int i = 0; i < m_rowsize; i++)
	{
		for (int e = 0; e < m_colsize; e++)
		{
			temp_1(i, e) = this->matrix[i][e];
		}
	}

	for (int i = 0; i < rhs.get_row_size(); i++)
	{
		for (int e = 0; e < rhs.get_col_size(); e++)
		{
			temp_2(i, e) = rhs(i, e);
		}
	}

	auto result = multiply(temp_1, temp_2);
	Matrix return_mat(m_rowsize, m_rowsize, 0);

	for (int i = 0; i < m_rowsize; i++)
	{
		for (int e = 0; e < m_rowsize; e++)
		{
			return_mat(i, e) = result(i, e);
		}
	}

	return return_mat;
}

Matrix Matrix::operator+(const float& rhs)
{
	Matrix return_matrix(m_rowsize, m_colsize);

	for (unsigned int i = 0; i < m_rowsize; i++)
	{
		for (unsigned int j = 0; j < m_colsize; j++)
		{
			return_matrix(i, j) = this->matrix[i][j] + rhs;
		}
	}

	return return_matrix;
}

Matrix Matrix::operator-(const float& rhs)
{
	Matrix return_matrix(m_rowsize, m_colsize);

	for (unsigned int i = 0; i < m_rowsize; i++)
	{
		for (unsigned int j = 0; j < m_colsize; j++)
		{
			return_matrix(i, j) = this->matrix[i][j] - rhs;
		}
	}

	return return_matrix;
}

Matrix Matrix::operator*(const float& rhs)
{
	Matrix return_matrix(m_rowsize, m_colsize);

	for (unsigned int i = 0; i < m_rowsize; i++)
	{
		for (unsigned int j = 0; j < m_colsize; j++)
		{
			return_matrix(i, j) = this->matrix[i][j] * rhs;
		}
	}

	return return_matrix;
}

Matrix Matrix::operator/(const float& rhs)
{
	Matrix return_matrix(m_rowsize, m_colsize);

	float divisor = 1 / rhs;

	for (unsigned int i = 0; i < m_rowsize; i++)
	{
		for (unsigned int j = 0; j < m_colsize; j++)
		{
			return_matrix(i, j) = this->matrix[i][j] * divisor;
		}
	}

	return return_matrix;
}

Matrix Matrix::transpose()
{
	Matrix return_matrix(m_rowsize, m_colsize);
	for (unsigned int i = 0; i < m_rowsize; i++)
	{
		for (unsigned int j = 0; j < m_colsize; j++)
		{
			return_matrix(i, j) = this->matrix[j][i];
		}
	}

	return return_matrix;
}

Matrix Matrix::inverse()
{
	return Matrix(0, 0, 0);
}

float& Matrix::operator()(const unsigned int& row, const unsigned int& col)
{
	return this->matrix[row][col];
}

const float& Matrix::operator()(const unsigned int& row, const unsigned int& col) const
{
	return this->matrix[row][col];
}

std::vector<float>& Matrix::operator()(const unsigned int& row)
{
	return this->matrix[row];
}

const std::vector<float>& Matrix::operator()(const unsigned int& row) const
{
	return this->matrix[row];
}

unsigned int Matrix::get_row_size() const
{
	return m_rowsize;
}

unsigned int Matrix::get_col_size() const
{
	return m_colsize;
}

std::ostream& operator<<(std::ostream& stream, const Matrix& mat)
{
	for (int i = 0; i < mat.get_row_size(); i++)
	{
		for (int j = 0; j < mat.get_col_size(); j++)
		{
			stream << mat(i, j) << " ";
		}
		stream << "\n";
	}
	return stream;
}
