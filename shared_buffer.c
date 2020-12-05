#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "buffer.h"
#define TRUE 1


buffer_item prodMidbuffer[BUFFER_SIZE]; // creating Producer/Middleman buffer
buffer_item midConsbuffer[BUFFER_SIZE]; // creating  Middleman/Consumer buffer

pthread_mutex_t mutex,mutex1; // Declaring mutexes

sem_t full, empty,full1,empty1; // Declaring semaphores

int in=0,out=0,in1=0,out1=0,prodMidCounter=0,midConsCounter=0; //counter variables

pthread_t pthreads,mthreads,cthreads; //Thread IDs for producer,middleman and consumer


pthread_attr_t attr; //Define thread attributes

/**
Function :display
Description :If the element is removed from the buffer then empty is displayed or else element
gets displayed and  also displaying value of in and out.
Input: buffer_item*
Return :void
 **/

void display(buffer_item* buf,char * queue,int inVal ,int outVal){
	if(buf[0]==0){
		printf("[""empty""]");
	}
	else {
		printf("[""%d""]",buf[0]);
	}
	if(buf[1]==0){
		printf("[""empty""]");
	}
	else {
		printf("[""%d""]",buf[1]);
	}
	if(buf[2]==0){
		printf("[""empty""]");
	}
	else {
		printf("[""%d""]",buf[2]);
	}
	if(buf[3]==0){
		printf("[""empty""]");
	}
	else {
		printf("[""%d""]",buf[3]);
	}
	if(buf[4]==0){
		printf("[""empty""]");
	}
	else {
		printf("[""%d""]",buf[4]);
	}
	printf(" in=%d ,",inVal);

	printf(" out=%d ,",outVal);
	printf("queue=%s\n",queue);

}

/**
Function :insert_item
Description :Producer inserts an item in to Producer/Middleman buffer if the buffer is not full
Input Type: buffer_item
Return Type:int
 **/
int insert_item(buffer_item item) {
	//If the buffer is full do not insert item
	if(prodMidCounter == BUFFER_SIZE) {
		// do nothing
		sleep(2);
	}
	pthread_mutex_lock(&mutex);//Acquire mutex lock
	sem_wait(&empty); //Acquire  empty lock
	//If the buffer is not full add item and increment the buffer counter
	if(prodMidCounter< BUFFER_SIZE) {
		prodMidbuffer[in] = item; //Insert the item in to the buffer
		printf("Insert_item inserted item %d at position %d\n",item,in);
		in=(in+1) % BUFFER_SIZE; //Increment the in value to point to next avaiable free slot
		prodMidCounter++; //Increment the buffer counter
		char* queue1="Producer/Middleman"; //Queue name
		display(prodMidbuffer,queue1,in ,out); //This method is called to display buffer elements

	}

	pthread_mutex_unlock(&mutex);  //Release mutex lock
	sem_post(&full); //Signal the semaphore as full

}

/**
Function :remove_item
Description :Middleman removes an item from  Producer/Middleman buffer if the buffer is not empty.
Input Type: buffer_item
Return Type:int
 **/
int remove_item(buffer_item *item) {
	//If the  buffer has no elements then there is nothing to remove.
	if(prodMidCounter==0) {
		//do nothing
		sleep(2);
	}

	pthread_mutex_lock(&mutex); //Acquire mutex lock
	sem_wait(&full); // Acquire the full lock

	// When the buffer is not empty remove the item and decrement the buffer counter.

	if(prodMidCounter > 0) {
		prodMidCounter--;  //Decrement the buffer counter
		*item=prodMidbuffer[out]; // Item to be deleted
		prodMidbuffer[out]=0; //Store the deleted item to be 0

		printf("Remove_item removed item %d at position %d\n",*item, out);
		out =(out+1)%BUFFER_SIZE ; //Increment the out value to point to first filled spot

		char * queue2="Producer/Middleman"; //Queue name
		display(prodMidbuffer,queue2,in,out); //This method is called to display buffer elements			
	}

	pthread_mutex_unlock(&mutex); //Release mutex lock
	sem_post(&empty); //Signal sempahore as empty
}

