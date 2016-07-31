#ifndef SIMILAR_H_
#define SIMILAR_H_

void init_data(short int* xx,short int* yy);
void conver_data(short int* sou_1, short int* sou_2,double* x,double* y);
double similarity(double* data_1, double* data_2);

#define length 512
#define length_error 0
#define PI 3.1415926

#endif /* SIMILAR_H_ */
