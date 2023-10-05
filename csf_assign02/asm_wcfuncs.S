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
/* Implementation of hash function */
wc_hash:
    subq $8, %rsp                   // Decrement stack pointer
    movl $5381, %edx                // Initialize hash_code to 5381

.Lhash_loop:
    movzbl (%rdi), %eax             // Load next byte (character) from memory to eax
    testb %al, %al                  // Test for null terminator
    jz .Lhash_end                   // If null, end the loop
    imul $33, %edx, %edx            // Multiply hash_code by 33
    addl %eax, %edx                 // Add the character to hash_code
    inc %rdi                        // Move to the next character in string
    jmp .Lhash_loop                 // Repeat the loop

.Lhash_end:
    movl %edx, %eax                 // Move the final hash_code to eax for returning
    addq $8, %rsp                   // Align stack pointer
    ret                             // Return the hash_code

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
    subq $8, %rsp                   // Decrement stack pointer

.Lstr_compare_loop:                 // Loop condition to parse through string until the NULL char is found
    movb (%rdi), %al                // Load a byte from the addresses in rdi and rsi into registers al and bl respectively
    movb (%rsi), %bl

    cmpb %bl, %al                   // Compare the bytes in al and bl
    jne .Lstr_compare_diff          // If they are not equal, jump to .is_difference

    cmpb $0, %al                    // Test if the end of the string (NULL character) is reached
    je .Lstr_compare_equal          // If NULL is reached, jump to .equal

    incq %rdi                       // Increment the string pointers (rdi and rsi)
    incq %rsi

    jmp .Lstr_compare_loop          // Jump back to .loop to compare the next characters

.Lstr_compare_diff:
    movzbl %al, %eax                // Move the zero extended values of al and bl into eax and ebx respectively
    movzbl %bl, %ebx
    subl %ebx, %eax                 // Subtract the value in ebx from eax

    addq $8, %rsp                   // Align the stack pointer
    ret                             // Return the result

.Lstr_compare_equal:
    mov $0, %eax                    // Set the return value to 0
    addq $8, %rsp                   // Align the stack pointer
    ret                             // Return 0 if equal

/*
 * Copy NUL-terminated source string to the destination buffer.
 *
 * C function prototype:
 *    void wc_str_copy(unsigned char *dest, const unsigned char *source);
 */
	.globl wc_str_copy
wc_str_copy:
    subq $8, %rsp                   // Decrement stack pointer

.Lstr_copy_loop:
    movb (%rsi), %dl                // load a byte (char) from string source index into the dl register
    movb %dl, (%rdi)                // move the char into the destination index

    cmpb $0, %dl                    // check if the null terminator has been reached
    je .Lstr_copy_end               // jump to the end condition

    incq %rdi                       // increment the destination register to get the next writable index
    incq %rsi                       // increment the source register to get the next char index

    jmp .Lstr_copy_loop             // jump back to the str_copy_loop to copy the next char

.Lstr_copy_end:
    addq $8, %rsp                   // Align the stack pointer
    ret                             // return the destination string

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
wc_isspace:                         // Input is dil (8 bit of rdi register)
    subq $8, %rsp                   // Decrement stack pointer

    cmp $32, %dil                   // Check if char is a space ' '
    je .Lisspace                    // Jump to is_space and return 1

    cmp $9, %dil                    // Check if char is a tab '\t'
    je .Lisspace                    // Jump to is_space and return 1

    cmp $13, %dil                   // Check if char is a carriage return '\r'
    je .Lisspace                    // Jump to is_space and return 1

    cmp $10, %dil                   // Check if char is a newline '\n'
    je .Lisspace                    // Jump to is_space and return 1

    cmp $12, %dil                   // Check if char is a form feed '\f'
    je .Lisspace                    // Jump to is_space and return 1

    cmp $11, %dil                   // Check if char is a vertical tab '\v'
    je .Lisspace                    // Jump to is_space and return 1

    mov $0, %eax                    // If not space, move 0 to eax
    addq $8, %rsp                   // Align stack pointer
    ret                             // Return 0

