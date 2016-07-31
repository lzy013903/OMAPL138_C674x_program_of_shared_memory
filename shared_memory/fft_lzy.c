#include <stdio.h>
#include "math.h"
#include "fft_lzy.h"

short int input[sample_num],DATA[sample_num];
float fWaveR[sample_num],fWaveI[sample_num],w[sample_num];
float sin_tab[sample_num],cos_tab[sample_num];

void run_fft(float dataR[sample_num],float dataI[sample_num])
{
	int x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,xx;
	int i,j,k,b,p,L;
	float TR,TI,temp;
	//位倒序排列
	for ( i=0;i<sample_num;i++ )
	{
		x0=x1=x2=x3=x4=x5=x6=x7=x8=x9=0;

		x0=i&0x01;
		x1=(i>>1)&0x01;
		x2=(i>>2)&0x01;
		x3=(i>>3)&0x01;
		x4=(i>>4)&0x01;
		x5=(i>>5)&0x01;
		x6=(i>>6)&0x01;
		x7=(i>>7)&0x01;
		x8=(i>>8)&0x01;
		x9=(i>>9)&0x01;

		xx=x0*512 + x1*256 + x2*128 + x3*64 + x4*32 + x5*16 + x6*8 + x7*4 + x8*2 + x9 ;

		dataI[xx]=dataR[i];
	}
	for ( i=0;i<sample_num;i++ )
	{
		dataR[i]=dataI[i]; dataI[i]=0;
	}
	//计算FFT
	for ( L=1;L<=10;L++ )
	{ /* for(1) */
		b=1; i=L-1;
		while ( i>0 )
		{
			b=b*2; i--;
		} /* b= 2^(L-1) */
		for ( j=0;j<=b-1;j++ ) /* for (2) */
		{
			p=1; i=10-L;
			while ( i>0 ) /* p=pow(2,7-L)*j; */
			{
				p=p*2; i--;
			}
			p=p*j;
			for ( k=j;k<1024;k=k+2*b ) /* for (3) */
			{
				TR=dataR[k]; TI=dataI[k]; temp=dataR[k+b];
				dataR[k]=dataR[k]+dataR[k+b]*cos_tab[p]+dataI[k+b]*sin_tab[p];
				dataI[k]=dataI[k]-dataR[k+b]*sin_tab[p]+dataI[k+b]*cos_tab[p];
				dataR[k+b]=TR-dataR[k+b]*cos_tab[p]-dataI[k+b]*sin_tab[p];
				dataI[k+b]=TI+temp*sin_tab[p]-dataI[k+b]*cos_tab[p];
			} /* END for (3) */
		} /* END for (2) */
	} /* END for (1) */
	//求能量
	for ( i=0;i<sample_num/2;i++ )
	{
		w[i]=sqrt(dataR[i]*dataR[i]+dataI[i]*dataI[i]);
	}
}

//生成波形
void MakeWave()
{
	int i;
	for ( i=0;i<sample_num;i++ )
	{
		input[i]=		sin(10*PI*2*i/sample_num)	\
					+2*	sin(20*PI*2*i/sample_num)	\
					+3*	sin(30*PI*2*i/sample_num)	\
					+4*	sin(40*PI*2*i/sample_num)	;
	}
}
/*
void main()
{
	MakeWave();
	get_fft_result(input,DATA);
	printf("ok\n");
	while(1);
}
*/
void get_fft_result(short int* data,short int* result)
{
	int i;
	short int* pointer = data;
	//生成旋转因子
	for ( i=0;i<sample_num;i++ )
	{
		sin_tab[i]=sin(PI*2*i/sample_num);
		cos_tab[i]=cos(PI*2*i/sample_num);
	}
	//生成复数
	for ( i=0;i<sample_num;i++ )
	{
		fWaveR[i]=*pointer;
		pointer++;
		fWaveI[i]=0.0f;
		w[i]=0.0f;
	}
	//进行FFT计算
	run_fft(fWaveR,fWaveI);

	//保存结果
	pointer = result;
	for ( i=0;i<sample_num;i++ )
	{
		*result = 10*w[i]/512;
		result++;
	}
}
