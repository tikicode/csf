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
  pushq %r12                      // Local variable for the hashcode
  pushq %r13                      // Local variable to store loaded chars
  movl $5381, %r12d               // Initialize hash_code to 5381

.Lhash_loop:
  movzbl (%rdi), %r13d            // Load next byte (character) from memory to eax
  testb %r13b, %r13b              // Test for null terminator
  jz .Lhash_end                   // If null, end the loop
  imul $33, %r12d, %r12d          // Multiply hash_code by 33
  addl %r13d, %r12d               // Add the character to hash_code
  inc %rdi                        // Move to the next character in string
  jmp .Lhash_loop                 // Repeat the loop

.Lhash_end:
  movl %r12d, %eax                // Move the final hash_code to eax for returning
  pop %r13                        // Restore value of %r13
  pop %r12                        // Restore value of %r12
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
  push %r12                       // Local variable for the lhs char
  push %r13                       // Local variable for the rhs char

.Lstr_compare_loop:               // Loop condition to parse through string until the NULL char is found
  movb (%rdi), %r12b              // Load a char from each string into their respective local variables
  movb (%rsi), %r13b

  cmpb %r12b, %r13b               // Compare the lhs and rhs chars
  jne .Lstr_compare_diff          // If they are not equal, jump to .is_difference

  cmpb $0, %r12b                  // Test if the end of the string (NULL character) is reached
  je .Lstr_compare_equal          // If NULL is reached, jump to .equal

  incq %rdi                       // Increment the string pointers (rdi and rsi)
  incq %rsi

  jmp .Lstr_compare_loop          // Jump back to .loop to compare the next characters

.Lstr_compare_diff:
  movzbl %r12b, %eax              // Move the zero extended values of the local chars to 32 bit registers
  movzbl %r13b, %r13d
  subl %r13d, %eax                // Subtract the values and store in the return value

  pop %r13                        // Restore value of %r13
  pop %r12                        // Restore value of %r12
  addq $8, %rsp                   // Align the stack pointer
  ret                             // Return the result

.Lstr_compare_equal:
  mov $0, %eax                    // Set the return value to 0
  pop %r13                        // Restore value of %r13
  pop %r12                        // Restore value of %r12
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
  push %r12                       // Local variable for the copied char

.Lstr_copy_loop:
  movb (%rsi), %r12b              // Load a byte (char) from string source index into the temp register
  movb %r12b, (%rdi)              // Move the char into the destination index

  cmpb $0, %r12b                  // Check if the null terminator has been reached
  je .Lstr_copy_end               // Jump to the end condition

  incq %rdi                       // Increment the destination register to get the next writable index
  incq %rsi                       // Increment the source register to get the next char index

  jmp .Lstr_copy_loop             // Jump back to the str_copy_loop to copy the next char

.Lstr_copy_end:
  movb $0, (%rdi)                 // Add null terminator to destination string
  pop %r12                        // Restore value of %r12
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
  subq $8, %rsp                     // Decrement stack pointer

  cmp $32, %dil                     // Check if char is a space ' '
  je .Lisspace                      // Jump to is_space and return 1

  cmp $9, %dil                      // Check if char is a tab '\t'
  je .Lisspace                      // Jump to is_space and return 1

  cmp $13, %dil                     // Check if char is a carriage return '\r'
  je .Lisspace                      // Jump to is_space and return 1

  cmp $10, %dil                     // Check if char is a newline '\n'
  je .Lisspace                      // Jump to is_space and return 1

  cmp $12, %dil                     // Check if char is a form feed '\f'
  je .Lisspace                      // Jump to is_space and return 1

  cmp $11, %dil                     // Check if char is a vertical tab '\v'
  je .Lisspace                      // Jump to is_space and return 1

  mov $0, %eax                      // If not space, move 0 to eax
  addq $8, %rsp                     // Align stack pointer
  ret                               // Return 0

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
  subq $8, %rsp                     // Decrement stack pointer

  cmp $65, %dil                     // Compare dil with 65 ('A')
  jb .Lisalpha_not                  // If less, jump to not alpha

  cmp $90, %dil                     // Compare value with 90 ('Z')
  jbe .Lisalpha                     // If less than or equal, jump to alpha

  cmp $97, %dil                     // compare value with 97 ('a')
  jb .Lisalpha_not                  // If less, jump to not alpha

  cmp $122, %dil                    // Compare value with 122 ('z')
  jbe .Lisalpha                     // If less than, jump to alpha

                                    // If above 122, jump to not alpha
.Lisalpha_not:
  mov $0, %eax                      // Move 0 to eax
  addq $8, %rsp                     // Align stack pointer
  ret                               // Return 0

