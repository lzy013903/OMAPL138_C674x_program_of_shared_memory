#include <stdio.h>
#include "protocol.h"
#include "fft_lzy.h"
#include "math.h"
#include "similar.h"
#include "model_lib.h"

#define debug_flag 			0			//��ӡָ����

int Get_time_data_from_ARM();
int Send_freq_data_to_ARM();
void get_data_setting_from_arm();
void compute_dB();

//��������ָ�����ڴ��ָ��
char* 		arm_to_dsp		= (char*)		ARM_to_DSP;			//0xC200_0000
short int* 	data_from_wav 	= (short int*)	A_to_D_data;		//0xC200_0004
char* 		dsp_to_arm		= (char*)		DSP_to_ARM;			//0xC200_1000
short int* 	data_of_fft 	= (short int*)	D_to_A_data;		//0xC200_1004
char*  		data_of_set     = (char*)  		ARM_setting;

int i;
char flag[4];						//���湲���ڴ��״̬
char* pointer = NULL;				//���������ַ��ָ��
short int* pointer_short = NULL;	//Ϊ����ʹ��void*ֻ�ܳ����²�
short int wav[1024];				//��ȡ��������ARM������
short int fft[512];					//�����һ��FFT����Ľ��
short int fft_2[512];				//����ڶ���FFT����Ľ��
double similar_data_x[512];			//�������ƶȼ���ĵ�һ��˫��������
double similar_data_y[512];			//�������ƶȼ���ĵڶ���˫��������
double similar_data_z[100];			//��ֹ�쳣
short int low_and_med = 100;		//�е�Ƶ�ֽ��
short int med_and_hig = 175;		//�и�Ƶ�ֽ��
int low_power = 0;					//��Ƶ����
int med_power = 0;					//��Ƶ����
int hig_power = 0;					//��Ƶ����
int sum = 0;						//�������
double percent[3];					//��������
double db_power;
short int db_value;

char setting_from_arm[8];

int main(void)
{
	while(1)
	{
//0	- ��ȡ��Ƶ������
		get_data_setting_from_arm();

//1.1 �ӹ����ڴ�A����ȡʱ������-------------------------------------------------------------------
		Get_time_data_from_ARM();

//1.2 �������ݵ�FFT
		printf("1st fft ...\n");
		get_fft_result(wav,fft);

//2.1 �ӹ����ڴ�A����ȡʱ������-------------------------------------------------------------------
		Get_time_data_from_ARM();

//2.2 �������ݵ�FFT
		printf("2nd fft ...\n");
		get_fft_result(wav,fft_2);

//3 �洢Ƶ��������--------------------------------------------------------------------------
		for(i=0;i<512;i++)
		{
			fft[i] = (fft[i]>>1) + (fft_2[i]>>1);
		}

//4   ������������ -----------------------------------------------------------------------------
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

//5 ���ƶȼ�� -------------------------------------------------------------------------------
	double similar_answer = 0.0;
	similar_answer = similar_answer;
	conver_data(fft,low_model,similar_data_x,similar_data_y);		//ת������Ϊdouble����
	similar_answer = similarity(similar_data_x,similar_data_y);		//���ƶȼ���
//	printf("A similarity : %f\n",similar_answer);

	conver_data(fft,normal_model,similar_data_x,similar_data_y);	//ת������Ϊdouble����
	similar_answer = similarity(similar_data_x,similar_data_y);		//���ƶȼ���
//	printf("B similarity : %f\n",similar_answer);

	conver_data(fft,high_model,similar_data_x,similar_data_y);		//ת������Ϊdouble����
	similar_answer = similarity(similar_data_x,similar_data_y);		//���ƶȼ���
//	printf("C similarity : %f\n",similar_answer);

//6 ���������źŵ�����Ҫ��ʾ��������---------------------------------------------------------------
	fft[500]=(short int)(percent[0]*100);
	fft[501]=(short int)(percent[1]*100);
	fft[502]=(short int)(percent[2]*100);
	fft[503]=(short int)(db_value);
	printf("percent[0] : %d\n",fft[500]);
	printf("percent[1] : %d\n",fft[501]);
	printf("percent[2] : %d\n",fft[502]);
	printf("db_value   : %d\n",fft[503]);
//7 - ��Ƶ�����ݷ��͵������ڴ�B����
		Send_freq_data_to_ARM();
	}
}

int Get_time_data_from_ARM()
{
//1.��鵱ǰ�����ڴ�A����״̬
check_A:
	pointer = arm_to_dsp;
	for(i=0 ; i<4 ; i++)		//���ƹ����ڴ�״̬����
	{
		flag[i] = *pointer;
		pointer++;
	}
	if(flag[0]!=Free)			//�����ڴ洦��æµ״̬
	{
		//printf("Mem A busy ... \n");
		goto check_A;
	}
	if(flag[1]!=ARM)			//��һ�β���ΪDSP��ARMδ��������
	{
		//printf("Mem A not update ... \n");
		goto check_A;
	}
//2.�޸Ĺ����ڴ�A����״̬
	pointer = arm_to_dsp;
	flag[0] = Busy;
	for(i=0 ; i<4 ; i++)
	{
		*pointer = flag[i];
		pointer++;
	}
//3.�ӹ����ڴ渴�����ݵ�DSP�Լ���RAM��
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
//4.�޸Ĺ����ڴ�A����״̬
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
//1.��鵱ǰ�����ڴ�B��״̬
	check_B:
	pointer = dsp_to_arm;
	for(i=0 ; i<4 ; i++)		//���ƹ����ڴ�B��״̬����
	{
		flag[i] = *pointer;
		pointer++;
	}
	if(flag[0]!=Free)			//�����ڴ洦��æµ״̬
	{
		//printf("Mem B busy ... \n");
		goto check_B;
	}
	if(flag[1]!=ARM)			//��һ�β���ΪDSP��ARMδ��������
	{
		//printf("Mem B not read ... \n");
		goto check_B;
	}
//2.�޸Ĺ����ڴ�B����״̬
	pointer = dsp_to_arm;
	flag[0] = Busy;
	for(i=0 ; i<4 ; i++)
	{
		*pointer = flag[i];
		pointer++;
	}
//3.д�����ݵ������ڴ�B����
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

//4.�޸Ĺ����ڴ�B����״̬
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

