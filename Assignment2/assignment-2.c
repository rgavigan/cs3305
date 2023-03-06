// Name: Riley Emma Gavigan
// Student Number: 251150776
// Student ID: rgavigan
// CS 3305 - Assignment 2

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

// Function to write two integer values to the pipe for child to read
void writeToChild(int a, int b, int port[2]) {
    printf("Parent (PID %d): Sending %d to child\n", getpid(), a);
    printf("Parent (PID %d): Sending %d to child\n", getpid(), b);
    // Write a and b to the pipe
    if(write(port[1], &a, sizeof(int)) == -1 || write(port[1], &b, sizeof(int)) == -1) {
        printf("Error when writing\n");
        exit(EXIT_FAILURE);
    }
}

// Function to read 2 integers from pipe and write result to the pipe
void readMultiplyAndWrite(int port[2], int port2[2]) {
    int a, b;
    // Read a and b from the pipe
    if(read(port[0], &a, sizeof(int)) == -1 || read(port[0], &b, sizeof(int)) == -1) {
        printf("Error when reading\n");
        exit(EXIT_FAILURE);
    }
    printf("        Child (PID %d): Received %d from parent\n", getpid(), a);
    printf("        Child (PID %d): Received %d from parent\n", getpid(), b);

    // Write a * b to the pipe
    int result = a * b;
    if(write(port2[1], &result, sizeof(int)) == -1) {
        printf("Error when writing\n");
        exit(EXIT_FAILURE);
    };
    printf("        Child (PID %d): Sending %d from parent\n", getpid(), result);
}

// Function to read result from pipe in parent
int readFromChild(int port[2]) {
    int result;
    // Read a * b from the pipe
    if(read(port[0], &result, sizeof(int)) == -1) {
        printf("Error when reading\n");
        exit(EXIT_FAILURE);
    }
    printf("Parent (PID %d): Received %d from child\n", getpid(), result);
    return result;
}

int main(int argc, char *argv[]) {

    // Read two parameters from the command line as strings
    char *int1 = argv[1];
    char *int2 = argv[2];
    if (argc < 2 || argc > 3 || strlen(int1) != 4 || strlen(int2) != 4) {
        printf("Proper usage is ./assignment-2 <4 digit integer> <4 digit integer>\n");
        exit(EXIT_FAILURE);
    }


    // Convert the strings to integers
    int num1 = atoi(int1);
    int num2 = atoi(int2);
    printf("Your integers are %d and %d\n", num1, num2);

    // Partition the two numbers into a1, a2, b1, b2
    int a1 = num1 / 100;
    int a2 = num1 % 100;
    int b1 = num2 / 100;
    int b2 = num2 % 100;
    
    // Establish pipes and then fork a child process
    int port[2]; // Parent -> child
    int port2[2]; // Child -> parent

    // Make pipes and check for error
    if (pipe(port) < 0 || pipe(port2) < 0) {
        perror("Pipe error\n");
        exit(EXIT_FAILURE);
    }
    int pid = fork();
    // Check for forking error
    if (pid < 0) {
        perror("Fork error\n");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0) {
        printf("Parent (PID %d): created child (PID %d)\n\n", getpid(), pid);
    }
    
    int A, B, C, D, X, Y, Z;
    // In child process
    if (pid == 0) {
        // Close read end of port2, write end of port
        close(port2[0]);
        close(port[1]);

        // a1 * b1 = A
        readMultiplyAndWrite(port, port2);
        // a2 * b1 = B
        readMultiplyAndWrite(port, port2);
        // a1 * b2 = C
        readMultiplyAndWrite(port, port2);
        // a2 * b2 = D
        readMultiplyAndWrite(port, port2);
        
        // Exit child process
        exit(0);
    }
    // Parent:
    else if (pid > 0) {
        // Close read end of port, write end of port2
        close(port[0]);
        close(port2[1]);

        // Send a1 and b1 to pipe, then read A after multiplication
        printf("###\n# Calculating X\n###\n");
        writeToChild(a1, b1, port);
        A = readFromChild(port2);
        printf("\n");
        
        // Send a2 and b1 to pipe, then read B after multiplication
        printf("###\n# Calculating Y\n###\n");
        writeToChild(a2, b1, port);
        B = readFromChild(port2);

        // Send a1 and b2 to pipe, then read C after multiplication
        writeToChild(a1, b2, port);
        C = readFromChild(port2);
        printf("\n");

        // Send a2 and b2 to pipe, then read D after multiplication
        printf("###\n# Calculating Z\n###\n");
        writeToChild(a2, b2, port);
        D = readFromChild(port2);
        printf("\n");

        // Calculate X, Y, Z, and their sum
        X = A * pow(10, 4);
        Y = (B + C) * pow(10, 2);
        Z = D;
        int sum = X + Y + Z;

        // Print the result
        printf("%d*%d == %d + %d + %d == %d\n", num1, num2, X, Y, Z, sum);
    }

    return 0;
}