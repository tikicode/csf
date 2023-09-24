/*
 * Assembly language function implementations
 */

#define MAX_WORDLEN 63

/*
 * Offsets for the fields of the struct WordEntry data type
 * (Note that there is 4 bytes of padding between the count and next
 * fields.)
 */
#define WORDENTRY_WORD_OFFSET   (0)
#define WORDENTRY_COUNT_OFFSET  (MAX_WORDLEN+1)
#define WORDENTRY_NEXT_OFFSET   (MAX_WORDLEN+1+4+4)

	.section .rodata
/* Define any string constants or read-only data here */

	.section .text

/*
 * Compute a hash code for the given NUL-terminated
 * character string.
 *
 * The hash algorithm should be implemented as follows:
 *
 * uint32_t hash_code = 5381
 * for each character c of w in order {
 *   hash_code = hash_code * 33 + c
 * }
 *
 * Note that the character values should be treated as
 * being unsigned (in the range 0..255)
 *
 * C function prototype:
 *    uint32_t wc_hash(const unsigned char *w);
 */
	.globl wc_hash
wc_hash:
/* TODO: implement */
	ret

/*
 * Compare two strings lexicographically. Return
 *
 * - a negative value if lhs string is less than rhs string
 * - 0 if lhs string is identical to rhs string
 * - a positive value if lhs string is greater than rhs string
 *
 * Lexicographical comparison is a generalization of alphabetical
 * order, but using character codes. If one string is a prefix
 * of the other, it is considered as "less than". E.g.,
 * "hi" would compare as less than "high".
 *
 * C function prototype:
 *    int wc_str_compare(const unsigned char *lhs, const unsigned char *rhs);
 */
	.globl wc_str_compare
wc_str_compare:
    .loop:
        // Load a byte from the addresses in rdi and rsi into registers al and bl respectively
        movb (%rdi), %al
        movb (%rsi), %bl
        // Compare the bytes in al and bl
        cmpb %bl, %al
        // If they are not equal, jump to .is_difference
        jne .is_difference 
        // Test if the end of the string (NULL character) is reached
        testb %al, %al
        // If NULL is reached, jump to .equal
        jz .equal 
        // Increment the string pointers (rdi and rsi)
        incq %rdi
        incq %rsi
        // Jump back to .loop to compare the next characters
        jmp .loop

    .is_difference:
        // Move the zero extended values of al and bl into eax and ebx respectively
        movzbl %al, %eax
        movzbl %bl, %ebx
        // Subtract the value in ebx from eax
        subl %ebx, %eax
        // Return the result
        ret

    .equal:
        // Set the return value to 0 (strings are equal)
        xorl %eax, %eax
        ret







/*
 * Copy NUL-terminated source string to the destination buffer.
 *
 * C function prototype:
 *    void wc_str_copy(unsigned char *dest, const unsigned char *source);
 */
	.globl wc_str_copy
wc_str_copy:
	/* TODO: implement */
	ret

/*
 * Return 1 if the character code in c is a whitespace character,
 * false otherwise.
 *
 * For the purposes of this function, a whitespace character is one of
 *
 *   ' '
 *   '\t'
 *   '\r'
 *   '\n'
 *   '\f'
 *   '\v'
 *
 * C function prototype:
 *    int wc_isspace(unsigned char c);
 */
	.globl wc_isspace
wc_isspace:
    // input is dil (8 bit of rdi register)
    cmp $32, %dil   // check if char is a space ' '
    je is_space     // jump to is_space and return 1

    cmp $9, %dil    // check if char is a tab '\t'
    je is_space     // jump to is_space and return 1

    cmp $13, %dil   // check if char is a carriage return '\r'
    je is_space     // jump to is_space and return 1

    cmp $10, %dil   // check if char is a newline '\n'
    je is_space     // jump to is_space and return 1

    cmp $12, %dil   // check if char is a form feed '\f'
    je is_space     // jump to is_space and return 1

    cmp $11, %dil   // check if char is a vertical tab '\v'
    je is_space     // jump to is_space and return 1

    mov $0, %eax    // if not space, move 0 to eax
    ret             // return 0

