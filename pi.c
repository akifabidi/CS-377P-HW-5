#include <math.h>
#include <pthread.h>   /*used in other parts of the assignment */
#include <stdatomic.h> /* used in other parts of the assignment */
#include <stdint.h>    /* for uint64  */
#include <stdio.h>
#include <stdint.h> /* for uint64  */
#include <time.h>   /* for clock_gettime */
#include <stdatomic.h>   /*used in other parts of the assignment */

#define MAX_THREADS 8
int threadArg[MAX_THREADS];
pthread_t handles[MAX_THREADS];

void *part3(void *);

int points;
double step3;
double globalSum = 0.0;

pthread_mutex_t globalSum_lock;

// for part 6
double sum[MAX_THREADS];

// for part 8
pthread_barrier_t varBarrier;


double f_part1(double x)
{
    return (6.0 / sqrt(1 - x * x));
}

int part1()
{

    double pi = 0.0;

    uint64_t execTime; /*time in nanoseconds */
    struct timespec tick, tock;

    int numPoints = 1000000000;
    double step = 0.5 / numPoints;

    clock_gettime(CLOCK_MONOTONIC_RAW, &tick);

    double x = 0.0;
    for (int i = 0; i < numPoints; i++)
    {
        pi = pi + step * f_part1(x); // Add to local sum
        x = x + step;          // next x
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &tock);

    execTime = 1000000000 * (tock.tv_sec - tick.tv_sec) + tock.tv_nsec - tick.tv_nsec;

    printf("\n ----PART 1---- \n elapsed process CPU time = %llu nanoseconds\n", (long long unsigned int)execTime);

    printf("%.20f\n", pi);
    return 0;
}

//--------------------------------------------------------------


double f_unit_circle(double x)
{
    return (sqrt(1 - x * x));
}

//The top half of this circle can be written analytically as y = sqrt(1-x*x) for x between -0.5 and 0.5. 
int part2()
{
    double pi = 0.0;
    uint64_t execTime; /*time in nanoseconds */
    struct timespec tick, tock;


    int numPoints = 100;
    double step = 2.0 / numPoints;

    clock_gettime(CLOCK_MONOTONIC_RAW, &tick);

    double x = -1.0;
    for (double i = 0; i <  numPoints; i++)
    {
        pi = pi + step * f_unit_circle(x); // Add to local sum
        x += step;          // next x
    }
    
    clock_gettime(CLOCK_MONOTONIC_RAW, &tock);

    execTime = 1000000000 * (tock.tv_sec - tick.tv_sec) + tock.tv_nsec - tick.tv_nsec;

    printf("\n ----PART 2---- \n elapsed process CPU time = %llu nanoseconds\n", (long long unsigned int)execTime);

    printf("%.20f\n", pi * 2);
    return 0;
}

//--------------------------------------------------------------

//The top half of this circle can be written analytically as y = sqrt(1-x*x) for x between -0.5 and 0.5. 
void *part3(void *threadIdPtr) {
// {
//     double pi = 0.0;
//     uint64_t execTime; /*time in nanoseconds */
//     struct timespec tick, tock;

//     int numPoints = 1000000000;
//     double step = 2.0 / numPoints;


    
    int myId = *(int*)threadIdPtr;
    for (int i =myId; i < points; i += MAX_THREADS)
    {
        double x = step3 * ((double)i) - 1;
        double value =  step3 * f_unit_circle(x); // Add to local sum
        globalSum = globalSum + value;         // next x
    }

    //pthread_exit(NULL);
   
    
}


void *part4(void *threadIdPtr) {
    
    int myId = *(int*)threadIdPtr;
    for (int i =myId; i < points; i += MAX_THREADS)
    {
        double x = step3 * ((double)i) - 1;
        double value =  step3 * f_unit_circle(x);
        pthread_mutex_lock(&globalSum_lock);
        globalSum = globalSum + value;         
        pthread_mutex_unlock(&globalSum_lock);
    }
    
    //pthread_exit(NULL);
    
}

void *part6(void *threadIdPtr) {
    int myId = *(int *)threadIdPtr;
    for (int i = myId; i < points; i+=MAX_THREADS) {
        double x = step3 * ((double) i) - 1; // next x
        sum[myId] = sum[myId] + step3*f_unit_circle(x); // Add to local sum
    }
    if(myId == 0) {
        globalSum += sum[0];
        for (int i=1; i< MAX_THREADS; i++) {
            globalSum += sum[i];
        }
    }
}

// this is the code for part 7 and 8
void *part7(void *threadIdPtr) {
    int myId = *(int *)threadIdPtr;
    double currSum = 0.0;
    for (int i = myId; i < points; i+=MAX_THREADS) {
        double x = step3 * ((double) i) - 1; // next x
        currSum = currSum + step3*f_unit_circle(x); // Add to local sum
    }
    //part 8 code
    //pthread_barrier_wait (&varBarrier);
    sum[myId] = currSum;
}

int main(int argc, char *argv[])
{
    //part1();
    part2();


   uint64_t execTime; /*time in nanoseconds*/
    struct timespec tick, tock;
    clock_gettime(CLOCK_MONOTONIC_RAW, &tick);

    points = 1000000000;
    step3 = 2.0 / points;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_mutex_init(&globalSum_lock, NULL);
    // part 3 code
    for(int i = 0; i < MAX_THREADS; i++) {
        // part 6 code will include sum[i] = 0;
        threadArg[i] = i;
        pthread_create(& handles[i], &attr, part4, &threadArg[i]);
    }


    // part 8 code
    //pthread_barrier_init (&varBarrier,NULL,MAX_THREADS);


    // part 4 code 
    // TODO figure out mutex lock init and where to do
    /*pthread_attr_t attr;
    pthread_attr_init(&attr);
    for(int i = 0; i < MAX_THREADS; i++) {
        threadArg[i] = i;
        pthread_create(& handles[i], &attr, part4, &threadArg[i]);
    }*/

    
    for (int i=0; i< MAX_THREADS; i++) {
        pthread_join(handles[i], NULL);
    }

    // part 6 and 7 code
    /*for (int i=0; i< MAX_THREADS; i++) {
        pthread_join(handles[i], NULL);
        pi += sum[i];
    }*/


    clock_gettime(CLOCK_MONOTONIC_RAW, &tock);
    execTime = 1000000000 * (tock.tv_sec - tick.tv_sec) + tock.tv_nsec - tick.tv_nsec;
    printf("\n ----PART 3---- \n elapsed process CPU time = %llu nanoseconds\n", (long long unsigned int)execTime);


    printf("thread %d, pi = %.20f\n", MAX_THREADS, globalSum * 2);
    //pthread_exit(NULL);
    //return 0;
}
    
