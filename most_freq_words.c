/* Filename: most_freq_words.c
 * Author: Gary Atwal
 * Project: Picovoice Screening Questions
 *
 * Description: 
 * Find the n most frequent words in the TensorFlow Shakespeare dataset
 * 
 * Assumption:
 * 1) User has downloaded .txt file and is local to source code or can specify
 *    full filepath as command line argument to main function
 * 2) A word is defined as characters containing a-z,A-Z:
 *    i) no difference between uppercase and lowercase characters (e.g. There
 *       and there are considered the same word)
 *   ii) Apostrophes in middle  or end of word (not first character) is allowed
 *       and considered 1 word (e.g. can't is read as a single word)
 * 
 * Solution:
 * 1) Implement hash table such that:
 *    key = word
 *    value = frequency
 * 2) Once entire file is read, use quick sort algo to sort hash table array
 * 3) Return top n most frequent words from sorted hash table
 * 
 * Reference:
 * 1) https://storage.googleapis.com/download.tensorflow.org/data/shakespeare.txt
 * 2) http://www.cse.yorku.ca/~oz/hash.html (hash function)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#define HASH_TABLE_SIZE 10000
#define WORD_BUFFER_SIZE 150

/*******************************************************************************
 * HASH TABLE DEFINITION
 *******************************************************************************/
/* Hash table is an array of linked lists */
/* Linked list to deal with collisions of same hash key*/
typedef struct WordFreqNode {
    char* word;
    int count;
    struct WordFreqNode *next;
} WordFreqNode;

/* Use DJB2 Hash Function */
unsigned int djb2_hash(const char* word){
    unsigned int hash = 5381;
    int c;
    while (c = *word++){
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash % HASH_TABLE_SIZE;
}

/* Create a new WordFreqNode */
WordFreqNode* create_WordFreqNode(const char *word){
    WordFreqNode *new_node = (WordFreqNode*)malloc(sizeof(WordFreqNode));
    if (!new_node) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }
    new_node->word = strdup(word);
    new_node->count = 1;
    new_node->next = NULL;
    return new_node;
}

/* Insert word or update frequency count */
void add_word(WordFreqNode **table, const char *word){
    unsigned int search_key = djb2_hash(word);
    WordFreqNode *node = table[search_key];

    /* Check if word is already in hash table */
    while (node != NULL){
        if (strcmp(node->word, word) == 0){
            node->count++;
            return;
        }
        node = node->next;
    }

    /* Else if not found, add to hash table at top of list */
    WordFreqNode *new_node = create_WordFreqNode(word);
    new_node->next = table[search_key];
    table[search_key] = new_node;
}

/* Comparison function for quick sort algorithm */
int compare_by_frequency(const void *a, const void *b){
    WordFreqNode *node_a = *(WordFreqNode **)a;
    WordFreqNode *node_b = *(WordFreqNode **)b;
    return node_b->count - node_a->count; // sort high to low
}

/* Free hash table dynamic memory 
void free_hash_table(WordFreqNode **table){
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        WordFreqNode *node = table[i];
        while (node) {
            WordFreqNode *temp = node;
            node = node->next;
            free(temp->word);  // Free the word string
            free(temp);         // Free the WordFreq node
        }
    }
} */

/*******************************************************************************
 * SOLUTION
 *******************************************************************************/

/* Function below can be refactored into multiple functions */
char **find_frequent_words(const char *path, int32_t n){
    FILE *file = fopen(path, "r");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }
    
    /* Read in file, a character at a time, discard non {a-z,A-Z} characters
       Consider apostrophes such as the word know't that appears in Shakespeare
       as a single word. Upper case and lower case are treated the same */
    WordFreqNode *hash_table[HASH_TABLE_SIZE] = {0};
    char word_buffer[WORD_BUFFER_SIZE];
    int c;
    int pos = 0; // position in word_buffer

    /* Read the file and allocate the hash table */
    while ((c = fgetc(file)) != EOF){
        if (isalpha(c) || (c == '\'' && pos > 0)){
            /* Prevent buffer overflow */
            if (pos < WORD_BUFFER_SIZE - 1){
                word_buffer[pos] = tolower(c);
                pos++;
            }
        }
        else {
            /* not valid word character */
            if (pos > 0){
                /* valid word in buffer, add to hash table/update count */
                word_buffer[pos] = '\0';
                add_word(hash_table, word_buffer);
                pos = 0; // reset to start of buffer to read next word
            }
        }
    }

    /* Reached end of file but last word might be in buffer */
    if (pos > 0){
        word_buffer[pos] = '\0';
        add_word(hash_table, word_buffer);
    }

    fclose(file);

    /* Translate the WordFreqNodes in the hash table into an array of WordFreqNodes
       so that qsort algorithm can be used */
    WordFreqNode *word_linked_list[HASH_TABLE_SIZE];
    int count = 0;

    /* iterate through each hash table index */
    for (int i = 0; i < HASH_TABLE_SIZE; i++){
        WordFreqNode *node = hash_table[i];
        while (node){
            /* iterate through linked list in hash table*/
            word_linked_list[count] = node;
            count++;
            node = node->next;
        }
    }

    /* Sort array by word count */
    qsort(word_linked_list, count, sizeof(WordFreqNode *), compare_by_frequency);

    /* Gather results */
    char **result = malloc(n * sizeof(char *));
    if (!result){
        perror("Failed to allocate memory");
        // free_hash_table(hash_table);
        /* Free result array */
        return NULL;
    }

    /* Top n frequent words */
    for (int i = 0; i < n; i++) {
        if (i < count) {
            result[i] = strdup(word_linked_list[i]->word);
            if (!result[i]) {
                perror("Failed to allocate memory");
                // Free the result array
                //for (int j = 0; j < i; ++j) {
                //    free(result[j]);
                //}
                free(result);
                // Free the hash table
                //free_hash_table(hash_table);
                return NULL;
            }
        }
    }

    //free_hash_table(hash_table);
    return result;
}

/*******************************************************************************
 * MAIN FUNCTION
 *******************************************************************************/
int main(int argc, char *argv[]){
    // Default values
    const char *default_filepath = "shakespeare.txt";
    int32_t default_n = 20;

    // Use command-line arguments if provided
    const char *filepath = (argc > 1) ? argv[1] : default_filepath;
    int n = (argc > 2) ? atoi(argv[2]) : default_n;

    // Validate the value of n
    if (n <= 0) {
        fprintf(stderr, "The value of n must be a positive integer.\n");
        return EXIT_FAILURE;
    }

    // Call the function to get the most frequent words
    char **frequent_words = find_frequent_words(filepath, n);

    if (!frequent_words) {
        fprintf(stderr, "Failed to retrieve the most frequent words.\n");
        return EXIT_FAILURE;
    }

    // Print the results
    printf("Top %d most frequent words:\n", n);
    for (int i = 0; i < n; i++) {
        if (frequent_words[i]) {
            printf("%d: %s\n", i + 1, frequent_words[i]);
            free(frequent_words[i]);  // Free each string
        }
    }
    free(frequent_words);  // Free the array itself

    return EXIT_SUCCESS;
}