.Lisspace:
    mov $1, %eax                    // If space, move 1 to eax
    addq $8, %rsp                   // Align stack pointer
    ret                             // Return 1

/*
 * Return 1 if the character code in c is an alphabetic character
 * ('A' through 'Z' or 'a' through 'z'), 0 otherwise.
 *
 * C function prototype:
 *    int wc_isalpha(unsigned char c);
 */
    .globl wc_isalpha
wc_isalpha:                         // Input is dil (8 bit of rdi register)
    subq $8, %rsp                   // Decrement stack pointer

    cmp $65, %dil                   // Compare dil with 65 ('A')
    jb .Lisalpha_not                // If less, jump to not alpha

    cmp $90, %dil                   // Compare value with 90 ('Z')
    jbe .Lisalpha                   // If less than or equal, jump to alpha

    cmp $97, %dil                   // compare value with 97 ('a')
    jb .Lisalpha_not                // If less, jump to not alpha

    cmp $122, %dil                  // Compare value with 122 ('z')
    jbe .Lisalpha                   // If less than, jump to alpha

                                    // If above 122, jump to not alpha
.Lisalpha_not:
    mov $0, %eax                    // Move 0 to eax
    addq $8, %rsp                   // Align stack pointer
    ret                             // Return 0

.Lisalpha:
    mov $1, %eax                    // Move 1 to eax
    addq $8, %rsp                   // Align stack pointer
    ret                             // Return 1

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
    subq $8, %rsp                   // Decrement stack pointer

.Lreadnext_initial:
    call fgetc                      // Get the next char in the file
    cmp $-1, %rax                   // Check if c == EOF
    je .Lreadnext_failed            // If c == EOF, exit loop

    push %rdi                       // Push FILE *in to store it
    push %al                        // Move char to first argument
    call wc_isspace                 // Call the isspace function
    pop %al
    test %eax, %eax                 // Check if the char is a space
    jnz .Lreadnext_end              // If the char is a space, end the read

    movb %al, (%rsi)               // Store c in the string
    pop %rdi                        // Restore FILE *in to %rdi
    incq %rsi                       // Increment string position


.Lreadnext_loop:
    test %rdi, %rdi
    jz .Lreadnext_end
    incq %rdi
    call fgetc                      // Call fgetc(in)
    cmp $-1, %rax                   // Check if c == EOF
    je .Lreadnext_end               // If c == EOF, exit loop

    push %rdi                       // Push FILE *in to store it
    movb %al, %dil                  // Move char to first argument
    call wc_isspace                 // Call the isspace function
    test %eax, %eax                 // Check if the char is a space
    jnz .Lreadnext_end              // If the char is a space, end the read

    movb %dil, (%rsi)               // Store c in the string
    pop %rdi                        // Restore FILE *in to %rdi
    incq %rsi                       // Increment string position

.Lreadnext_end:
    incq %rsi                       // Increment string position
    movb $0, (%rsi)                 // Null-terminate the string
    mov $1, %rax                    // Set the return value to 1 (successful)
    addq $8, %rsp                   // Restore the stack pointer
    ret

.Lreadnext_failed:
    movl $0, %eax                   // Set the return value to 0 (failure)
    addq $8, %rsp                   // Restore the stack pointer
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
    subq $8, %rsp                   // Decrement stack pointer

.Ltolower_loop:
    movb (%rdi), %al                // Load a byte (char) into the al register
    cmpb $0, %al                    // Check if the char is the null terminator
    je .Ltolower_end                // End the loop and return

    cmpb $65, %al                   // Compare the char to 'A'
    jb .Ltolower_continue           // If the char is less than 'A', continue the loop

    cmpb $90, %al                   // Compare the char to 'Z'
    jbe .Ltolower_convert           // If the char is greater than 'Z', continue the loop

.Ltolower_continue:
    incq %rdi                       // Get the next char of the string
    jmp .Ltolower_loop              // Repeat loop checks

