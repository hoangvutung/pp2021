#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<time.h>

#define size 20
#define test 1

void* Init(int* a, int* b,int* c_test, int newSize);
int main(int argc, char* argv[]){
    int *a,*b,*c, *a_m, *c_m, *c_test;
	int id,numProcs,newSize=size;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
    int n = size/numProcs;
	if(size%numProcs ){
		n = n+1;
		newSize = n * numProcs;
	}
	a = (int*) calloc(newSize*newSize,sizeof(int));
	a_m = (int*) calloc(newSize*n,sizeof(int));
	c_m = (int*) calloc(newSize*n,sizeof(int));
	b = (int*) calloc(newSize*newSize,sizeof(int));
	if(test) c_test = (int*) calloc(newSize*newSize,sizeof(int));
	c = (int*) calloc(newSize*newSize,sizeof(int));
	int start_row = id * n;
	if(id ==0) Init(a, b,c_test,newSize);
	MPI_Bcast(b, newSize*newSize, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(a, newSize*n, MPI_INT, a_m, newSize*n, MPI_INT, 0, MPI_COMM_WORLD);
	double start = MPI_Wtime();
	for(int k = 0; k < n; k++){
        for(int i = 0; i<newSize; i++){
            int sum =0;
            for(int j = 0; j<newSize; j++)
                sum += a_m[k*newSize+j] * b[j*newSize+i];
            c_m[k*newSize+i] = sum;
        }
    }
	MPI_Gather(c_m, newSize*n, MPI_INT, c, newSize*n, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);
    if(id ==0){
		double end = MPI_Wtime();
		double time_spent = end - start; 
		printf("time_use is %f\n", time_spent);
		if(test){
			int result = 1;
			for(int i=0;i<newSize*newSize;i++) {
				if(c[i] != c_test[i]){
					result=0;
					break;
				}
			}
			if(result) printf("Correct answer!");
			else printf("Wrong answer!");
		}
	}
	MPI_Finalize();
    return 0;
}
void* Init(int* a, int* b,int* c_test, int newSize){
	FILE *fp;
	if(test){
		int i, j;
		fp=fopen("a.txt","r");
		for(i=0;i<newSize;i++){
			for(j=0;j<newSize;j++){
				if(i<size && j<size){
					fscanf(fp,"%d",&a[i*newSize+j]);
				}
				else a[i*newSize+j]=0;
			}
		}
		fclose(fp);
		fp=fopen("b.txt","r");
		for(i=0;i<newSize;i++){
			for(j=0;j<newSize;j++){
				if(i<size && j<size) {
					fscanf(fp,"%d",&b[i*newSize+j]);
				}
				else b[i*newSize+j]=0;
			}
		}
		fclose(fp);
		fp=fopen("c.txt","r");
		for(i=0;i<newSize;i++){
			for(j=0;j<newSize;j++){
				if(i<size && j<size) {
					fscanf(fp,"%d",&c_test[i*newSize+j]);
				}
				else c_test[i*newSize+j]=0;
			}
		}
		fclose(fp);
	}
	else{
		for(int i=0;i<newSize;i++){
			for(int j=0;j<newSize;j++){
				if(i<size && j<size){
					a[i*newSize+j]= rand()%20;
					b[i*newSize+j]= rand()%20;
				}
				else a[i*newSize+j]= b[i*newSize+j]= 0;
			}
		}
	}
}
