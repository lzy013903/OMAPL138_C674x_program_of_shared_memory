#ifndef FFT_LZY_H_
#define FFT_LZY_H_

#define PI 3.1415926
#define sample_num 1024

void MakeWave();
void run_fft(float dataR[sample_num],float dataI[sample_num]);
void get_fft_result(short int* data,short int* result);

#endif /* FFT_LZY_H_ */