is_space:
    mov $1, %eax    // if space, move 1 to eax
    ret             // return 1



/*
 * Return 1 if the character code in c is an alphabetic character
 * ('A' through 'Z' or 'a' through 'z'), 0 otherwise.
 *
 * C function prototype:
 *    int wc_isalpha(unsigned char c);
 */
    .globl wc_isalpha
wc_isalpha:
    // Comparing value with 65 ('A')
    cmp $65, %dil 
    jl is_not_alpha      // If less, jump to not alpha

    // Comparing value with 90 ('Z')
    cmp $90, %dil 
    jle is_alpha         // If less than or equal, jump to alpha

    // Comparing value with 97 ('a')
    cmp $97, %dil 
    jl is_not_alpha      // If less, jump to not alpha

    // Comparing value with 122 ('z')
    cmp $122, %dil 
    jg is_not_alpha      // If greater, jump to not alpha

is_alpha:
    mov $1, %eax        // Move 1 to eax
    ret                 // Return 1

is_not_alpha:
    mov $0, %eax        // Move 0 to eax
    ret                 // Return 0



/*
 * Read the next word from given input stream, storing
 * the word in the array pointed-to by w. (This array should be
 * assumed to be MAX_WORDLEN+1 elements in size.) Return 1
 * if a word is read successfully, 0 otherwise.
 *
 * For the purposes of this function, a word is a sequence of
 * 1 or more non-whitespace characters.
 *
 * If a sequence of non-whitespace characters has more than
 * MAX_WORDLEN characters, then only the first MAX_WORDLEN
 * characters in the sequence should be stored in the array.
 *
 * C function prototype:
 *    int wc_readnext(FILE *in, unsigned char *w);
 */
	.globl wc_readnext
wc_readnext:
	/* TODO: implement */
	ret

/*
 * Convert the NUL-terminated character string in the array
 * pointed-to by w so that every letter is lower-case.
 *
 * C function prototype:
 *    void wc_tolower(unsigned char *w);
 */
	.globl wc_tolower
wc_tolower:
	/* TODO: implement */
	ret

/*
 * Remove any non-alphaabetic characters from the end of the
 * NUL-terminated character string pointed-to by w.
 *
 * C function prototype:
 *    void wc_trim_non_alpha(unsigned char *w);
 */
	.globl wc_trim_non_alpha
wc_trim_non_alpha:
	/* TODO: implement */
	ret

/*
 * Search the specified linked list of WordEntry objects for an object
 * containing the specified string.
 *
 * If a matching object is found, set the int variable pointed-to by
 * inserted to 0 and return a pointer to the matching object.
 *
 * If a matching object is not found, allocate a new WordEntry object,
 * set its next pointer to point to what head points to (i.e., so the
 * new object is at the head of the list), set the variable pointed-to
 * by inserted to 1, and return a pointer to the new node. Note that
 * the new node should have its count value set to 0. (It is the caller's
 * job to update the count.)
 *
 * C function prototype:
 *    struct WordEntry *wc_find_or_insert(struct WordEntry *head, const unsigned char *s, int *inserted);
 */
	.globl wc_find_or_insert
wc_find_or_insert:
	/* TODO: implement */
	ret

/*
 * Find or insert the WordEntry object for the given string (s), returning
 * a pointer to it. The head of the linked list which contains (or should
 * contain) the entry for s is the element of buckets whose index is the
 * hash code of s mod num_buckets.
 *
 * Returns a pointer to the WordEntry object in the appropriate linked list
 * which represents s.
 *
 * C function prototype:
 *    struct WordEntry *wc_dict_find_or_insert(struct WordEntry *buckets[], unsigned num_buckets, const unsigned char *s);
 */
	.globl wc_dict_find_or_insert
wc_dict_find_or_insert:
	/* TODO: implement */
	ret

/*
 * Free all of the nodes in given linked list of WordEntry objects.
 *
 * C function prototype:
 *    void wc_free_chain(struct WordEntry *p);
 */
	.globl wc_free_chain
wc_free_chain:
	/* TODO: implement */
	ret

/*
vim:ft=asm:
*/
