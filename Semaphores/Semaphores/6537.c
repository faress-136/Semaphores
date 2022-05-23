#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#define BufferSize 15
const int MAX = 22;
const int MIN =1;

const int MIN_M = 1;
const int MAX_M = 18;

const int MIN_C=1;
const int MAX_C=6;
//const int SizeOfBuffer = 100;

sem_t counterhold;
sem_t bufferhold;
sem_t empty;
sem_t full;

int buffer[BufferSize];
//int inBuffer = 0;
//int data=0;
int counter =0;
int rear = -1;
int front = -1;


void InsertQueue(int data){
    if((rear == BufferSize - 1 && front == 0 ) ||(front == rear+1)){
        //printf("Buffer is Full \n");
        return;
    }
    else
    {
        if(front== - 1){
            front = 0;
            
        }
        rear = (rear + 1)%BufferSize;
        buffer[rear] = data;
        
    }
}


void Dequeue()
{
    if(front == - 1)
    {
        // printf("Buffer is Empty \n");
        return;
    }
    else
    {
        if(front == rear){
            front = rear = -1;
            //printf("Buffer is Empty \n");
            
        }
        else{
            front = (front + 1)% BufferSize;
        }
    }
}


void *monitorFunc(void * args){
    while (1) {
        printf("Monitor thread: waiting to read counter\n");
        sem_wait(&counterhold);
        int hand =0;
        hand = counter;
        counter = 0;
        sem_post(&counterhold);
        int value2 = 0;
        sem_getvalue(&full, &value2);
        if (value2 == BufferSize) {
            printf("Monitor thread: Buffer full!!\n");
        }
        sem_wait(&empty);
        sem_wait(&bufferhold);
        //        data = counter;
        //        buffer[inBuffer] = counter;
        printf("Monitor thread: reading a count value %d\n",hand);
        InsertQueue(hand);
        printf("Monitor thread: Writing to buffer at position: %d\n",rear);
        
        
        sem_post(&bufferhold);
        sem_post(&full);
        
        
        srand(time(0));
        int randMonitor = MIN_M + rand() % (MAX_M+1 - MIN_M);
        sleep(randMonitor);
        
    }
    
    return NULL;
}

void *collectorFunc(void * args){
    while (1) {
        int value =0;
        sem_getvalue(&full, &value);
        if (value == 0) {
            printf("Collector thread: nothing is in the buffer!\n");
        }
        sem_wait(&full);
        sem_wait(&bufferhold);
        printf("Collector thread: reading a count value from position: %d\n",front);
        Dequeue();
        sem_post(&bufferhold);
        sem_post(&empty);
        
        
        
        srand(time(0));
        int randCollector = MIN + rand() % (MAX+1 - MIN);
        sleep(randCollector);
        
    }
    
    
    return NULL;
    
}


void *counterFunc(void * args){
    while (1) {
        printf("Counter thread %ld: received a message\n",(long) pthread_self());
        printf("Counter thread %ld: waiting to write\n",(long) pthread_self());
        
        sem_wait(&counterhold);
        counter++;
        printf("Counter thread %ld: Now adding to counter, counter value = %d \n",pthread_self(),counter);
        sem_post(&counterhold);
        
        srand(time(0));
        int randCounter = MIN_C + rand() % (MAX_C+1 - MIN_C);
        sleep(randCounter);
        
    }
    
    
    return NULL;
    
}


//void random_int()
//{
//    srand(time(0));
//    int x = MIN + rand() % (MAX+1 - MIN);
//    printf("%d",x);
//
//}

int main() {
    sem_init(&full, 0, 0);     //Sempaohore init of Buffer which we inc and put inside
    sem_init(&counterhold, 0, 1);
    sem_init(&bufferhold, 0, 1);                                  //Semaphore init of Cirtical Section
    sem_init(&empty, 0, BufferSize);      // Semaphore init of Size Of Buffer
    
    int nCounter =0;
    printf("Enter number of threads between 5 --> 10 for mCounter Thread: \n");
    scanf("%d",&nCounter);
    
    while (nCounter<5 || nCounter >10 ){
        printf("It's recommended to enter value between 5 --> 10 for mCounter Thread: \n");
        scanf("%d",&nCounter);
    }
    pthread_t monitor,collector;
    pthread_t counterThreads[nCounter];
    
    for (int i =0; i<nCounter; i++) {
        pthread_create(&counterThreads[i],NULL ,counterFunc ,NULL);
    }
    
    pthread_create(&monitor, NULL, monitorFunc, NULL);
    pthread_create(&collector, NULL, collectorFunc, NULL);
    
    for (int i =0; i<nCounter; i++) {
        pthread_join(counterThreads[i], NULL);
    }
    pthread_join(monitor, NULL);
    pthread_join(collector, NULL);
    
    //pthread_join
    
    
    return 0;
}