.Lisalpha:
  mov $1, %eax                      // Move 1 to eax
  addq $8, %rsp                     // Align stack pointer
  ret                               // Return 1

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
    subq $8, %rsp                 // Decrement stack pointer

    testq %rdi, %rdi              // Check if FILE *in is null
    jz .Lreadnext_failed          // Jump to failed read if null

    pushq %r12                    // Local var for FILE *in
    pushq %r13                    // Local var for the destination string
    pushq %r14                    // Local var for the counter
    pushq %r15                    // Local var for chars

    movq %rdi, %r12               // Put FILE *in in local
    movq %rsi, %r13               // Put destination in local
    movl $0, %r14d                // Initialize counter to zero
    movq $0, %r15                 // Ensure char var is empty

.Lreadnext_loop:
    cmpl $MAX_WORDLEN, %r14d      // Check count against max size of dest string (64)
    je .Lconsume_extra            // If max size reached, consume extra characters

    movq %r12, %rdi               // Restore %rdi to FILE *in for the call to fgetc
    call fgetc                    // Get next char from file

    movb %al, %r15b               // Move char return from fgetc to local char var
    cmp $-1, %eax                 // Check if c == EOF
    je .Lreadnext_end             // If EOF, end read

    movb %r15b, %dil             // Move char from fgetc to first argument for input into isspace
    call wc_isspace               // Call isspace
    cmpl $1, %eax                 // Check if the char is a space
    je .Lreadnext_end             // If the char is a space, end the read

    movb %r15b, (%r13)            // Move the char to the destination string

    incq %r13                     // Move to the next position of the destination string
    incl %r14d                    // Increment the counter

    jmp .Lreadnext_loop           // Get the next char

.Lconsume_extra:                  // New label to consume extra characters
    movq %r12, %rdi               // Restore %rdi to FILE *in for the call to fgetc
    call fgetc                    // Get next char from file

    cmp $-1, %eax                 // Check if c == EOF
    je .Lreadnext_end             // If EOF, end read

    movb %al, %dil               // Move char from fgetc to first argument for input into isspace
    call wc_isspace               // Call isspace
    cmpl $1, %eax                 // Check if the char is a space
    je .Lreadnext_end             // If the char is a space, end the read

    jmp .Lconsume_extra           // Consume next extra char

.Lreadnext_end:
    movb $0, (%r13)               // Null terminate the destination string

    movl %r14d, %eax              // Move the counter to the output val
    mov %r13, %rsi                // Move the local FILE *in and restore to input
    mov %r12, %rdi                // Restore destination string to the input

    popq %r15                     // Restore value of local vars
    popq %r14
    popq %r13
    popq %r12

    cmpl $0, %eax                 // Check if the counter is 0, if so, the read has failed
    je .Lreadnext_failed

    movl $1, %eax                 // If the read was successful, return one
    addq $8, %rsp                 // Restore the stack ptr
    ret

.Lreadnext_failed:
    movl $0, %eax                 // If the read failed, return zero
    addq $8, %rsp                 // Restore the stack ptr
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
  subq $8, %rsp                     // Decrement stack pointer
  push %r12                         // Local var for char

.Ltolower_loop:
  movb (%rdi), %r12b                // Load a byte (char) into the char register
  cmpb $0, %r12b                    // Check if the char is the null terminator
  je .Ltolower_end                  // End the loop and return

  cmpb $65, %r12b                   // Compare the char to 'A'
  jb .Ltolower_continue             // If the char is less than 'A', continue the loop

  cmpb $90, %r12b                   // Compare the char to 'Z'
  jbe .Ltolower_convert             // If the char is greater than 'Z', continue the loop

.Ltolower_continue:
  incq %rdi                         // Get the next char of the string
  jmp .Ltolower_loop                // Repeat loop checks

.Ltolower_convert:
  addl $32, (%rdi)                  // Make the char lowercase ('a' - 'A' = 97 - 65 = 32)
  incq %rdi                         // Move to next char in string
  jmp .Ltolower_loop

.Ltolower_end:  
  pop %r12                          // Restore local var
  addq $8, %rsp                     // Align stack pointer
  ret                               // Return the lowercase string

/*
 * Remove any non-alphaabetic characters from the end of the
 * NUL-terminated character string pointed-to by w.
 *
 * C function prototype:
 *    void wc_trim_non_alpha(unsigned char *w);
 */
	.globl wc_trim_non_alpha
wc_trim_non_alpha:
  subq $8, %rsp                     // Align the stack pointer
.Lloop:
  movb (%rdi), %al                  // get first char
  testb %al, %al
  jz .Ltrim
  incq %rdi
  jmp .Lloop
.Ltrim:
  decq %rdi
  movb (%rdi), %al
  testb %al, %al
  jz .Ltrim_non_alpha_end
  jmp .Ltrim_isalpha
  jnz .Ltrim_non_alpha_end
  jmp .Ltrim
.Ltrim_isalpha:
  cmp $65, %al      
  jl .Ltrim                      

  cmp $90, %al      
  jle .Ltrim_non_alpha_end      


  cmp $97, %al     
  jl .Ltrim          


  cmp $122, %al     
  jg .Ltrim            

