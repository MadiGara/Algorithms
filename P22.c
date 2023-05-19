/*
* Madison Gara (0973333)
* March 13th, 2023
* CIS*3490 A3
*/

#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#define BUFFER 4096
#define CHARS 127

int * shiftTable(char * pattern, int * table);
int horspool(char * array, char * pattern, int * table);

//print number of times a specified pattern occurs in the file and time to find it
int main() {
    int index = 0;
    int size = 0;
    char * array;
    char * filename;
    char * pattern = malloc(BUFFER);
    int * table = malloc(CHARS * sizeof(int));
    double time = 0.0;
    struct timespec t1, t2;
    FILE * file = NULL;
    int count = 0;

    filename = "data_A3_Q2.txt";

    file = fopen(filename, "r");
    if (file == NULL) {
        printf("Could not open the file %s.\n", filename);
        free(table);
        free(pattern);
        return -1;
    }

    //find size of file and malloc space for it
    fseek(file, 0L, SEEK_END);
    size = ftell(file);
    array = malloc(size * sizeof(char));
    fseek(file, 0L, SEEK_SET);

    printf("\nPlease enter the pattern you'd like to find: \n");
    scanf(" %s", pattern);
    printf("\n"); 

    //scan all characters in the file into an array
    while (index < size) {
        fscanf(file, "%c", &array[index]);
        index++;
    }

    if (clock_gettime(CLOCK_REALTIME, &t1) != 0) {
        fprintf(stderr, "Error: clock_gettime for t1 did not execute properly\n");
        free(table);
        free(array);
        free(pattern);
        fclose(file);
        return -1;
    }

    table = shiftTable(pattern, table);
    count = horspool(array, pattern, table);

    if (clock_gettime(CLOCK_REALTIME, &t2) != 0) {
        fprintf(stderr, "Error: clock_gettime for t2 did not execute properly\n");
        free(table);
        free(array);
        free(pattern);
        fclose(file);
        return -1;
    }

    //calculate difference between the time at start and end of algorithm
    time = (double)(t2.tv_sec - t1.tv_sec) + ((double)(t2.tv_nsec - t1.tv_nsec)/1000000000L);
    time *= 1000;

    printf("Pattern count: %d\n", count);
    printf("Execution time: %.2lf ms\n\n", time);

    free(array);
    free(pattern);
    free(table);
    fclose(file);

    return 0;
}

//establishes shift table for the given pattern
int * shiftTable(char * pattern, int * table) {
    int i, j, m;
    m = strlen(pattern);

    //initialize all alphabet characters to max length of pattern
    for (i = 0; i < CHARS; i++) { 
        table[i] = m;
    }

    //establish distance for letters in pattern
    for (j = 0; j < (m - 1); j++) {
        table[(int)pattern[j]] = m - 1 - j;
    }

    return table;
}

//implements horspool's algorithm for string matching
int horspool (char * array, char * pattern, int * table) {
    int count = 0;
    int numShifts = 0;
    int i, k, m, n;
    m = strlen(pattern);
    n = strlen(array);
    i = m - 1;  //pattern's right end

    while (i <= n - 1) {
        k = 0;
        //matched character found, increment character
        while ((k <= m - 1) && (pattern[m-1-k] == array[i - k])) {
            k++;
        }

        //whole pattern found - increment i by length of pattern
        if (k == m) {
            count++;
            i += m;
        }
        //else, increment by table value
        else {
            i += table[(int)array[i]];
            numShifts++;
        }
    }

    printf("Shifts: %d\n", numShifts);

    return count;
}