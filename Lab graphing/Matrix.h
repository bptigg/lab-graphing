#pragma once

#include <vector>
#include <iostream>

class Matrix
{
private:
	unsigned int m_rowsize;
	unsigned int m_colsize;

	std::vector<std::vector<float>> matrix; //a vector of rows
public:
	Matrix(unsigned int row_num, unsigned int col_num, float value = 0);
	Matrix(const Matrix& rhs);
	void resize_matrix(int row_size, int col_num);

	Matrix multiply(const Matrix& lhs, const Matrix& rhs);
	std::vector<std::shared_ptr<Matrix>> qaudrants(const Matrix& mat);

	Matrix Laderman(const Matrix& lhs, const Matrix& rhs); //3x3
	Matrix Strassen(const Matrix& lhs, const Matrix& rhs); //2x2
	int find_value(int rowsize);

	Matrix operator=(const Matrix& rhs);
	Matrix operator+(const Matrix& rhs);
	Matrix operator-(const Matrix& rhs);
	Matrix operator*(const Matrix& rhs);

	Matrix operator+(const float& rhs);
	Matrix operator-(const float& rhs);
	Matrix operator*(const float& rhs);
	Matrix operator/(const float& rhs);
	
	Matrix transpose();
	Matrix inverse();

	friend std::ostream& operator<<(std::ostream& stream, const Matrix& mat);

	float& operator()(const unsigned int& row, const unsigned int& col);
	const float& operator()(const unsigned int& row, const unsigned int& col) const;
	std::vector<float>& operator()(const unsigned int& row);
	const std::vector<float>& operator()(const unsigned int& row) const;

	unsigned int get_row_size() const;
	unsigned int get_col_size() const;


};

