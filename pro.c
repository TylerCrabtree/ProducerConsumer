//Tyler Crabtree

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include "buffer.h"

//For while loops
#define TRUE 1

//Hide warnings
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wint-to-void-pointer-cast"

//Functions stubs
void producer(void *param);
void consumer(void *param);
int insertItem(int buffer_item, int item);       
int removeItem(int buffer_item, int *item);      


//Global variables 
int buffer[1][BUFFER_SIZE];
sem_t full[1];
sem_t empty[1];
pthread_mutex_t mutex[1];
int counter[1];
int insert[1];
int removed[1];

// Producer function (didn't print errors like book, since prompt said not to print other messages, so only prints produced items)
void producer(void *param) {
     int item = 0;                 // for produced items
     int sleepDelay = 0;           // for sleep
     int buffer = 0;               // set to correct buffer
     while(TRUE) {                 // mimic prompt
          sleepDelay = rand() % 5; // sleep in range of 0 to 5 seconds
          sleep(sleepDelay);       // sleep for given time
          item = rand();           // randomize item
          sem_wait(&empty[buffer]);// sem_wait buffer
          pthread_mutex_lock(&mutex[buffer]); // set mutex
          if(!insertItem(buffer, item)){// insertitem
               printf("producer produced %d\n", item); // Mimic's Proffesor's Zhang's prompt  
          }
          pthread_mutex_unlock(&mutex[buffer]); // unlock
          sem_post(&full[buffer]); // sem_post buffer
     }
}

//Consumer function
void consumer(void *param) {
      int item = 0;                 // for produced items
     int sleepDelay = 0;           // for sleep
     int buffer = 0;               // set to correct buffer
     while(TRUE) {                 // mimic prompt
          sleepDelay = rand() % 5; // sleep in range of 0 to 5 seconds
          sleep(sleepDelay);       // sleep for given time
          item = rand();           // randomize item
          sem_wait(&empty[buffer]);// sem_wait buffer
          pthread_mutex_lock(&mutex[buffer]); // set mutex
          if(!removeItem(buffer, &item)){// remove item
               printf("consumer consumed %d \n", item); // print consumed items
          }
          pthread_mutex_unlock(&mutex[buffer]);        // unlock
          sem_post(&empty[buffer]);     //sem_post buffer
     }
}


int insertItem(int buffer_item, int item) {
     int success = 0;   // prompt said to return 0 or 1,
     int fail = -1;
     if(counter[buffer_item] < BUFFER_SIZE) { // check to insert
          buffer[buffer_item][insert[ buffer_item ]] = item; // insert
          insert[buffer_item] = (insert[buffer_item] + 1) % BUFFER_SIZE; // update new size
          counter[buffer_item]++; // increase counter
          return success; // return 0
     }
     return fail; // retun -1
}

int removeItem(int buffer_item, int *item) {
     int success = 0; // prompt said to return 0 or 1,
     int fail = -1;
     if(0 < counter[buffer_item]) { // check if item can be removed
          *item = buffer[buffer_item][removed[ buffer_item] ]; // remove
          removed[buffer_item] = (removed[buffer_item] + 1) % BUFFER_SIZE; // update size
          counter[buffer_item]--; // decrease counter
          return success; // return 0
     }
     
     return fail; // return -1 
}



// main follows this format
/* 1. Get command line arguments argv[1],argv[2],argv[3] */ 
/* 2. Initialize buffer */
/* 3. Create producer thread(s) */
/* 4. Create consumer thread(s) */
/* 5. Sleep */
/* 6. Exit */

int main(int argc, char *argv[]) {
     printf("Welcome to the Producer-Consumer program\n");
     int producers;    //argv 2
     int consumers;   //argv 3
     int sleepDelay; //argv 1
     int createdProducers; // count of producers
     int createdConsumers;// count of consumers
     srand(time(NULL));  // initialize ransom time 
     //Produce error if wrong number of arguments, or any arguments are negative 
     if((argc != 4)|| (atoi(argv[1]) < 0 )|| (atoi(argv[2]) < 0) ||( atoi(argv[3]) < 0) ) {  // checks arguments
          fprintf(stderr, "Please ensure that there are three positve arguments\n"); // error prompt
          return -1;
     }

     sleepDelay = atoi(argv[1]);  //grab sleep
     producers = atoi(argv[2]);  // grab producers
     consumers = atoi(argv[3]); // grab consumers

     createdProducers = 0;
     while(createdProducers < 1){ // initialize arrays
          createdConsumers = 0;   // initialize consumers
          while(createdConsumers < BUFFER_SIZE) {
               buffer[createdProducers][createdConsumers] = -1; // set to -1
               createdConsumers++; // increase counter
          }
          createdProducers++;  // increase counter 
     }
     
     counter[0] = 0;  //initialize arrays
     insert[0] = 0;  //initialize arrays
     removed[0] = 0;//initialize arrays
     
     sem_init(&full[0], 0, 0); // sem_init
     sem_init(&empty[0], 0, BUFFER_SIZE);
     pthread_mutex_init(&mutex[0], NULL);    //init mutex    
     pthread_t producer_threads[producers]; // init threads
     pthread_t consumer_threads[consumers]; 
     while(createdProducers <= producers) { // create producers up till argument
          if(pthread_create(&producer_threads[createdProducers], NULL, (void *) producer, (void *) createdProducers) != 0) { 
               fprintf(stderr, "Error: Producer thread creation");
               exit(1);  // exit if need be
          }
           
          createdProducers++;
     }
     createdConsumers = 1;
     while(createdConsumers <= consumers) { // create consumers up till argument
          if(pthread_create(&consumer_threads[createdConsumers], NULL, (void *) consumer, (void *) createdConsumers) != 0) {
               fprintf(stderr, "Error: Consumer thread creation");
               exit(2);  // differentiate errors 
          }    
          createdConsumers++;
     }
     sleep(sleepDelay); // sleep
     return 0;
}