/*
Name: Riley Emma Gavigan
ID Number: 251150776
Student ID: rgavigan
CS 3305B - Assignment 4 - CPU Scheduling Algorithms
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Create struct to hold process ID (String) and burst time (Integer)
struct Process {
    char processID[10];
    int burstTime;
    int arrivalTime;
    int waitTime;
    int turnaroundTime;
    int numBursts;
    int remainingTime;
};

// Method to print out the processes waiting time, turnaround, and total average
void printWaitAndTurnaround(struct Process processes[], int numLines) {
    // Store total waiting time and total turnaround time
    float totalWaitTime = 0;
    float totalTurnaroundTime = 0;

    // Go through the processes array to print each processes wait and turnaround time
    for (int i = 0; i < numLines; i++) {
        printf("\n%s\n", processes[i].processID);
        printf("      Waiting time:         %d\n", processes[i].waitTime);
        printf("      Turnaround time:      %d\n", processes[i].turnaroundTime);
        totalWaitTime += processes[i].waitTime;
        totalTurnaroundTime += processes[i].turnaroundTime;
    }
    printf("\nTotal average waiting time:      %0.1f\n", totalWaitTime / numLines);
    printf("Total average turnaround time:    %0.1f\n", totalTurnaroundTime / numLines);
}

/*
Method to run the FCFS algorithm
Takes in the array of processes structures and the number of lines in the file
Outputs details about the processes and the average wait and turnaround times
*/
void runFCFS(struct Process processes[], int numLines) {
    printf("First Come First Served\n");

    // Set the time and wait time to 0
    int t = 0;
    int waitTime = 0;

    // Go through the processes array for each second of time and print out the PID, its burst time, wait time, turnaround time
    for (int i = 0; i < numLines; i++) {
        // Get the burst time
        int burstTime = processes[i].burstTime;

        // Set the process wait time
        processes[i].waitTime = waitTime;
        // Set the process turnaround time
        processes[i].turnaroundTime = burstTime + waitTime;

        // While burst time is not 0 (no pre-emption)
        for (int j = burstTime; j > 0; j--) {
            // Print out the time, PID, wait time, burst time
            printf("T%d : %s - Burst left  %d, Wait time  %d, Turnaround time  %d\n", t, processes[i].processID, j, waitTime, (burstTime + waitTime));
            t += 1;
        }

        // Update the wait time for the next process
        waitTime += burstTime - 1;
    }

    // Print out the final process and total results
    printWaitAndTurnaround(processes, numLines);
}


// Method to run the SJF algorithm
void runSJF(struct Process processes[], int numLines, int totalTime) {
    printf("Shortest Job First\n");

    // Loop through all time units
    for (int t = 0; t < totalTime; t++) {
        // Select the shortest burst time process that has arrived (t >= arrivalTime)
        int shortestBurstTime = 100000;
        int shortestIndex = 0;
        for (int i = 0; i < numLines; i++) {
            if (processes[i].burstTime <= shortestBurstTime && processes[i].remainingTime > 0 && t >= processes[i].arrivalTime) {
                shortestBurstTime = processes[i].remainingTime;
                shortestIndex = i;
            }
        }

        // Go through all other processes and increment their wait time if they have not arrived yet
        for (int i = 0; i < numLines; i++) {
            if (i != shortestIndex && processes[i].remainingTime > 0 && t >= processes[i].arrivalTime) {
                processes[i].waitTime += 1;
            }
        }

        processes[shortestIndex].turnaroundTime = 1 + processes[shortestIndex].waitTime + processes[shortestIndex].burstTime - processes[shortestIndex].remainingTime;

        // Print out the time, PID, wait time, burst time
        printf("T%d : %s - Burst left  %d, Wait time  %d, Turnaround time  %d\n", t, processes[shortestIndex].processID, processes[shortestIndex].remainingTime, processes[shortestIndex].waitTime, processes[shortestIndex].turnaroundTime);

        // Decrement the burst time
        processes[shortestIndex].remainingTime -= 1;
    }

    // Print out the final process and total results
    printWaitAndTurnaround(processes, numLines);
}


