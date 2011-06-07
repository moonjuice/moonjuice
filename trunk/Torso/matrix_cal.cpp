#include "stdafx.h"
#include "matrix_cal.h"
#include <math.h>

///�غc�l�A��l��0
matrixA::matrixA()
{
	arr = 0;
}

///�Ѻc�l�A�R���}�C
matrixA::~matrixA()
{
	if (arr != 0)
	{
		for (int i=0;i<row;i++)
			delete [] arr[i];
		delete [] arr;
	}
}


/**@brief �a����C�Ѽƪ��غc�l
 * @param row �C��
 * @param column ���
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

/**@brief �a����C�ѼƥH�ΰ}�C���e���غc�l
 * @param row �C��
 * @param column ���
 * @param array �}�C���e
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

/**@brief �a����C�ѼƥH�ΰ}�C�������غc�l
 *
 * �}�C���e������ constant
 * @param row �C��
 * @param column ���
 * @param constant �}�C����
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

/**@brief �a���}�C�Ѽƪ��غc�l
 * @param matrix �n�ƻs���}�C
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


/**@brief ��l�ư}�C
 * @param row �C��
 * @param column ���
 * @return Initial(row, column, double(0))
 */
bool matrixA::Initial(int row, int column)
{
	return Initial(row, column, double(0));
}

/**@brief �Q�ζǤJ���}�C�Ӫ�l�ư}�C
 * @param row �C��
 * @param column ���
 * @param array �n�ƻs���}�C
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

/**@brief �Q�ΰ}�C�����Ӫ�l�ư}�C
 * @param row �C��
 * @param column ���
 * @param constant �}�C����
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

/**@brief �B��l�h��(�ƻs)
 * @param matrix �n�ƻs���}�C
 * @return matrixA& �ƻs�����}�C
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

/**@brief �B��l�h��(A=A+B)
 * @param matrix �n�B�⪺�}�CB
 * @return matrixA& ���G�}�CA
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

/**@brief �B��l�h��(A=A-B)
 * @param matrix �n�B�⪺�}�CB
 * @return matrixA& ���G�}�CA
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

/**@brief �B��l�h��(C=A+B)
 * @param matrix �n�B�⪺�}�CB
 * @return matrixA& ���G�}�CC
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

/**@brief �B��l�h��(C=A-B)
 * @param matrix �n�B�⪺�}�CB
 * @return matrixA& ���G�}�CC
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

/**@brief �B��l�h��(C=A*B)
 * @param matrix �n�B�⪺�}�CB
 * @return matrixA& ���G�}�CC
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

/**@brief �B��l�h��(C=(multiplier)*A)
 * @param multiplier �Ʀr�Ѽ�
 * @return matrixA ���G�}�CC
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

/**@brief �B��l�h��(C=(1/divisor)*A)
 * @param divisor �Ʀr�Ѽ�
 * @return matrixA ���G�}�CC
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

/**@brief �ˬd�}�C�O�_�۵�
 * @param matrix �n�ˬd���}�C
 * @retval true �۵�
 * @retval false ���۵�
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

/**@brief �ˬd�}�C�O�_�۵�
 * @param matrix �n�ˬd���}�C
 * @retval true ���۵�
 * @retval false �۵�
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

/**@brief �D�o�f�x�}
 * @return matrixA �f�x�}
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

/**@brief ���o�����ϯx�}
 * @return matrixA �����ϯx�}
 */
matrixA matrixA::PseudoInverse()
{
	if (arr != 0)
		return (this->Transpose() * (*this)).Inverse() * this->Transpose();
	else
		return *this;
}

/**@brief ���o��m�x�}
 * @return matrixA ��m�x�}
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
