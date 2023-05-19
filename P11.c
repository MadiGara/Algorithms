/*
* Madison Gara
* March 27th, 2023
*/

#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

#define NUM_WORDS 2045
#define BUFFER 4096
#define MAX_TREE 100

typedef struct WordFreq {
    char * word;
    int count;
    double probability, compars;
    struct WordFreq * left, * right;
} WordFreq;

void sort (int numUniqueWords, WordFreq ** words);
WordFreq * dynamicBST(WordFreq ** words, int numUniqueWords);
WordFreq * insert (WordFreq ** words, double ** avgCompars, int ** rootIndex, int start, int end);
void inorder(WordFreq * root);
WordFreq * search (WordFreq * root, char * key);

//print search key, average comparisons to reach it, and which subtree to take
int main() {
    char * key = malloc(BUFFER);
    FILE * file = NULL;
    int uniqueWords = 0;
    WordFreq * words[NUM_WORDS];
    char temp[BUFFER];
    int i;
    WordFreq * root = NULL;

    char filename[] = "data_A4_Q1.txt";

    file = fopen(filename, "r");
    if (file == NULL) {
        printf("Could not open the file %s.\n", filename);
        return -1;
    }

    /* get frequencies for the unique words in the file */

    //read each unique word in the file
    while ((uniqueWords < NUM_WORDS) && (fscanf (file, " %s", temp) == 1)) {

        for (i = 0; i < uniqueWords; i++) {
            //get index of word if it exists
            if (strcmp(words[i]->word, temp) == 0) {
                break;
            }
        }

        //update frequency of existing word
        if (i < uniqueWords) {
            words[i]->count++;
            continue;
        }

        //otherwise - new word, initialize it
        words[uniqueWords] = malloc(sizeof * words[uniqueWords]);
        words[uniqueWords]->word = malloc(strlen(temp) + 1);
        words[uniqueWords]->count = 0;
        words[uniqueWords]->probability = 0.0;

        strcpy(words[uniqueWords]->word, temp);
        words[uniqueWords]->count++;
        uniqueWords++;
    }

    sort(uniqueWords, words);

    for (int i = 0; i < uniqueWords; i++) {
        words[i]->probability = (double) words[i]->count / (double) NUM_WORDS;
    }

    root = dynamicBST(words, uniqueWords);

    printf("\nPlease enter a key:\n");
    scanf(" %s", key);
    printf("\n"); 

    search(root, key);

    free(key);
    for (int k = 0; k < uniqueWords; k++) {
        free(words[k]);
    }
    fclose(file);

    return 0;
}

//helper function to sort the words alphabetically
void sort (int numUniqueWords, WordFreq ** words) {
    int i, j;
    WordFreq * temp;

    for (i = 0; i < numUniqueWords - 1; i++) {
        for (j = i + 1; j < numUniqueWords; j++) {
            if (strcmp(words[i]->word, words[j]->word) > 0) {
                temp = words[i];
                words[i] = words[j];
                words[j] = temp;
            }
        }
    }
}

//uses dynamic programming to create an optimal BST for the unique words 
WordFreq * dynamicBST(WordFreq ** words, int numUniqueWords) {
    int i, j, k, l, kmin, diagonal; 
    double min;
    double sum = 0.0;
    double ** avgCompars = malloc(sizeof(double *) * (numUniqueWords+1));
    int ** rootIndex = malloc(sizeof(int *) * (numUniqueWords+1));
    for (int m = 0; m < (numUniqueWords+1); m++) {
        avgCompars[m] = malloc(sizeof(double) * (numUniqueWords+1));
        rootIndex[m] = malloc(sizeof(int) * (numUniqueWords+1));
    }

    //build the average number and rootIndex tables - from pg. 302 of the text as permitted
    for (i = 0; i < numUniqueWords; i++) {
        avgCompars[i][i] = 0.0;
        avgCompars[i][i] = words[i]->probability;
        rootIndex[i][i] = i;
    }
    avgCompars[numUniqueWords][numUniqueWords] = 0.0;

    for (diagonal = 0; diagonal < numUniqueWords; diagonal++) {
        for (i = 0; i < numUniqueWords - diagonal; i++) {
            j = i + diagonal;
            min = (double) INT_MAX;

            for (k = i; k <= j; k++) {
                if ((avgCompars[i][k-1] + avgCompars[k+1][j]) < min) {
                    min = avgCompars[i][k-1] + avgCompars[k+1][j];
                    kmin = k;
                }
            }
            rootIndex[i][j] = kmin;

            sum = words[i]->probability;

            for (l = i + 1; l <= j; l++) {
                sum += words[l]->probability;
            }
            avgCompars[i][j] = min + sum;
        }
    }

    //build the binary tree from the tables given
    WordFreq * root = NULL;
    int start = 0; int end = j; 
    root = insert(words, avgCompars, rootIndex, start, end);

    //inorder(root);

    for (int m = 0; m < (numUniqueWords+1); m++) {
        free(avgCompars[m]);
        free(rootIndex[m]);
    }
    free(avgCompars);
    free(rootIndex);

    return root;
}

//helper function to insert node with a given word into the OBST - root is of the subtree
WordFreq * insert (WordFreq ** words, double ** avgCompars, int ** rootIndex, int start, int end) {
    
    WordFreq * root;
    if (start > end) {
        root = NULL;
        return root;
    }

    root = malloc(sizeof(struct WordFreq));
    root->word = words[rootIndex[start][end]]->word;
    root->left = NULL;
    root->right = NULL;
    root->compars = avgCompars[start][end];

    //the nodes of the obst are the values in the root table, ie. the indexes in the array of unique words
    root->left = insert(words, avgCompars, rootIndex, start, rootIndex[start][end]-1);
    root->right = insert(words, avgCompars, rootIndex, rootIndex[start][end]+1, end);

    return root;
} 

//helper function to print bst inorder
void inorder(WordFreq * root) {
    if (root != NULL) {
        inorder(root->left);
        printf("%-20s %lf comparisons\n", root->word, root->compars);
        inorder(root->right);
    }
}

//helper function to search the tree for the key
WordFreq * search (WordFreq * root, char * key) {
    if (root == NULL) {
        printf("%s not found.\n\n", key);
        return root;
    }

    //compare key to words in tree
    int isEqual = strcmp(key, root->word);
    if (isEqual == 0) {
        printf("Compared with %s (%.3lf), found.\n\n", root->word, root->compars);
    }
    //left subtree
    else if (isEqual < 0) {
        printf("Compared with %s (%.3lf), go left subtree.\n", root->word, root->compars);
        root->left = search(root->left, key);
    }  
    //right subtree
    else {
        printf("Compared with %s (%.3lf), go right subtree.\n", root->word, root->compars);
        root->right = search(root->right, key);
    }

    return root;
}
