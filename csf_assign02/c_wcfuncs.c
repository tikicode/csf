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
    hash_code = hash_code * 33 + *w++;
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
  while (*lhs && *rhs) {
    char lhs_c = *lhs++;
    char rhs_c = *rhs++;
    if (lhs_c < rhs_c)
      return -1;
    if (rhs_c < lhs_c)
      return 1;
  }
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
  *dest = '\0';
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
  while((c = fgetc(in)) != EOF) {
    if (i == MAX_WORDLEN) {
      i++;
      break;
    }
    c = (unsigned char) c;
    if (c == EOF)
      return 0;
    if (wc_isspace(c))
      break;
    w[i++] = c;
  }
  if (i == 0)
    return 0;
  w[i] = '\0';
  return 1;
}

// Convert the NUL-terminated character string in the array
// pointed-to by w so that every letter is lower-case.
void wc_tolower(unsigned char *w) {
  while(*w) {
    if (wc_isalpha(*w)) {
      if (*w <= 90 && *w >= 65)
        *w += 32;
    }
    w++;
  }
}

// Remove any non-alphaabetic characters from the end of the
// NUL-terminated character string pointed-to by w.
void wc_trim_non_alpha(unsigned char *w) {
  while(*w) {
    w++;
  }
  w--;
  while(*w) {
    if (wc_isalpha(*w))
      break;
    *w-- = '\0';
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
  while (cur) {
    if (wc_str_compare(cur->word, s) == 0) {
      *inserted = 0;
      return cur;
    }
    cur = cur->next;
  }
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
  int index = wc_hash(s) % (int)num_buckets;
  struct WordEntry *head = buckets[index];
  int inserted = 0;
  struct WordEntry *entry = wc_find_or_insert(head, s, &inserted);
  if (inserted == 1)
    buckets[index] = entry;
  return entry;
}

// Free all of the nodes in given linked list of WordEntry objects.
void wc_free_chain(struct WordEntry *p) {
  if (!p)
    return;
  wc_free_chain(p->next);
  free(p);
}
