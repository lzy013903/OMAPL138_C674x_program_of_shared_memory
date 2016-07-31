#include <stdio.h>
#include <math.h>
#include "similar.h"


double data_x[512];				//数据1
double data_y[512];				//数据2

/*
void main()
{
	double answer=0.0;
	printf("\n");

	init_data(source_1,source_2);
	conver_data(low_model,normal_model,data_x,data_y);
	answer = similarity(data_x,data_y);

	printf("answer : %f\n",answer);
	printf("\n");
	while(1);
}
*/

void init_data(short int* xx,short int* yy)
{
	int i = 0;
	for(i=0;i<length;i++)
	{
		(*xx) = i;
		(*yy) = i;
		xx++;
		yy++;
	}
}

void conver_data(short int* sou_1, short int* sou_2,double* x,double* y)
{
	int i=0;
	for(i=0;i<length;i++)
	{
		*x = (double)(*sou_1);
		*y = (double)(*sou_2);
		x++;
		y++;
		sou_1++;
		sou_2++;
	}

}

double similarity(double* data_1, double* data_2)
{
	int i=0;
	double aver_data_1=0.0;
	double aver_data_2=0.0;

	double* p_1 = data_1;
	double* p_2 = data_2;

	double denominator_1 = 0.0;
	double denominator_2 = 0.0;
	double similar=0.0;
//求取均值
	for(i=0;i<length-length_error;i++)
	{
		aver_data_1 += *data_1;
		aver_data_2 += *data_2;
		data_1++;
		data_2++;
	}
	aver_data_1 /=length;
	aver_data_2 /=length;
//求取偏差
	data_1 = p_1;
	data_2 = p_2;

	for(i=0;i<length-length_error;i++)
	{
		*data_1 = *data_1 - aver_data_1;
		*data_2 = *data_2 - aver_data_2;

		data_1++;
		data_2++;
	}
//求取分子
	double molecule = 0.0;
	data_1 = p_1;
	data_2 = p_2;

	for(i=0;i<length-length_error;i++)
	{
		molecule += (*data_1) * (*data_2);
		data_1++;
		data_2++;
	}
	//printf("molecule = %6f\n",molecule);
//求取分母左值
	data_1 = p_1;
	for(i=0;i<length-length_error;i++)
	{
		denominator_1 += (*data_1)*(*data_1);
		data_1++;
	}
	denominator_1 = sqrt(denominator_1);
	//printf("denominator_1 = %6f\n",denominator_1);

//求取分母右值
	data_2 = p_2;

	for(i=0;i<length-length_error;i++)
	{
		denominator_2 += (*data_2)*(*data_2);
		data_2++;
	}
	denominator_2 = sqrt(denominator_2);
	//printf("denominator_2 = %6f\n",denominator_2);

//求取相似度
	similar = molecule/(denominator_1*denominator_2);
	//printf("similar = %6f\n",similar);
	return similar;
}

