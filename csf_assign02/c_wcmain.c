#include <stdio.h>
#include <stdint.h>
#include "wcfuncs.h"

// Suggested number of buckets for the hash table
#define HASHTABLE_SIZE 13249

// TODO: prototypes for helper functions

int main(int argc, char **argv) {
  // stats (to be printed at end)
  uint32_t total_words = 0;
  uint32_t unique_words = 0;
  const unsigned char *best_word = (const unsigned char *) "";
  uint32_t best_word_count = 0;

  // handle error and return nonzero value if
  // more than two arguments are supplied
  FILE *input;
  
  // handle diffeerent argument numbers and opening file
  if (argc > 2) {
    fprintf(stderr, "Too many arguments supplied");
    return -1;
  } else if (argc < 2) {
    input = stdin;
  } else {
    input = fopen(argv[1], "r");
  }

  // handle error and return nonzero value if
  // the input file is invalid
  if (input == NULL) {
    fprintf(stderr, "Invalid input file");
    return -2;
  }

  struct WordEntry *ht[HASHTABLE_SIZE] = { NULL }; // hash table

  unsigned char word[MAX_WORDLEN + 1];

  // add words to hash table
  while (wc_readnext(input, word)) {
    struct WordEntry *insert;
    wc_tolower(word);
    wc_trim_non_alpha(word);
    insert = wc_dict_find_or_insert(ht, HASHTABLE_SIZE, word);
    ++insert->count;
  }

  // collect counts
  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    struct WordEntry *head = ht[i];
    while (head) {
      total_words += head->count; // update total word count
      unique_words += 1; // update unique word count

      // update best word count and best word
      if (head->count > best_word_count) {
        best_word_count = head->count;
        best_word = head->word;
      } 
      // if best counts are equal, set the best word to 
      // the candidate that compares least lexicographically
      else if (head ->count == best_word_count) {
        if (wc_str_compare(head->word, best_word) == -1)
          best_word = head->word;
      }
      head = head->next; // move to next element
    }
  }

  printf("Total words read: %u\n", (unsigned int) total_words);
  printf("Unique words read: %u\n", (unsigned int) unique_words);
  printf("Most frequent word: %s (%u)\n", (const char *) best_word, best_word_count);

  fclose(input); // close input file

  // free memory
  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    wc_free_chain(ht[i]);
  }
  return 0;
}

int wc_str_compare(const unsigned char *lhs, const unsigned char *rhs);
int wc_readnext(FILE *in, unsigned char *w);
void wc_tolower(unsigned char *w);
void wc_trim_non_alpha(unsigned char *w);
struct WordEntry *wc_dict_find_or_insert(struct WordEntry *buckets[], unsigned num_buckets, const unsigned char *s);
void wc_free_chain(struct WordEntry *p);
