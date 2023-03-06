/*
Name: Riley Emma Gavigan
Student ID: 251150776
UWO ID: rgavigan
Assignment 1 - CS 3350B - Password Cracking W/ Parent & Child Processes
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "checkPassword.h"

// Function to test possible 3-character combinations
void testThreeChars(int start) {
    int minASCII = 33;
    int maxASCII = 126;
    for (int i = minASCII; i <= maxASCII; i++) {
        for (int j = minASCII; j <= maxASCII; j++) {
            for (int k = minASCII; k <= maxASCII; k++) {
                // Create password guess and check it
                char password[4] = {i, j, k, '\0'};
                int result = checkPassword(password, start);
                if (result == 0) {
                    // Password found
                    printf("%s\n", password);
                    fflush(stdout);
                }
            }
        }
    }
}

// Main function to find the password
int main(int argc, char *argv[]) {
    // Check CLA and update flags depending on if forking/execl will be used
    int forkFlag = 0;
    int execlFlag = 0;
    for (int i = 0; i < argc; i++) {
        printf("Argument: %s\n", argv[i]);
        if (strcmp(argv[i], "-f") == 0) {
            forkFlag = 1;
        }
        if (strcmp(argv[i], "-p") == 0) {
            execlFlag = 1;
        }
    }

    int child2, child1, child3;
    // Forking is not requested
    if (forkFlag == 0) {
        // Call testThreeChars on 3 characters at a time for entire password
        testThreeChars(0);
        testThreeChars(3);
        testThreeChars(6);
        testThreeChars(9);
    }
    // Forking is requested
    else {
        // Create 3 children forks
        child2 = fork();
        if (child2 != 0) {
            child1 = fork();
        }
        else if (child2 == 0) {
            child3 = fork();
        }

        // Child 2 branch
        if (child2 == 0) {
            // Inside child 3, test 9-11
            if (child3 == 0) {
                printf("PID 1.1 IS %d. CHILD 1.1.1 IS %d\n", getppid(), getpid());
                printf("PID 1.1.1 IS %d, PPID 1.1 IS %d\n", getpid(), getppid());
                testThreeChars(9);
            }
            // Inside child 2, test 6-8
            else {
                printf("PID 1.0 IS %d. CHILD 1.1 IS %d\n", getppid(), getpid());
                printf("PID 1.1 IS %d. PPID 1.0 IS %d\n", getpid(), getppid());
                testThreeChars(6);
                // Wait for child 3 to finish
                waitpid(child3, NULL, 0);
            }
        }
        else {
            // Inside child 1, test 3-5
            if (child1 == 0) {
                printf("PID 1.0 IS %d. CHILD 1.2 IS %d\n", getppid(), getpid());
                printf("PID 1.2 IS %d, PPID 1.0 IS %d\n", getpid(), getppid());
                testThreeChars(3);
            }
            // Inside main process, test 0-2
            else {
                testThreeChars(0);
                // Wait for child 1 and 2 to finish
                waitpid(child1, NULL, 0);
                waitpid(child2, NULL, 0);
            }
        }
    }

    // In parent branch, run hackme if execlFlag is set
    if (child2 != 0 && child1 != 0) {
        if (execlFlag == 1) {
            execl("./hackme", "hackme", NULL);
        }
        else {
            return 0;
        }
    }
}