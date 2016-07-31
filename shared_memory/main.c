#include <stdio.h>
#include "protocol.h"
#include "fft_lzy.h"
#include "math.h"
#include "similar.h"
#include "model_lib.h"

#define debug_flag 			0			//打印指针标记

int Get_time_data_from_ARM();
int Send_freq_data_to_ARM();
void get_data_setting_from_arm();
void compute_dB();

//申明各类指向共享内存的指针
char* 		arm_to_dsp		= (char*)		ARM_to_DSP;			//0xC200_0000
short int* 	data_from_wav 	= (short int*)	A_to_D_data;		//0xC200_0004
char* 		dsp_to_arm		= (char*)		DSP_to_ARM;			//0xC200_1000
short int* 	data_of_fft 	= (short int*)	D_to_A_data;		//0xC200_1004
char*  		data_of_set     = (char*)  		ARM_setting;

int i;
char flag[4];						//保存共享内存的状态
char* pointer = NULL;				//操作物理地址的指针
short int* pointer_short = NULL;	//为避免使用void*只能出此下策
short int wav[1024];				//获取的来自于ARM的数据
short int fft[512];					//保存第一次FFT计算的结果
short int fft_2[512];				//保存第二次FFT计算的结果
double similar_data_x[512];			//用于相似度计算的第一个双精度数据
double similar_data_y[512];			//用于相似度计算的第二个双精度数据
double similar_data_z[100];			//防止异常
short int low_and_med = 100;		//中低频分界点
short int med_and_hig = 175;		//中高频分界点
int low_power = 0;					//低频能量
int med_power = 0;					//中频能量
int hig_power = 0;					//高频能量
int sum = 0;						//能量求和
double percent[3];					//能量比例
double db_power;
short int db_value;

char setting_from_arm[8];

int main(void)
{
	while(1)
	{
//0	- 获取分频点数据
		get_data_setting_from_arm();

//1.1 从共享内存A区获取时域数据-------------------------------------------------------------------
		Get_time_data_from_ARM();

//1.2 计算数据的FFT
		printf("1st fft ...\n");
		get_fft_result(wav,fft);

//2.1 从共享内存A区获取时域数据-------------------------------------------------------------------
		Get_time_data_from_ARM();

//2.2 计算数据的FFT
		printf("2nd fft ...\n");
		get_fft_result(wav,fft_2);

//3 存储频域波形数据--------------------------------------------------------------------------
		for(i=0;i<512;i++)
		{
			fft[i] = (fft[i]>>1) + (fft_2[i]>>1);
		}

//4   能量比例计算 -----------------------------------------------------------------------------
	low_power = 0;
	med_power = 0;
	hig_power = 0;

	for(i=40 ; i<low_and_med ; i++)
	{
		low_power += fft[i];
	}
	printf("low_power = %d \n" , low_power);

	for(i=low_and_med ; i<med_and_hig ; i++)
	{
		med_power += fft[i];
	}
	printf("med_power = %d \n" , med_power);

	for(i=med_and_hig ; i<512 ; i++)
	{
		hig_power += fft[i];
	}
	printf("hig_power = %d \n" , hig_power);

	sum = low_power+med_power+hig_power;
	printf("sum = %d \n" , sum);

	percent[0] = (double)low_power / (double)sum;
	percent[1] = (double)med_power / (double)sum;
	percent[2] = (double)hig_power / (double)sum;

	compute_dB();
	printf("db_power = %d \n" , db_power);
	db_value = (short int)db_power;

//5 相似度检测 -------------------------------------------------------------------------------
	double similar_answer = 0.0;
	similar_answer = similar_answer;
	conver_data(fft,low_model,similar_data_x,similar_data_y);		//转换数据为double类型
	similar_answer = similarity(similar_data_x,similar_data_y);		//相似度计算
//	printf("A similarity : %f\n",similar_answer);

	conver_data(fft,normal_model,similar_data_x,similar_data_y);	//转换数据为double类型
	similar_answer = similarity(similar_data_x,similar_data_y);		//相似度计算
//	printf("B similarity : %f\n",similar_answer);

	conver_data(fft,high_model,similar_data_x,similar_data_y);		//转换数据为double类型
	similar_answer = similarity(similar_data_x,similar_data_y);		//相似度计算
//	printf("C similarity : %f\n",similar_answer);

//6 保存能量信号到不需要显示的数据中---------------------------------------------------------------
	fft[500]=(short int)(percent[0]*100);
	fft[501]=(short int)(percent[1]*100);
	fft[502]=(short int)(percent[2]*100);
	fft[503]=(short int)(db_value);
	printf("percent[0] : %d\n",fft[500]);
	printf("percent[1] : %d\n",fft[501]);
	printf("percent[2] : %d\n",fft[502]);
	printf("db_value   : %d\n",fft[503]);
//7 - 将频域数据发送到共享内存B区中
		Send_freq_data_to_ARM();
	}
}

