// Important: do not add any additional #include directives!
// The only C library functions that may be used are
//
// - fgetc
// - malloc
// - free

#include <stdlib.h>
#include "wcfuncs.h"

// Compute a hash code for the given NUL-terminated
// character string.
//
// The hash algorithm should be implemented as follows:
//
// uint32_t hash_code = 5381
// for each character c of w in order {
//   hash_code = hash_code * 33 + c
// }
//
// Note that the character values should be treated as
// being unsigned (in the range 0..255)
uint32_t wc_hash(const unsigned char *w) {
  uint32_t hash_code = 5381;
  while (*w){
    hash_code = hash_code * 33 + *w++; // compute hashcode for each char in w
  }
  return hash_code;
}

// Compare two strings lexicographically. Return
//
// - a negative value if lhs string is less than rhs string
// - 0 if lhs string is identical to rhs string
// - a positive value if lhs string is greater than rhs string
//
// Lexicographical comparison is a generalization of alphabetical
// order, but using character codes. If one string is a prefix
// of the other, it is considered as "less than". E.g.,
// "hi" would compare as less than "high".
int wc_str_compare(const unsigned char *lhs, const unsigned char *rhs) {
  // exit loop when one or both strings have been fully checked
  while (*lhs && *rhs) {
    unsigned char lhs_c = *lhs++;
    unsigned char rhs_c = *rhs++;
    if (lhs_c < rhs_c)
      return -1;
    if (rhs_c < lhs_c)
      return 1;
  }
  // if strings are the same length and identical, return 0
  if (!*lhs && !*rhs) 
    return 0;
  if (!*lhs) 
    return -1;
  else 
    return 1;
  return 0;
}

// Copy NUL-terminated source string to the destination buffer.
void wc_str_copy(unsigned char *dest, const unsigned char *source) {
  while(*source) {
    *dest++ = *source++;
  }
  *dest = '\0'; // add null character after the string has been copied
}

// Return 1 if the character code in c is a whitespace character,
// false otherwise.
//
// For the purposes of this function, a whitespace character is one of
//
//   ' '
//   '\t'
//   '\r'
//   '\n'
//   '\f'
//   '\v'
int wc_isspace(unsigned char c) {
  // switch case to check whitespace characters
  switch (c) {
    case ' ':
      return 1;
    case '\t':
      return 1;
    case '\r':
      return 1;
    case '\n':
      return 1;
    case '\f':
      return 1;
    case '\v':
      return 1;
    default:
      return 0;
  }
  return 0;
}

// Return 1 if the character code in c is an alphabetic character
// ('A' through 'Z' or 'a' through 'z'), 0 otherwise.
int wc_isalpha(unsigned char c) {
  // uppercase chars are between 65 and 90
  // lowercase chars are between 97 and 122
  if ((c <= 90 && c >= 65) | (c <= 122 && c >= 97))
    return 1;
  return 0;
}

// Read the next word from given input stream, storing
// the word in the array pointed-to by w. (This array should be
// assumed to be MAX_WORDLEN+1 elements in size.) Return 1
// if a word is read successfully, 0 otherwise.
//
// For the purposes of this function, a word is a sequence of
// 1 or more non-whitespace characters.
//
// If a sequence of non-whitespace characters has more than
// MAX_WORDLEN characters, then only the first MAX_WORDLEN
// characters in the sequence should be stored in the array.
int wc_readnext(FILE *in, unsigned char *w) {
  int i = 0; int c;
  // check that the initial read is successful and if not, return 0
  while ((c = fgetc(in)) != EOF && wc_isspace(c));
  if (c == EOF && i == 0) {
    return 0;
  }
  w[i++] = (unsigned char)c;
  // read in a word while the file has not ended and a space
  // has not been seen
  while(i < MAX_WORDLEN && (c = fgetc(in)) != EOF && !wc_isspace(c)) {
    w[i++] = (unsigned char)c;
  }
  // add null char to the end of the input word
  w[i] = '\0';
  return 1;
}

// Convert the NUL-terminated character string in the array
// pointed-to by w so that every letter is lower-case.
void wc_tolower(unsigned char *w) {
  while(*w) {
    // check that the char is a character between a-z and A-Z
    if (wc_isalpha(*w)) {
      if (*w <= 90 && *w >= 65)
        *w += 32; // Difference between uppercase A (65) and lowercase a (97)
    }
    w++;
  }
}

// Remove any non-alphaabetic characters from the end of the
// NUL-terminated character string pointed-to by w.
void wc_trim_non_alpha(unsigned char *w) {
  // move to the end of the string
  while(*w) {
    w++;
  }
  w--; // go back one since the while loop terminated after we went past the string
  // find the first alphabetic character from the back of the string
  while(*w) {
    if ((*w >= 'A' && *w <= 'Z') || (*w >= 'a' && *w <= 'z'))
        break;
    *w-- = '\0'; // replace non alphabetic char with a null terminator
  }
}

// Search the specified linked list of WordEntry objects for an object
// containing the specified string.
//
// If a matching object is found, set the int variable pointed-to by
// inserted to 0 and return a pointer to the matching object.
//
// If a matching object is not found, allocate a new WordEntry object,
// set its next pointer to point to what head points to (i.e., so the
// new object is at the head of the list), set the variable pointed-to
// by inserted to 1, and return a pointer to the new node. Note that
// the new node should have its count value set to 0. (It is the caller's
// job to update the count.)

struct WordEntry *wc_find_or_insert(struct WordEntry *head, const unsigned char *s, int *inserted) {
  struct WordEntry *cur = head;
  // search for the matching string
  while (cur) {
    // if the matching string is found, return the current node
    if (wc_str_compare(cur->word, s) == 0) {
      *inserted = 0;
      return cur;
    }
    cur = cur->next;
  }
  // create a new node at the top of the list if the word doesn't exist
  // and return it
  struct WordEntry *entry = malloc(sizeof(struct WordEntry));
  entry->next = head;
  entry->count = 0;
  wc_str_copy(entry->word, s);
  *inserted = 1;
  return entry;
}

// Find or insert the WordEntry object for the given string (s), returning
// a pointer to it. The head of the linked list which contains (or should
// contain) the entry for s is the element of buckets whose index is the
// hash code of s mod num_buckets.
//
// Returns a pointer to the WordEntry object in the appropriate linked list
// which represents s.
struct WordEntry *wc_dict_find_or_insert(struct WordEntry *buckets[], unsigned num_buckets, const unsigned char *s) {
  int index = wc_hash(s) % (int)num_buckets; // convert hash to index
  struct WordEntry *head = buckets[index];
  int inserted = 0;
  struct WordEntry *entry = wc_find_or_insert(head, s, &inserted);
  // if a new WordEntry was created, since it is the new head, 
  // set the bucket to its value
  if (inserted == 1)
    buckets[index] = entry;
  return entry;
}

// Free all of the nodes in given linked list of WordEntry objects.
void wc_free_chain(struct WordEntry *p) {
  // recursively free the WordEntry linked list
  if (!p)
    return;
  wc_free_chain(p->next);
  free(p);
}
