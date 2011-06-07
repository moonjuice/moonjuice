#include "stdafx.h"
#include "matrix_cal.h"
#include <math.h>

///篶﹍0
matrixA::matrixA()
{
	arr = 0;
}

///秆篶埃皚
matrixA::~matrixA()
{
	if (arr != 0)
	{
		for (int i=0;i<row;i++)
			delete [] arr[i];
		delete [] arr;
	}
}


/**@brief 盿Τ︽把计篶
 * @param row 计
 * @param column ︽计
 */
matrixA::matrixA(int row, int column)
{
	if (row > 0  &&  column > 0)
	{
		this->row = row;
		this->column = column;

		arr = new double * [row];
		for (int i=0;i<row;i++)
			arr[i] = new double [column];
	}
	else
		arr = 0;
}

/**@brief 盿Τ︽把计の皚ず甧篶
 * @param row 计
 * @param column ︽计
 * @param array 皚ず甧
 */
matrixA::matrixA(int row, int column, double **array)
{
	if (row > 0  &&  column > 0)
	{
		this->row = row;
		this->column = column;

		arr = new double * [row];
		for (int i=0;i<row;i++)
			arr[i] = new double [column];

		for (int i=0;i<row;i++)
			for (int j=0;j<column;j++)
				arr[i][j] = array[i][j];
	}
	else
		arr = 0;
}

/**@brief 盿Τ︽把计の皚じ篶
 *
 * 皚ず甧场 constant
 * @param row 计
 * @param column ︽计
 * @param constant 皚じ
 */
matrixA::matrixA(int row, int column, double constant)
{
	if (row > 0  &&  column > 0)
	{
		this->row = row;
		this->column = column;

		arr = new double * [row];
		for (int i=0;i<row;i++)
			arr[i] = new double [column];

		for (int i=0;i<row;i++)
			for (int j=0;j<column;j++)
				arr[i][j] = constant;
	}
	else
		arr = 0;
}

/**@brief 盿Τ皚把计篶
 * @param matrix 璶狡籹皚
 */
matrixA::matrixA(matrixA& matrix)
{
	row = matrix.row;
	column = matrix.column;

	arr = new double * [row];
	for (int i=0;i<row;i++)
		arr[i] = new double [column];

	for (int i=0;i<row;i++)
		for (int j=0;j<column;j++)
			arr[i][j] = matrix.arr[i][j];
}


/**@brief ﹍て皚
 * @param row 计
 * @param column ︽计
 * @return Initial(row, column, double(0))
 */
bool matrixA::Initial(int row, int column)
{
	return Initial(row, column, double(0));
}

/**@brief ノ肚皚ㄓ﹍て皚
 * @param row 计
 * @param column ︽计
 * @param array 璶狡籹皚
 * @retval true row and column >0
 * @retval false row<=0 or column<=0
 */
bool matrixA::Initial(int row, int column, double **array)
{
	if (row > 0  &&  column > 0)
	{
		this->row = row;
		this->column = column;

		arr = new double * [row];
		for (int i=0;i<row;i++)
			arr[i] = new double [column];

		for (int i=0;i<row;i++)
			for (int j=0;j<column;j++)
				arr[i][j] = array[i][j];

		return true;
	}
	else
		return false;
}

/**@brief ノ皚じㄓ﹍て皚
 * @param row 计
 * @param column ︽计
 * @param constant 皚じ
 * @retval true row and column >0
 * @retval false row<=0 or column<=0
 */
bool matrixA::Initial(int row, int column, double constant)
{
	if (row > 0  &&  column > 0)
	{
		this->row = row;
		this->column = column;

		arr = new double * [row];
		for (int i=0;i<row;i++)
			arr[i] = new double [column];

		for (int i=0;i<row;i++)
			for (int j=0;j<column;j++)
				arr[i][j] = constant;

		return true;
	}
	else
		return false;
}

/**@brief 笲衡更(狡籹)
 * @param matrix 璶狡籹皚
 * @return matrixA& 狡籹Ч皚
 */
matrixA& matrixA::operator=(const matrixA& matrix)
{
	if (arr != 0  &&  matrix.arr != 0)
		if (row == matrix.row  &&  column == matrix.column)
			for (int i=0;i<row;i++)
				for (int j=0;j<column;j++)
					arr[i][j] = matrix.arr[i][j];

	return *this;
}

/**@brief 笲衡更(A=A+B)
 * @param matrix 璶笲衡皚B
 * @return matrixA& 挡狦皚A
 */
matrixA& matrixA::operator+=(const matrixA& matrix)
{
	if (arr != 0  &&  matrix.arr != 0)
		if (row == matrix.row  &&  column == matrix.column)
			for (int i=0;i<row;i++)
				for (int j=0;j<column;j++)
					arr[i][j] += matrix.arr[i][j];

	return *this;
}

/**@brief 笲衡更(A=A-B)
 * @param matrix 璶笲衡皚B
 * @return matrixA& 挡狦皚A
 */
matrixA& matrixA::operator-=(const matrixA& matrix)
{
	if (arr != 0  &&  matrix.arr != 0)
		if (row == matrix.row  &&  column == matrix.column)
			for (int i=0;i<row;i++)
				for (int j=0;j<column;j++)
					arr[i][j] -= matrix.arr[i][j];

	return *this;
}

/**@brief 笲衡更(C=A+B)
 * @param matrix 璶笲衡皚B
 * @return matrixA& 挡狦皚C
 */