int Get_time_data_from_ARM()
{
//1.检查当前共享内存A区的状态
check_A:
	pointer = arm_to_dsp;
	for(i=0 ; i<4 ; i++)		//复制共享内存状态数据
	{
		flag[i] = *pointer;
		pointer++;
	}
	if(flag[0]!=Free)			//共享内存处于忙碌状态
	{
		//printf("Mem A busy ... \n");
		goto check_A;
	}
	if(flag[1]!=ARM)			//上一次操作为DSP，ARM未更新数据
	{
		//printf("Mem A not update ... \n");
		goto check_A;
	}
//2.修改共享内存A区的状态
	pointer = arm_to_dsp;
	flag[0] = Busy;
	for(i=0 ; i<4 ; i++)
	{
		*pointer = flag[i];
		pointer++;
	}
//3.从共享内存复制数据到DSP自己的RAM中
	#if debug_flag
		printf("debug : data_from_wav -> %p \n", data_from_wav);
	#endif
	pointer_short = data_from_wav;
	for(i=0;i<1024;i++)
	{
		wav[i]=(*pointer_short)>>2;
		pointer_short++;
	}
//	printf("Copy over ...\n");
//4.修改共享内存A区的状态
	pointer = arm_to_dsp;
	flag[0] = Free;
	flag[1] = DSP;
	for(i=0 ; i<4 ; i++)
	{
		*pointer = flag[i];
		pointer++;
	}
	return 0;
}

int Send_freq_data_to_ARM()
{
//1.检查当前共享内存B区状态
	check_B:
	pointer = dsp_to_arm;
	for(i=0 ; i<4 ; i++)		//复制共享内存B区状态数据
	{
		flag[i] = *pointer;
		pointer++;
	}
	if(flag[0]!=Free)			//共享内存处于忙碌状态
	{
		//printf("Mem B busy ... \n");
		goto check_B;
	}
	if(flag[1]!=ARM)			//上一次操作为DSP，ARM未更新数据
	{
		//printf("Mem B not read ... \n");
		goto check_B;
	}
//2.修改共享内存B区的状态
	pointer = dsp_to_arm;
	flag[0] = Busy;
	for(i=0 ; i<4 ; i++)
	{
		*pointer = flag[i];
		pointer++;
	}
//3.写入数据到共享内存B区中
	pointer_short = data_of_fft;
	fft[0]=0;
	for(i=0;i<20;i++)
	{
		*pointer_short = (fft[i]>>4);
		pointer_short++;
	}
	for(i=20;i<40;i++)
	{
		*pointer_short = (fft[i]>>3);
		pointer_short++;
	}
	for(i=40;i<512;i++)
	{
		*pointer_short = fft[i];
		pointer_short++;
	}

//4.修改共享内存B区的状态
	pointer = dsp_to_arm;
	flag[0] =Free;
	flag[1] =DSP;
	for(i=0 ; i<4 ; i++)
	{
		*pointer = flag[i];
		pointer++;
	}

	return 0;
}


void get_data_setting_from_arm()
{
	data_of_set = (char*) ARM_setting;
	for(i=0;i<8;i++)
	{
		setting_from_arm[i]=*data_of_set;
		data_of_set++;
//		printf("%p",setting_from_arm[i]);
	}
	printf("\n",setting_from_arm[i]);
	low_and_med = 	(1000*setting_from_arm[0])\
					+(100*setting_from_arm[1])\
					 +(10*setting_from_arm[2])\
					  +(setting_from_arm[3]);
	low_and_med /=8;

	med_and_hig = 	(1000*setting_from_arm[4])\
					+(100*setting_from_arm[5])\
					 +(10*setting_from_arm[6])\
					  +(setting_from_arm[7]);
	med_and_hig /=8;

	printf("low and med = %d \n",low_and_med);
	printf("med_and_hig = %d \n",med_and_hig);
}

void compute_dB()
{
	if(sum<=10000)
	{
		db_power = 60 * ((double)sum/10000);
	}
	else if((sum>10000)&&(sum<20000))
	{
		db_power = 70 + 10* ((double)(sum-10000)/10000);
	}
	else if((sum>20000)&&(sum<50000))
	{
		db_power = 80 + 10* ((double)(sum-30000)/30000);
	}
	else if((sum>50000)&&(sum<100000))
	{
		db_power = 90 + 10* ((double)(sum-50000)/30000);
	}
	else if((sum>100000)&&(sum<200000))
	{
		db_power = 100 + 10* ((double)(sum-100000)/100000);
	}
	else if((sum>200000)&&(sum<400000))
	{
		db_power = 110 + 10* ((double)(sum-200000)/200000);
	}
	else
	{
		db_power = 120 + 80* ((double)(sum-400000)/400000);
	}

}