// Method to run the RR algorithm with the given time quantum
void runRR(struct Process processes[], int numLines, int timeQuantum) {
    printf("Round Robin with Quantum %d\n", timeQuantum);

    // Set the time to 0
    int t = 0;
    // Set the wait time to 0
    int waitTime = 0;

    // Go through the processes array for each second of time and print out the PID, its burst time, wait time, turnaround time
    while(1) {
        // Variable to check for number of finished processes
        int numZeroBursts = 0;

        // Go through each process
        for (int i = 0; i < numLines; i++) {
            // Still burst time remaining in process
            if (!(processes[i].burstTime <= 0)) {
                // Set the process wait time (wait time - previous burst time - arrival time)
                processes[i].waitTime = waitTime - (timeQuantum * (processes[i].numBursts)) - processes[i].arrivalTime;
                // Set the process turnaround time (wait time + completed burst time)
                processes[i].turnaroundTime = (timeQuantum * (processes[i].numBursts)) + processes[i].waitTime;

                // While RR burst time is not 0 (no pre-emption)
                int burstTracker = 0;
                for (int j = timeQuantum; j > 0; j--) {
                    burstTracker += 1;

                    // Print out the time, PID, wait time, burst time
                    printf("T%d : %s - Burst left  %d, Wait time  %d, Turnaround time  %d\n", t, processes[i].processID, processes[i].burstTime, processes[i].waitTime, processes[i].turnaroundTime);

                    // Increment the time and decrement the burst time remaining
                    t += 1;
                    processes[i].burstTime -= 1;

                    // Break if the burst time runs out for the processes
                    if (processes[i].burstTime == 0) {
                        numZeroBursts += 1;
                        processes[i].turnaroundTime += 1;
                        break;
                    }

                    // Increment the turnaround time
                    processes[i].turnaroundTime += 1;
                }

                // Update the basic wait time of time quantums completed or remaining burst completed
                waitTime += burstTracker;

                // Update the number of previous bursts completed
                processes[i].numBursts += 1;
            }
            // Simply increment number of zero bursts for checking that all processes are done
            else {
                numZeroBursts += 1;
            }
        }

        // Check if all bursts are down to 0 (done)
        if (numZeroBursts >= numLines) {
            break;
        }
    }

    // Print out the final process and total results
    printWaitAndTurnaround(processes, numLines);
}


/*
Main function that takes 2 or 3 parameters (Algorithm simulation)
Param 1/2: -f = FCFS Algorithm. -s = SJF Algorithm. -r <integer> = RR Algorithm.
Param 2/3: Filename to read input from (assignment-4-input.csv)
*/
int main(int argc, char *argv[]) {
    // Check if the number of parameters is correct
    if (argc != 3 && argc != 4) {
        printf("Proper usage is ./assignment-4 [-f|-s|-r <quantum>] <Input file name>\n");
        return 1;
    }

    // Create integer to hold algorithm type (0 = FCFS, 1 = SJF, 2 = RR)
    int algorithmType = 0;
    // Create integer to hold time quantum if RR algorithm is used
    int timeQuantum = -1;

    // Check the first parameter to see which algorithm to run
    if (strcmp(argv[1], "-f") == 0) {
        algorithmType = 0;
    } else if (strcmp(argv[1], "-s") == 0) {
        algorithmType = 1;
    } else if (strcmp(argv[1], "-r") == 0) {
        algorithmType = 2;
        // Check if the time quantum is valid
        if (atoi(argv[2]) > 0) {
            timeQuantum = atoi(argv[2]);
        } else {
            printf("Error: Invalid time quantum given. Give a postive integer\n");
            return 1;
        }
    } else {
        printf("Error: Invalid algorithm type\n");
        return 1;
    }

    // Get the second parameter of the filename
    FILE *file;
    if (algorithmType != 2) {
        // Check if the file exists
        file = fopen(argv[2], "r");
        if (file == NULL) {
            printf("Could not open %s\n", argv[2]);
            return 1;
        }
    } else {
        // Check if the file exists
        file = fopen(argv[3], "r");
        if (file == NULL) {
            printf("Could not open %s\n", argv[3]);
            return 1;
        }
    }

    // Get the number of lines from the input file
    int numLines = 0;
    char c;
    for (c = getc(file); c != EOF; c = getc(file)) {
        if (c == '\n') {
            numLines++;
        }
    }
    // Reset the file pointer to the beginning of the file
    rewind(file);

    // Create an array of Process structures to hold the information from the input file
    struct Process processes[numLines];
    // Create a string to hold the line read from the file
    char line[100];
    // Create a string to hold the process ID
    char processID[10];
    // Create an integer to hold the burst time and one to hold arrival time
    int burstTime;
    int arrivalTime;

    /*
    Read the input file into the array of Process structures with the process ID and burst time stored
    Line Format: Process ID, Burst Time
    */
    int i = 0;
    int totalTime = 0;
    while (fgets(line, sizeof(line), file)) {
        // Get the process ID
        char *token = strtok(line, ",");
        strcpy(processID, token);

        // Get the burst time
        token = strtok(NULL, ",");
        burstTime = atoi(token);
        totalTime += burstTime;

        // Set the arrival time
        arrivalTime = i;

        // Store the process ID, burst time, and arrival time in the array of Process structures
        strcpy(processes[i].processID, processID);
        processes[i].burstTime = burstTime;
        processes[i].remainingTime = burstTime;
        processes[i].arrivalTime = arrivalTime;
        processes[i].waitTime = 0;
        processes[i].turnaroundTime = 0;
        processes[i].numBursts = 0;
        i++;
    }

    // Close the file
    fclose(file);

    // Run the algorithm based on the algorithm type
    if (algorithmType == 0) {
        runFCFS(processes, numLines);
    } else if (algorithmType == 1) {
        runSJF(processes, numLines, totalTime);
    } else if (algorithmType == 2) {
        runRR(processes, numLines, timeQuantum);
    }
}
