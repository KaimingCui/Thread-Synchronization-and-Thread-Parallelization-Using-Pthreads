#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<time.h>
#include "header.h"

int Matrix_Size = 0;
int Number_Thread = 0;
double** A = NULL;
double** B = NULL;
double** C_seq = NULL;
double** C_mul = NULL;


//global row number an column number to ensure every element in C_mul only be computed once
//need synchronization among threads
int row = 0;
int col = 0;
int finishFlag = 0;
//semaphore
struct semaphore{
	int value;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
}mutex;



void P(struct semaphore* S){
	pthread_mutex_lock(&S->mutex);
	while(S->value <= 0){
		pthread_cond_wait(&S->cond,&S->mutex);
	}
	S->value--;
	//printf("value = %d",S->value);
	pthread_mutex_unlock(&S->mutex);
}

void V(struct semaphore* S){
	//printf("before mutex value: %d\n",S->value);
	pthread_mutex_lock(&S->mutex);
	S->value++;
	if(S->value > 0){
		pthread_cond_signal(&S->cond);
	}
	//printf("value = %d",S->value);
	pthread_mutex_unlock(&S->mutex);
}


//computer the matrix multiplication sequentially
void sequence_multi(int size,double **C, double **A, double **B){
	int i = 0;
	int j = 0;
	int k = 0;
	double temp = 0;

	for(i = 0; i < size; i++){
		for(j =0; j < size; j++){
			temp = 0;
			for(k = 0; k < size;k++){
				temp += A[i][k]*B[k][j];
			}
			C[i][j] = temp;
		}
	}

}

void init(struct semaphore* S){
	S->value = 0;
	if(pthread_mutex_init(&S->mutex,NULL)){
		perror("mutex init error\n");
	}
	if(pthread_cond_init(&S->cond,NULL)){
		perror("cond init error\n");
	}
}

//each thread do
void* do_thread(void* arg){
	int i = 0;
	int j = 0;

	while(1){
		//critical section
		P(&mutex);
		if(row == Matrix_Size-1 && col == Matrix_Size-1){
			finishFlag = 1;
			V(&mutex);
			return NULL;
		}
		else if(col == Matrix_Size-1){
			col = 0;
			row++;
			i = row;
			j = col;
		}
		else{
			col++;
			i = row;
			j = col;
		}

		V(&mutex);

		//computer the element
		int k = 0;
		double temp = 0;
		for(k = 0; k < Matrix_Size;k++){
			temp += A[i][k]*B[k][j];
		}
		C_mul[i][j] = temp;

	}

}



void main(int argc, char* argv[]){
	
	//initialize
	Matrix_Size = atoi(argv[1]);
	Number_Thread = atoi(argv[2]);
	int i = 0;
	init(&mutex);
	mutex.value = 1;


	//malloc the matrixes
	A = (double **)malloc(sizeof(double*)*Matrix_Size);
	B = (double **)malloc(sizeof(double*)*Matrix_Size);
	C_seq = (double **)malloc(sizeof(double*)*Matrix_Size);
	C_mul = (double **)malloc(sizeof(double*)*Matrix_Size);
	for(i = 0;i<Matrix_Size;i++){
		A[i] = (double *)malloc(sizeof(double)*Matrix_Size);
		B[i] = (double *)malloc(sizeof(double)*Matrix_Size);
		C_seq[i] = (double *)malloc(sizeof(double)*Matrix_Size);
		C_mul[i] = (double *)malloc(sizeof(double)*Matrix_Size);
	}
	
	//initialize the matrixes
	int j = 0;
	for(i = 0; i < Matrix_Size;i++){
		for(j = 0; j < Matrix_Size;j++){
			A[i][j] = j;
			B[i][j] = j;
			C_seq[i][j] = 0;
			C_mul[i][j] = 0;
		}
	}


	//compute matrix sequentially	
	sequence_multi(Matrix_Size,C_seq,A,B);

	//compute matrix concurrently
	int* p_id;
	pthread_t* threads;
	pthread_attr_t* p_attrs;
	void* retval;


	threads = (pthread_t*)malloc(sizeof(pthread_t)*Number_Thread);
	p_attrs = (pthread_attr_t*)malloc(sizeof(pthread_attr_t)*Number_Thread);
	p_id = (int*)malloc(sizeof(int)*Number_Thread);

	i = 0;
	for(i = 0;i < Number_Thread;i++){
		if(pthread_attr_init(p_attrs+i)){
			perror("attr_init\n");
		}

		p_id[i] = i;
		if(pthread_create(threads+i,p_attrs+i,do_thread,p_id+i) != 0){
			perror("pthread_create\n");
			exit(1);
		}
	}



	//join threads
	for(i = 0; i < Number_Thread; i++){
		pthread_join(threads[i],&retval);

	}

	//check two result to see if they are same or not
	j = 0;
        i = 0;
        int equalFlag = 1;
        for(i = 0;i < Matrix_Size;i++){
                for(j=0;j<Matrix_Size;j++){
			if(C_seq[i][j] != C_mul[i][j]){
				equalFlag = 0;
				break;
                        }
                }
		if(!equalFlag){
			break;
		}
        }

	if(equalFlag == 1){
		printf("Two result Matrix are same. Success.\n");
	}
	else{
		printf("Two result are not same. Failed.\n");
	}


	for(i = 0; i < Matrix_Size; i++){
		free(*(A+i));
		free(*(B+i));
		free(*(C_seq+i));
		free(*(C_mul+i));
	}
	
	exit(0);
}