matrixA matrixA::operator+(const matrixA& matrix)
{
	if (arr != 0  &&  matrix.arr != 0)
	{
		if (row == matrix.row  &&  column == matrix.column)
		{
			matrixA temp(row, column);

			for (int i=0;i<row;i++)
				for (int j=0;j<column;j++)
					temp.arr[i][j] = arr[i][j] + matrix.arr[i][j];

			return temp;
		}
		else
			return *this;
	}
	else
		return *this;
}

/**@brief 笲衡更(C=A-B)
 * @param matrix 璶笲衡皚B
 * @return matrixA& 挡狦皚C
 */
matrixA matrixA::operator-(const matrixA& matrix)
{
	if (arr != 0  &&  matrix.arr != 0)
	{
		if (row == matrix.row  &&  column == matrix.column)
		{
			matrixA temp(row, column);

			for (int i=0;i<row;i++)
				for (int j=0;j<column;j++)
					temp.arr[i][j] = arr[i][j] - matrix.arr[i][j];

			return temp;
		}
		else
			return *this;
	}
	else
		return *this;
}

/**@brief 笲衡更(C=A*B)
 * @param matrix 璶笲衡皚B
 * @return matrixA& 挡狦皚C
 */
matrixA matrixA::operator*(const matrixA& matrix)
{
    if (arr != 0  &&  matrix.arr != 0)
	{
		if (column == matrix.row)
		{
			matrixA temp(row, matrix.column, double(0));

			for (int i=0;i<row;i++)
				for (int j=0;j<matrix.column;j++)
					for (int k=0;k<column;k++)
						temp.arr[i][j] += arr[i][k] * matrix.arr[k][j];

			return temp;
		}
		return *this;
	}
	else
		return *this;
}

/**@brief 笲衡更(C=(multiplier)*A)
 * @param multiplier 计把计
 * @return matrixA 挡狦皚C
 */
matrixA matrixA::operator*(const double& multiplier)
{
	if (arr != 0)
	{
		matrixA temp(row, column);

		for (int i=0;i<row;i++)
			for (int j=0;j<column;j++)
				temp.arr[i][j] = arr[i][j] * multiplier;

		return temp;
	}
	else
		return *this;
}

/**@brief 笲衡更(C=(1/divisor)*A)
 * @param divisor 计把计
 * @return matrixA 挡狦皚C
 */
matrixA matrixA::operator/(const double& divisor)
{
	if (arr != 0)
	{
		matrixA temp(row, column);

		for (int i=0;i<row;i++)
			for (int j=0;j<column;j++)
				temp.arr[i][j] = arr[i][j] / divisor;

		return temp;
	}
	else
		return *this;
}

/**@brief 浪琩皚琌单
 * @param matrix 璶浪琩皚
 * @retval true 单
 * @retval false ぃ单
 */
bool matrixA::operator==(const matrixA& matrix)
{
	if (arr != 0  &&  matrix.arr != 0)
	{
		if (row == matrix.row  &&  column == matrix.column)
		{
			for (int i=0;i<row;i++)
				for (int j=0;j<column;j++)
					if (arr[i][j] != matrix.arr[i][j])
						return false;

			return true;
		}
		else
			return false;
	}
	else
		return false;
}

/**@brief 浪琩皚琌单
 * @param matrix 璶浪琩皚
 * @retval true ぃ单
 * @retval false 单
 */
bool matrixA::operator!=(const matrixA& matrix)
{
	if (arr != 0  &&  matrix.arr != 0)
	{
		if (row == matrix.row  &&  column == matrix.column)
		{
			for (int i=0;i<row;i++)
				for (int j=0;j<column;j++)
					if (arr[i][j] != matrix.arr[i][j])
						return true;

			return false;
		}
		else
			return true;
	}
	else
		return true;
}

/**@brief ―眔癴痻皚
 * @return matrixA 癴痻皚
 */
matrixA matrixA::Inverse()
{
	if (arr != 0  &&  row == column)
	{
		matrixA temp(row, 2 * column);

		for (int i=0;i<row;i++)
		{
			for (int j=0;j<column;j++)
				temp.arr[i][j] = arr[i][j];

			for (int j=column;j<2*column;j++)
			{
				if (j-column == i)
					temp.arr[i][j] = 1;
				else
					temp.arr[i][j] = 0;
			}
		}

		double divisor, multiplier;
		for (int i=0;i<row;i++)
		{
			divisor = temp.arr[i][i];

			temp.arr[i][i] = 1;
			for (int j=i+1;j<2*column;j++)
				temp.arr[i][j] /= divisor;

			for (int m=0;m<row;m++)
				if (m != i)
				{
					multiplier = - temp.arr[m][i];
					for (int n=i;n<2*column;n++)
						temp.arr[m][n] += multiplier * temp.arr[i][n];
				}
		}

		matrixA sol(row, column);

		for (int i=0;i<row;i++)
			for (int j=0;j<column;j++)
				sol.arr[i][j] = temp.arr[i][j+column];

		return sol;
	}
	else
		return *this;
}

/**@brief 眔店览は痻皚
 * @return matrixA 店览は痻皚
 */
matrixA matrixA::PseudoInverse()
{
	if (arr != 0)
		return (this->Transpose() * (*this)).Inverse() * this->Transpose();
	else
		return *this;
}

/**@brief 眔锣竚痻皚
 * @return matrixA 锣竚痻皚
 */
matrixA matrixA::Transpose()
{
	if (arr != 0)
	{
		matrixA temp(column, row);

		for (int i=0;i<column;i++)
			for (int j=0;j<row;j++)
				temp.arr[i][j] = arr[j][i];

		return temp;
	}
	else
		return *this;
}
