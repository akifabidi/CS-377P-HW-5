#include<iostream>
#include <math.h>
#include <atomic>
using namespace std;

#define MAX_THREADS 1
int threadArg[MAX_THREADS];
pthread_t handles[MAX_THREADS];
std::atomic<double> pi{0.0};
int points;
double step3;

void add_to_pi(double bar) {
  auto current = pi.load();
  while (!pi.compare_exchange_weak(current, current + bar));
}

double f_unit_circle(double x)
{
    return (sqrt(1 - x * x));
}


void * part5(void *threadIdPtr) {
    
    int myId = *(int*)threadIdPtr;
    for (int i =myId; i < points; i += MAX_THREADS)
    {
        double x = step3 * ((double)i) - 1;
        double value =  step3 * f_unit_circle(x);

        // add to global sum
        add_to_pi(value);
    }
}

int main()
{    

    uint64_t execTime; /*time in nanoseconds*/
    struct timespec tick, tock;

    
    points = 1000000000;
    step3 = 2.0 / points;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    clock_gettime(CLOCK_MONOTONIC_RAW, &tick);
    for(int i = 0; i < MAX_THREADS; i++) {
        threadArg[i] = i;
        pthread_create(& handles[i], &attr, part5, &threadArg[i]);
    }

     for (int i=0; i< MAX_THREADS; i++) {
        pthread_join(handles[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &tock);
    execTime = 1000000000 * (tock.tv_sec - tick.tv_sec) + tock.tv_nsec - tick.tv_nsec;
    printf("\n ----PART 5---- \n elapsed process CPU time = %llu nanoseconds\n", (long long unsigned int)execTime);

    printf("thread %d, pi = %.20f\n", MAX_THREADS, pi * 2);
    
    return 0;
}