.Ltrim_non_alpha_end:
  incq %rdi
  movb $0, (%rdi)
  addq $8, %rsp
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
  subq $8, %rsp
  pushq %r12                        // Local var to track cur pos of WordEntry
  pushq %r13                        // Local var to track string
  pushq %r14                        // Local var to track inserted
  pushq %r15                        // Local var to track *head

  movq %rdi, %r12                   // Assign cur pos to head
  movq %rsi, %r13                   // Assign string var to s
  movq %rdx, %r14                   // Assign inserted var to inserted
  movq %rdi, %r15                   // Assign head tracker to head

.Lsearch_loop:
  testq %r12, %r12                  // Check if the WordEntry list is null
  jz .Lnot_found                    // Insert a new entry

  movq %r12, %rdi                   // Move the current WordEntry to rdi
  movq %r13, %rsi                   // Move the target word to rsi 
  call wc_str_compare               // Compare the words
  test %eax, %eax                   // Check if the words are the same
  jz .Lfound_match                  // Return cur pos

  movq WORDENTRY_NEXT_OFFSET(%r12), %r12 // Move to the next WordEntry object in the linked list
  jmp .Lsearch_loop                 // Continue searching

.Lnot_found:
  movq $80, %rdi                    // Move 80 (size of WordEntry obj) to the first input var %rdi for malloc
  call malloc                       // Allocate a new WordEntry object
  movq %rax, %r12                   // Move result to the cur pos

  movq %r13, %rsi                   // Move the target string into %rsi for copying
  call wc_str_copy                  // Copy the target string into the new WordEntry object

  movl $0, WORDENTRY_COUNT_OFFSET(%r12)  // Initialize count to 0
  movq %r15, WORDENTRY_NEXT_OFFSET(%r12) // Put the new object at the top of the linked list

  

  movq %r12, %rax                   // Move the new object to the return register
  movl $1, (%r14)                   // Set inserted to one

  popq %r15                         // Restore local variables
  popq %r14
  popq %r13
  popq %r12
  
  addq $8, %rsp                     // Align stack ptr
  ret                               // Return the new WordEntry

.Lfound_match:
  movq %r12, %rax                   // Move the cur WordEntry to the return address
  movl $0, (%r14)                   // Set inserted to zero

  popq %r15                         // Restore local variables
  popq %r14
  popq %r13
  popq %r12

  addq $8, %rsp                     // Align stack ptr
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
wc_dict_find_or_insert:
    // Preserve callee-saved registers
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    pushq %rbp

    // Store input arguments for clarity and future use
    movq %rdi, %r12  // Save pointer to buckets in r12
    movl %esi, %r13d // Save num_buckets in r13d
    movq %rdx, %r14  // Save pointer to source word in r14

    // Calculate hash index of the source word
    movq %r14, %rdi  // Set source word as argument for wc_hash
    call wc_hash
    xorl %edx, %edx  // Clear edx for division
    divl %r13d       // Divide, result in eax, remainder in edx
    movl %edx, %r15d // Store remainder (hash index) in r15d

    // Locate appropriate bucket using the hash index
    movq (%r12, %r15, 8), %rdi  // Retrieve bucket's head pointer

    // Set up arguments for wc_find_or_insert
    movq %r14, %rsi  // Source string
    leaq -8(%rbp), %r13  // Address of 'inserted' local variable
    movq $0, (%r13)  // Initialize 'inserted' to zero
    movq %r13, %rdx  // Address for 'inserted' as third argument

    call wc_find_or_insert

    // Check and act on the inserted flag
    cmpq $1, (%r13)
    je .LupdateBucket

.LskipUpdate:
    addq $8, %rsp   // Realign stack after 'inserted'
    jmp .Lexit

.LupdateBucket:
    // Update the bucket with the new WordEntry if needed
    movq %rax, (%r12, %r15, 8)

.Lexit:
    // Restore callee-saved registers and return
    popq %rbp
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    ret

/*
 * Free all of the nodes in given linked list of WordEntry objects.
 *
 * C function prototype:
 *    void wc_free_chain(struct WordEntry *p)//
 */
	.globl wc_free_chain
wc_free_chain:
  subq $8, %rsp                     // Align stack ptr
  pushq %r12                        // Create local var for memory to be freed
  movq %rdi, %r12                   // Put WordEntry object in local var

.Lfree_chain_loop:
  testq %r12, %r12                  // Check if the whole chain has been freed
  jz .Lfree_chain_end               // End the loop

  movq %r12, %rdi                   // Move the current WordEntry object to the input var for free
  movq WORDENTRY_NEXT_OFFSET(%r12), %r12  // Move to the next WordEntry object
  call free                         // Call free on the previous WordEntry object
  jmp .Lfree_chain_loop             // Continue freeing

.Lfree_chain_end:
  popq %r12                         // Restore local variable
  addq $8, %rsp                     // Align stack ptr
  ret                               // Void return

/*
vim:ft=gas:
*/
