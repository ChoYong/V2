#pragma once
#include <iostream>

using namespace std;

class Matrix {
private:
	int dy = 0;
	int dx = 0;
	int **array;
	void alloc(int cy, int cx);
public:
	int get_dy();
	int get_dx();
	int** get_array();
	Matrix();
	Matrix(int cy, int cx);
	Matrix(const Matrix *obj);
	Matrix(int *arr, int col, int row);
	~Matrix();
	Matrix *clip(int top, int left, int bottom, int right);
	void paste(const Matrix *obj, int top, int left);
	Matrix *add(const Matrix *obj);
	int sum();
	void mulc(int coef);
	Matrix *int2bool();
	Matrix *int2negative();
	Matrix *odd2even();
	bool anyGreaterThan(int val);
	void clearNegative();
	void print();
	Matrix& operator=(const Matrix& obj);
};
