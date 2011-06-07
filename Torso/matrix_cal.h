/**
 * @file matrix_cal.h 
 * @brief 包含 matrixA 類別
 * @author 謝秉恆
 * @date 2000/09
 *
 * 編號:math01
 */
#ifndef __matrix_cal_H__
#define __matrix_cal_H__


///矩陣運算類別
class matrixA
{
public:
	int row;     ///< matrix寬度
	int column;    ///< matrix高度
	double **arr;  ///< 紀錄matrix的二維陣列
	bool check;    ///< 測試運算可否成立

	matrixA();
	~matrixA();
	matrixA(int row, int column);
    matrixA(int row, int column, double **array);
	matrixA(int row, int column, double constant);
	matrixA(matrixA& matrix);

	bool Initial(int row, int column);
	bool Initial(int row, int column, double **array);
	bool Initial(int row, int column, double constant);

    matrixA& operator=(const matrixA& matrix);
	matrixA& operator+=(const matrixA& matrix);
	matrixA& operator-=(const matrixA& matrix);
	matrixA operator+(const matrixA& matrix);
	matrixA operator-(const matrixA& matrix);
	matrixA operator*(const matrixA& matrix);
    matrixA operator*(const double& multiplier);
	matrixA operator/(const double& divisor);
	bool operator==(const matrixA& matrix);
	bool operator!=(const matrixA& matrix);

	matrixA Inverse();
	matrixA PseudoInverse();
	matrixA Transpose();
};

#endif