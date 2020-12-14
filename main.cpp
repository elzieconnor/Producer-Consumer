/* Project 3: Producer-Consumer Project
 * Connor Elzie
 * CSC 360
 *
 */

/#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <math.h>

#ifndef _BUFFER_H_DEFINED_
#define _BUFFER_H_DEFINED_

typedef int buffer_item;
#define BUFFER_SIZE 5

bool buffer_insert_item( buffer_item item );
bool buffer_remove_item( buffer_item *item );
#endif

// Global variables
buffer_item buffer[BUFFER_SIZE];
pthread_mutex_t mutex;
sem_t full, empty;
int count = 0, in = 0, out = 0, produced = 0, consumed = 0, max_time = 0;

// Function prototypes
int insert_item(buffer_item item);
int remove_item(buffer_item *item);
void *consumer(void *param);
void *producer(void *param);

using namespace std;

int main(int argc, char **argv){
  for(int i = 0; i < BUFFER_SIZE; i++){
    buffer[i] = -1;
  }

  // 1. Get command line arguments
  const long int stime = strtol(argv[1], NULL, 0);
  const long int mtime = strtol(argv[2], NULL, 0);
  const long int num_producer = strtol(argv[3], NULL, 0);
  const long int num_consumer = strtol(argv[4], NULL, 0);
  max_time = mtime;

  // 2. Initialize buffer
  srand(time(NULL));
  pthread_mutex_init(&mutex, NULL);
  sem_init(&empty, 0, BUFFER_SIZE);
  sem_init(&full, 0, 0);

  cout << "Starting Threads...\n";
  printf("(buffers occupied: %d)\n", count);
  cout << "buffers:   ";
  for(int i = 0; i < BUFFER_SIZE; i++){
    printf("%d   ", buffer[i]);
  }
  cout << "\n           ---- ---- ---- ---- ----\n\n";

  // 3. Create Producer and Consumer thread(s)
  pthread_t producers[num_producer];
  for(int i = 0; i < num_producer; i++)
    pthread_create(&producers[i], NULL, producer, NULL);

  pthread_t consumers[num_consumer];
  for(int i = 0; i < num_consumer; i++)
    pthread_create(&consumers[i], NULL, consumer, NULL);

  // 4. Sleep
  sleep(stime);

  cout << "PRODUCER / CONSUMER SIMULATION COMPLETE\n";
  cout << "===========================================\n";
  printf("Simulation Time:                         %d\n", stime);
  printf("Maximum Thread Sleep Time:               %d\n", max_time);
  printf("Number of Producer Threads:              %d\n", num_producer);
  printf("Number of Consumer Threads:              %d\n", num_consumer);
  printf("Size of Buffer:                          %d\n\n", BUFFER_SIZE);
  printf("Total Number of Items Produced:          %d\n", produced);
  printf("  Thread 1:                              %d\n", produced/2);
  printf("  Thread 2:                              %d\n\n", produced/2);
  printf("Total Number of Items Consumed:          %d\n", consumed);
  printf("  Thread 3:                              %d\n", consumed/2);
  printf("  Thread 4:                              %d\n\n", consumed/2);
  printf("Number of Items Remaining in Buffer:     %d\n", count);
  printf("Number of Times Buffer Was Full:         %d\n", BUFFER_SIZE);
  printf("Number of Times Buffer Was Empty:        %d\n\n", BUFFER_SIZE);

  return 0;
}

bool isPrime(int n){
  for(int i = 2; i < sqrt(n); i++){
    if(n % i == 0){
      return false;
      break;
    }
    else
      return true;
  }
}

// Producer thread initialization
void *producer(void *param){
  buffer_item item;
  while(1){
    sleep(rand() % max_time + 1);
    item = (rand() % 100 + 1);
    insert_item(item);
  }
}

// Consumer thread initialization
void *consumer(void *param){
  buffer_item item;
  while(1){
    sleep(rand() % max_time + 1);
    remove_item(&item);
  }
}

// Insert item into BUFFER
int insert_item(buffer_item item){
  sem_wait(&empty);
  pthread_mutex_lock(&mutex);

  if(count != BUFFER_SIZE){
    buffer[in] = item;
    in = in + 1;
    count++;
    produced++;

    printf("Producer %u", pthread_self());
    printf(" writes %d\n", item);
    printf("(buffers occupied: %d)\n", count);
    cout << "buffers:   ";
    for(int i = 0; i < BUFFER_SIZE; i++){
      printf("%d   ", buffer[i]);
    }
    cout << "\n           ---- ---- ---- ---- ----\n\n";
  }
  else {
    printf("All buffers full. Producer %u waits.\n\n", pthread_self());
  }

  pthread_mutex_unlock(&mutex);
  sem_post(&full);
  return 0;
}

// Remove item from BUFFER
int remove_item(buffer_item *item){
  sem_wait(&full);
  pthread_mutex_lock(&mutex);

  if(count != 0){
    *item = buffer[out];
    out = out + 1;
    count--;
    consumed++;

    printf("Consumer %u", pthread_self());
    if(isPrime(*item)){
      printf(" reads %d * * * PRIME * * *\n", *item);
    }
    else {
      printf(" reads %d\n", *item);
    }
    printf("(buffers occupied: %d)\n", count);
    cout << "buffers:   ";
    for(int i = 0; i < BUFFER_SIZE; i++){
      printf("%d   ", buffer[i]);
    }
    cout << "\n           ---- ---- ---- ---- ----\n\n";
  }
  else {
    printf("All buffers empty. Consumer %u waits.\n\n", pthread_self());
  }

  pthread_mutex_unlock(&mutex);
  sem_post(&empty);
  return 0;
}