.Ltolower_convert:
    addl $32, (%rdi)                // Make the char lowercase ('a' - 'A' = 97 - 65 = 32)
    incq %rdi                       // Move to next char in string
    jmp .Ltolower_loop

.Ltolower_end:
    addq $8, %rsp                   // Align stack pointer
    ret                             // Return the lowercase string

/*
 * Remove any non-alphaabetic characters from the end of the
 * NUL-terminated character string pointed-to by w.
 *
 * C function prototype:
 *    void wc_trim_non_alpha(unsigned char *w);
 */
	.globl wc_trim_non_alpha
wc_trim_non_alpha:
    subq $8, %rsp                   // Decrement stack pointer

.Ltrim_non_alpha_loop:
    movq %rsi, %r8                  // Store the address of the beginning of the string
    movb (%rsi), %al                // Load a byte (char) into the al register
    testb %al, %al                  // Check if the char is the null terminator
    jz .Ltrim_non_alpha             // If the end of the string is reached, begin trimming

    incq %rsi                       // Otherwise, go to the next char
    jmp .Ltrim_non_alpha_loop       // Continue the loop

.Ltrim_non_alpha:
    decq %rsi                       // Move to the previous char
    test %rsi, %r8                  // Copy char to the dil register for the function call
    jz .Ltrim_non_alpha_end

    movb (%rsi), %dil               // Move the char into %dil for the call to isalpha
    call wc_isalpha                 // Call the isalpha function
    test %eax, %eax                 // Check if the character is alphanumeric from the function result
    jnz .Ltrim_non_alpha_end        // If alphanumeric, stop trimming and return

    movb $0, (%rsi)                 // Replace char with a null terminator
    jmp .Ltrim_non_alpha            // Check if the next character is not alphanumeric

.Ltrim_non_alpha_end:
    addq $8, %rsp                   // Align stack pointer
    ret                             // Void return


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
    subq $8, %rsp                   // Decrement stack pointer
    movq %rdi, %rcx

.search_loop:
    cmpq $0, %rdi                   // Check if cur == NULL
    je .not_found

    call wc_str_compare
    test %eax, %eax
    jz .found_match

    movq WORDENTRY_NEXT_OFFSET(%rdi), %rdi
    jmp .search_loop

.not_found:
    movq $72, %rdi
    call malloc              // Allocate memory for WordEntry
    call wc_str_copy
    movq $0, WORDENTRY_COUNT_OFFSET(%rdi)
    movq %rcx, WORDENTRY_NEXT_OFFSET(%rdi)
    movq $1, %rdx
    addq $8, %rsp                   // Align the stack pointer   
    ret

.found_match:
    movq $0, %rdx  
    addq $8, %rsp                   // Align the stack pointer    
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
 *    struct WordEntry *wc_dict_find_or_insert(struct WordEntry *buckets[], unsigned num_buckets, const unsigned char *s)//
 */
	.globl wc_dict_find_or_insert
wc_dict_find_or_insert:
    subq $8, %rsp                   // Decrement stack pointer
    movq %rdi, %rcx

.dsearch_loop:
    cmpq $0, %rdi                   // Check if cur == NULL
    je .dnot_found

    call wc_str_compare
    test %eax, %eax
    jz .dfound_match

    movq WORDENTRY_NEXT_OFFSET(%rdi), %rdi
    jmp .dsearch_loop

.dnot_found:
    movq $72, %rdi
    call malloc              // Allocate memory for WordEntry
    call wc_str_copy
    movq $0, WORDENTRY_COUNT_OFFSET(%rdi)
    movq %rcx, WORDENTRY_NEXT_OFFSET(%rdi)
    movq $1, %rdx
    addq $8, %rsp                   // Align the stack pointer   
    ret

.dfound_match:
    movq $0, %rdx  
    addq $8, %rsp                   // Align the stack pointer    
    ret

/*
 * Free all of the nodes in given linked list of WordEntry objects.
 *
 * C function prototype:
 *    void wc_free_chain(struct WordEntry *p)//
 */
	.globl wc_free_chain
wc_free_chain:
    
    ret

/*
vim:ft=gas:
*/