/**
Function :insert_item1
Description :Middleman inserts an item in to Middleman/Consumer buffer if the buffer is not full
Input Type: buffer_item
Return Type:int
 **/
int insert_item1(buffer_item item) {
	//If the buffer is full do not insert item
	if(midConsCounter == BUFFER_SIZE) {
		// do nothing
		sleep(2);
	}
	pthread_mutex_lock(&mutex1); //Acquire mutex lock
	sem_wait(&empty1); //Acquire  empty lock
	//If the buffer is not full add item and increment buffer counter
	if(midConsCounter< BUFFER_SIZE) {
		midConsbuffer[in1] = item; //Insert the item in the buffer
		printf("Insert_item inserted item %d at position %d\n",item,in1);
		in1=(in1+1) % BUFFER_SIZE; //Increment the in1 value to point to next avaiable free slot
		midConsCounter++; //Increment the buffer counter
		char * queue3="Middleman/Consumer"; //Queue name
		display(midConsbuffer,queue3,in1,out1); //This method is called to display buffer elements	
	}
	pthread_mutex_unlock(&mutex1); //Release mutex lock
	sem_post(&full1); ///Signal the semaphore as full
}

/**
Function :remove_item1
Description :Consumer removes an item from  Middleman/Consumer buffer if the  buffer is not empty
Input Type: buffer_item
Return Type:int
 **/
int remove_item1(buffer_item *item) {
	//If the  buffer has no elements then there is nothing to remove.
	if(midConsCounter==0) {
		//do nothing
		sleep(2);
	}

	pthread_mutex_lock(&mutex1); //Acquire mutex lock
	sem_wait(&full1); // Acquire the full lock

	// When the buffer is not empty remove the item and decrement the buffer counter

	if(midConsCounter > 0) {
		midConsCounter--; //Decrement the buffer counter
		*item=midConsbuffer[out1]; // Item to be deleted
		midConsbuffer[out1]=0; //Store the deleted item to  be 0

		printf("Remove_item removed item %d at position %d\n",*item, out1);
		out1 =(out1+1)%BUFFER_SIZE ; //Increment the out1 value to point to first filled spot

		char * queue4="Middleman/Consumer"; //Queue name
		display(midConsbuffer,queue4,in1,out1); //This method is called to display buffer elements	
	}
	pthread_mutex_unlock(&mutex1); //Release mutex lock
	sem_post(&empty1); //Signal sempahore as empty
}

/**
Function :*producer
Description : When a producer thread is created this function is called where it sleeps for some random amount of time and inserts the random item using insert_item function
Input Type: *param
Return Type:void
 **/
void *producer(void *param) {
	buffer_item item;

	while(TRUE) {
		int pSleepRandomNum = (rand()%3)+1; //Generating random number for sleep
		printf("Producer thread %ld sleeping for %d seconds \n ",pthread_self(),pSleepRandomNum);
		sleep(pSleepRandomNum); //sleep for the random amount of time.
		item = (rand()%50)+1;  //Random number for inserting item in to a buffer

		//Call insert_item to insert item in to buffer
		if(insert_item(item)<0) {
			printf("Producer error\n");
		}
		else {
			printf("Producer thread %ld inserted value %d\n",pthread_self(),item);
		}

	}
	//Exit producer thread
	pthread_exit(NULL);
}

/**
Function :*middleman
Description : When a middleman thread is created this fucntion is called where it sleeps for some random amount of time and removes the item and later inserts the item to another buffer.
Input Type: *param
Return Type:void
 **/
