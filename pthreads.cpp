#include<stdio.h>
#include<pthread.h>
#include <stdlib.h>
#include<time.h>
 
#define thread_count 4
#define size 20
#define test 1

int local_size[thread_count];
int start_row[thread_count];
int a[size*size], b[size*size], c[size*size],c_test[size*size];

void* pthread_mult(void* rank){
    int* my_id = (int*) rank;
    int quantity = local_size[*my_id];
	int rowNumber = start_row[*my_id];
    for(int k = 0; k < quantity; k++){
        for(int i = 0; i<size; i++){
            int sum =0;
            for(int j = 0; j<size; j++)
                sum += a[(rowNumber+k)*size+j] * b[j*size+i];
            c[(rowNumber+k)*size+i] = sum;
        }
 
    }
}
void* Init(){
	int i, j;
    if(test){
		FILE *fp;
		fp=fopen("a.txt","r");
		for(i=0;i<size;i++) 
			for(j=0;j<size;j++)
				fscanf(fp,"%d",&a[i*size+j]);
		fclose(fp);
		fp=fopen("b.txt","r");
		for(i=0;i<size;i++)
			for(j=0;j<size;j++)
				fscanf(fp,"%d",&b[i*size+j]);
		fclose(fp);
		fp=fopen("c.txt","r");
		for(i=0;i<size;i++)
			for(j=0;j<size;j++)
				fscanf(fp,"%d",&c_test[i*size+j]);
		fclose(fp);
	}
	else{
		for(i=0; i<size*size; i++){
			a[i] = rand()% 20;
			b[i] = rand()%20;
		}
	}
}
 
int main(int argc, char* argv[]){
    pthread_t t[thread_count];
	pthread_attr_t a;
	int id[thread_count];
    int n = size/thread_count;
	for(int i =0; i < thread_count;i++)	local_size[i] = n;
	if(n*thread_count < size){
		int temp = size - n*thread_count;
		for(int i =0; i < temp;i++) local_size[i] ++;
	}
	int counter =0;
	for(int i =0; i< thread_count;i++){
		start_row[i] = counter;
		counter += local_size[i];
	}
	printf("size = %d, number of thread = %d \n",size,thread_count);
	struct timeval stop, start;
	gettimeofday(&start, NULL);
	int i, thread;
    for(i = 0; i<thread_count; i++){
		id[i] = i;
		pthread_attr_init(&a);
		pthread_create(&t[i],&a,pthread_mult, (void*)&id[i]);
	}
    for (thread=0; thread<thread_count; thread++)
        pthread_join(t[thread], NULL);
    gettimeofday(&stop, NULL);
	printf("time_use is %lu ms\n",(stop.tv_sec - start.tv_sec) * 1000 + (stop.tv_usec - start.tv_usec)/1000);
	if(test){
		int result =1;
		for(i =0;i<size*size;i++){
			if(c[i] != c_test[i]){
				result = 0;
				break;
			}
		}
		if(result) printf("Correct answer!\n");
		else printf("Wrong answer!\n");
	}
    return 0;
}