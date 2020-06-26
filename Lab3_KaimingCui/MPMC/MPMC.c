#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<time.h>
#include "header.h"

struct semaphore{
	int value;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
}mutex,emptyCount,fullCount;

int Number_Producers;
int Number_Consumers;
int Max_Sleep_Seconds;
int Total_Number_Items2Produce;
int Ring_Buffer_Size;
int* ringBuffer = NULL;
//producer's index that is the position where producer puts item into the buffer
int index_in = 0; 
//consumer's index
int index_out = 0; 

//ringbuffer head and tail
//int head = 0;
//int tail = 0;

//how many products produced and consumed by all threads
int pCounter = 0;
int cCounter = 0;


//P function decrease
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

void *Producer(void* param){
	//how many items producted by this thread
	int counter = 0;
	//ID of this thread
	int* pid = (int*)param;
	int flag = 0;
	//printf("I am %d producer\n",*pid);	
	while(1){
		//see if there are already enough products, if the producer job is done, exit thread
		if(flag){
			return NULL;
		}
		//generate a random sleeptime
	
		srand((unsigned int)(time(NULL)));
		int sleepSeconds = (int)rand()%Max_Sleep_Seconds;
		sleep(sleepSeconds);
		
		//produce a item
		P(&emptyCount);
		P(&mutex);
		if(pCounter == Total_Number_Items2Produce){
			V(&mutex);
			V(&emptyCount);
			flag = 1;
			continue;
		}
		//sleep(1);
		int item = 1;
		/*
		if(head -tail == Ring_Buffer_Size){
			//buffer is full
			return -1;
		}
		*/
		//put item into buffer
		index_in = index_in%Ring_Buffer_Size;
		ringBuffer[index_in] = item;
		index_in++;
		pCounter++;
		counter++;


		//printf("I am %d producer\n",*pid);	
		V(&mutex);
		V(&fullCount);
		 
	}


}

void *Consumer(void* param){
	//how many products consumed by this thread
	int counter = 0;
	int flag = 0;
	int* cid = (int*)param;
	//printf("I am %d consumer\n",*cid);
	while(1){
		//consume and product job done
		if(flag){
			return NULL;
		}

		int consume_item = 0;
		P(&fullCount);
		P(&mutex);
		//printf("I am %d consumer\n",*cid);
		/*
		if(head - tail == 0){
			return -1;
		}
		*/
		
		
		if(pCounter == Total_Number_Items2Produce && cCounter == pCounter){
			V(&mutex);
			V(&fullCount);
			flag = 1;
			continue;
		}
		//consume a item in the buffer
		index_out = index_out%Ring_Buffer_Size;
		ringBuffer[index_out] = consume_item;
		index_out++;
		cCounter++;
		counter++;
		//printf("ccounter: %d",cCounter); 
		
		if(pCounter == Total_Number_Items2Produce && cCounter == pCounter){
			V(&mutex);
			V(&fullCount);
			flag = 1;
			continue;
	
		}
		//sleep(1);
		V(&mutex);
		V(&emptyCount);

		srand((unsigned int)(time(NULL)));
		int sleepSeconds = (int)rand()%Max_Sleep_Seconds;
		sleep(sleepSeconds);
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

void main(int argc,char* argv[]){
	Number_Producers = atoi(argv[1]);
	Number_Consumers = atoi(argv[2]);
	Max_Sleep_Seconds = atoi(argv[3]);
	Total_Number_Items2Produce = atoi(argv[4]);
	Ring_Buffer_Size = atoi(argv[5]);
	
	//printf("output: %d %d %d %d %d\n",Number_Producers,Number_Consumers,Max_Sleep_Seconds,Total_Number_Items2Produce,Ring_Buffer_Size);	
	
	//initialize the semaphores
	init(&mutex);
	init(&emptyCount);
	init(&fullCount);
	mutex.value = 1;
	emptyCount.value = Ring_Buffer_Size;
	fullCount.value = 0;
	
//	printf("so far okay\n");	
	//create ring buffer and init to 0
	ringBuffer = (int *)malloc(Ring_Buffer_Size*sizeof(int));
	if(NULL == ringBuffer){
		fprintf(stderr,"malloc failed.\n");
		exit(1);
	}
	int i = 0;
	for(i = 0;i<Ring_Buffer_Size;i++){
		ringBuffer[i] = 0;
	}
	
	int* p_id;
	int* c_id;
	pthread_t* producers;
	pthread_t* consumers;
	pthread_attr_t* p_attrs;
	pthread_attr_t* c_attrs;
	void* retval;


	producers = (pthread_t*)malloc(sizeof(pthread_t)*Number_Producers);
	p_attrs = (pthread_attr_t*)malloc(sizeof(pthread_attr_t)*Number_Producers);
	p_id = (int*)malloc(sizeof(int)*Number_Producers);
	consumers = (pthread_t*)malloc(sizeof(pthread_t)*Number_Consumers);
	c_attrs = (pthread_attr_t*)malloc(sizeof(pthread_attr_t)*Number_Consumers);
	c_id = (int*)malloc(sizeof(int)*Number_Consumers);	
	
	i = 0;
	for(i = 0; i < Number_Producers;i++){
		if(pthread_attr_init(p_attrs+i)){
			perror("attr_init\n");
		}
		p_id[i] = i;
		if(pthread_create(producers+i, p_attrs+i, Producer, p_id+i) != 0){
			perror("pthread_create() produce\n");
			exit(1);
		}
	}
	for(i = 0; i < Number_Consumers;i++){
		if(pthread_attr_init(c_attrs+i)){
			perror("attr_init\n");
		}
		c_id[i] = i;
		if(pthread_create(consumers+i, c_attrs+i, Consumer, c_id+i) != 0){
			perror("pthread_create() consume\n");
			exit(1);
		}
		
	}

	
	for(i = 0; i < Number_Producers;i++){
		pthread_join(producers[i],&retval);
		//printf("producer thread %d is finished.\n",i);
	}
//	printf("pcounter: %d",pCounter);
//	printf("ccounter: %d",cCounter);
	
	for(i = 0; i < Number_Consumers;i++){
		pthread_join(consumers[i],&retval);
		//printf("consumer thread %d is finished.\n",i);
	}	
	
	printf("All threads produced %d items.\n",pCounter);
	printf("All threads consumed %d items.\n",cCounter);
	if(pCounter == cCounter){
		printf("Success!\n");
	}
	
	free(ringBuffer);
	free(producers);
	free(p_attrs);
	free(p_id);
	free(consumers);
	free(c_attrs);
	free(c_id);
	
	exit(0);
}