void *middleman(void *param) {
	buffer_item item;

	while(TRUE) {
		int mSleepRandomNum = (rand()%3)+1; //Generating random number for sleep
		printf("Middleman thread %ld sleeping for %d seconds \n ",pthread_self(),mSleepRandomNum);
		sleep(mSleepRandomNum); //sleep for the random amount of time.
		//Call remove_item to delete the item from the buffer

		if(remove_item(&item)<0) {
			printf("Middleman error\n");
		}
		else {
			printf("Middleman thread %ld removed value %d\n",pthread_self(),item);
		}
		//Call insert_item1 to insert item in to buffer

		if(insert_item1(item)<0) {
			printf("Middleman error\n");
		}
		else {
			printf("Middleman thread %ld inserted value %d\n",pthread_self(),item);
		}

	}
	//Exit Middleman Thread
	pthread_exit(NULL);
}

/**
Function :*consumer
Description : When a consumer thread is created this fucntion is called where it sleeps for some random amount of time and removes the item using remove_item function
Input Type: *param
Return Type:void
 **/
void *consumer(void *param) {
	buffer_item item;
	while(TRUE) {
		//Sleep for a random period of time

		int cSleepRandomNum = (rand()%3)+1; //Generating random number for sleep

		printf("Consumer thread %ld sleeping for %d seconds \n ",pthread_self(),cSleepRandomNum);

		sleep(cSleepRandomNum); //Sleep for the random amount of time.

		//call remove_item1 to delete the item from the buffer
		if(remove_item1(&item)<0) {
			printf("Consumer error\n");
		}
		else {
			printf("Consumer thread %ld removed value %d \n",pthread_self(),item);
		}

	}
	//Exit consumer thread
	pthread_exit(NULL);
}


int main(int argc, char *argv[]) {

	printf("Main thread beginning\n");

	int i,j,k;

	//Checking if correct number of arguments are passed
	if(argc != 5) {
		fprintf(stderr," Pass correct number of arguments\n");
	}
	int sleepTime = atoi(argv[1]); //Length of time program should run
	int numOfProducers = atoi(argv[2]); // Number of producer threads
	int numOfMiddlemans = atoi(argv[3]); // Number of middleman threads
	int numOfConsumers = atoi(argv[4]); // Number of consumer threads

	pthread_mutex_init(&mutex, NULL); //Created mutex lock

	sem_init(&full, 0, 0); //Initialized sempahore full to 0.

	sem_init(&empty, 0, BUFFER_SIZE); //Initialized empty semaphore to BUFFER_SIZE

	pthread_mutex_init(&mutex1, NULL);//created mutex lock

	sem_init(&full1, 0, 0); //Initialized sempahore full1 to 0.

	sem_init(&empty1, 0, BUFFER_SIZE); //Initialized empty1 semaphore to BUFFER_SIZE


	pthread_attr_init(&attr); //getting default attributes

	pthread_t pthreads[numOfProducers]; //Defining the number of producer threads
	pthread_t mthreads[numOfMiddlemans]; //Defining the number of middleman threads
	pthread_t cthreads[numOfConsumers]; //Defining the number of consumer threads

	//Create number of  producer threads as per input.
	for(i = 0; i < numOfProducers; i++) {

		pthread_create(&pthreads[i],&attr,producer,NULL); //Create producer thread
		printf("Creating  producer thread with id %lu\n", pthreads[i]);
	}

	//Create number of  middleman threads as per input.
	for(j = 0; j < numOfMiddlemans; j++) {

		pthread_create(&mthreads[j],&attr,middleman,NULL); //Create middleman thread
		printf("Creating  middleman thread with id %lu\n", mthreads[j]);
	}

	//Create number of consumer threads as per input
	for(k = 0; k < numOfConsumers; k++) {

		pthread_create(&cthreads[k],&attr,consumer,NULL); //Create consumer thread
		printf("Creating consumer thread with id %ld\n", cthreads[k]);
	}


	printf(" Main thread sleeping for %d seconds \n",sleepTime);
	sleep(sleepTime); //sleep time based on input

	printf("Main thread exiting\n");
	exit(0); //Exit the program
}


