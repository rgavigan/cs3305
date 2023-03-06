/*
Riley Emma Gavigan
Student #: 251150776
Student ID: rgavigan
CS 3305B - Assignment 3
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

// Global count and sum variables for Grand Sum and Grand Count
long long globalCount = 0;
long long globalSum = 0;

// Struct to hold thread data to be passed to countPrimes function
typedef struct {
    int start;
    int end;
    int curThread;
} thread_data;

/* Function to calculate number of primes from start to end in range of numbers
Uses a simple primarily test for determining if a number is prime, not Sieve of Eratosthenes because that requires the range 2 -> n, whereas these threads independently calculate ranges.
*/
void * countPrimes(void * data) {
    // Get start, end, and thread number from input array
    thread_data *info = data;
    int start = info->start;
    int end = info->end;
    int curThread = info->curThread;

    // Initialize count and sum variables for thread
    long long count = 0;
    long long sum = 0;

    // Print out Thread information
    printf("Thread # %d is finding primes from low = %d to high = %d\n", curThread, start, end);

    // Get the primes up to N for the interval [start, end]
    int i, j, flag;
    for (i = start + 1; i <= end; i++) {
        // 0 and 1 are not prime
        if (i == 1 || i == 0) {
            continue;
        }
        // Flag for primality test
        flag = 1;

        // Loop to check if prime
        for (j = 2; j <= i / 2; j++) {
            // i is not a prime number, set flag to 0 and break
            if (i % j == 0) {
                flag = 0;
                break;
            }
        }

        // If prime, add to count / sum
        if (flag == 1) {
            count++;
            sum += i;
        }
    }

    // Store the number of primes and their sum in the list
    globalCount += count;
    globalSum += sum;

    // Print out thread information
    printf("Sum of thread %d is %lld, Count is %lld\n", curThread, sum, count);
    free(info);
}

int main(int argc, char * argv[]) {
    // Inform user how to use the program if they don't use it correctly
    if (argc != 3) {
        printf("Proper usage is ./assignment-3 <threadCount> <highestInt>\n");
        return 1;
    }
    // Accept number of threads and max number to calculate primes up to (denoted N) from command line
    int numThreads = atoi(argv[1]);
    int N = atoi(argv[2]);

    // Create array of threads and initialize start / end variables for range
    pthread_t threads[numThreads];
    int start = 0;
    int end = (N + numThreads - 1) / numThreads;

    // Create the threads and call countPrimes on their range
    for (int i = 0; i < numThreads; i++) {
        // Add thread data to thread_data struct through a pointer
        thread_data *info = malloc(sizeof(thread_data));
        info->start = start;
        info->end = end;
        info->curThread = i;

        // Create the thread and call countPrimes, checking if successful
        if(pthread_create(&threads[i], NULL, countPrimes, info)) {
            printf("Error with creating thread\n");
            return 1;
        }

        // Update the start and end values for the next thread
        start = end;
        end = start + ((N + numThreads - 1) / numThreads);
        // Don't go past N
        if (end > N) {
            end = N;
        }
    }
    // Loop through threads and join them
    for (int i = 0; i < numThreads; i++) { 
        // Wait for the threads to finish
        if(pthread_join(threads[i], NULL)) {
            printf("Error with joining thread\n");
            return 1;
        }
    }
    // Print final sum and count
    printf("\n        GRAND SUM IS %lld, COUNT IS %lld\n", globalSum, globalCount);
    return 0;
}
