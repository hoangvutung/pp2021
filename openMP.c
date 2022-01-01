#include<stdio.h>
#include<omp.h>
#include<stdlib.h>
#include<time.h>
#define nthread 4
#define size 20
#define test 1

void Init(int* a, int* b,int* c_test){
	int i, j;
	FILE *fp;
	if(test){
		fp=fopen("a.txt","r");
		for(i=0;i<size;i++){
			for(j=0;j<size;j++){
				fscanf(fp,"%d",&a[i*size+j]);
			}
		}
		fclose(fp);
		fp=fopen("b.txt","r");
		for(i=0;i<size;i++){
			for(j=0;j<size;j++){
				fscanf(fp,"%d",&b[i*size+j]);
			}
		}
		fclose(fp);
		fp=fopen("c.txt","r");
		for(i=0;i<size;i++){
			for(j=0;j<size;j++){
				fscanf(fp,"%d",&c_test[i*size+j]);
			}
		}
		fclose(fp);
	}
	else{
		for(i=0;i<size;i++){
			for(j=0;j<size;j++){
				a[i*size+j]= rand()%20;
				b[i*size+j]= rand()%20;
			}
		}
	}
}
void gemm_omp(int *A, int *B, int *C, int n) 
{	
	#pragma omp parallel
	{
		int i, j, k;
		#pragma omp for
		for (i = 0; i < n; i++) { 
			for (j = 0; j < n; j++) {
				int sum  = 0;
				for (k = 0; k < n; k++) {
					sum += A[i*n+k]*B[k*n+j];
				} 
				C[i*n+j ] = sum;
			}
		}
	}
}
int main(int argc, char * argv[]){
	int *a,*b,*c,*c_test;
	double itime, ftime, exec_time;
	omp_set_num_threads(nthread);
	printf("num of thread = %d \n",nthread);
	a = (int*) calloc(size*size,sizeof(int));
	b = (int*) calloc(size*size,sizeof(int));
	c = (int*) calloc(size*size,sizeof(int));
	if(test) c_test = (int*) calloc(size*size,sizeof(int));
	Init(a,b,c_test);
	itime = omp_get_wtime();
	gemm_omp(a,b,c,size);
	ftime = omp_get_wtime();
	exec_time = ftime - itime;
    printf("time_use is %f\n", exec_time);
	if(test){
		int result =1,i;
		for(i=0;i<size*size;i++){
			if(c[i]!=c_test[i]){
				result =0;
				break;
			}
		}
		if(result) printf("Correct answer!\n");
		else printf("Incorrect answer!\n");
	}
	return 0;
}