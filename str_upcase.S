/* 
 * str_upcase - convert each lower-case letter to upper case 
 * in the specified string. The string value is modified in place. 
 * 
 * Parameters:
 * 
 * %rdi - pointer to beginning of the string to convert
 */

    .globl str_upcase

str_upcase: 

        /* 
         * Note: lower case characters are in the range 97-122
         * upper case characters are in the range 65-90 
         * Substracting 32 from lowercase converts it to uppercase 
         * stop when char code 0 reached 
         *
         * Register use: 
         * %r12 - saved pointer to string 
         * %r10 - index value 
         * %rax - length of string 
         */  
        /* call strlen to find out how long the string is */
        pushq %r12 /* save pointer to string locally, implicitly align stack by subtracting 8 bytes */
        movq %rdi, %r12 /* save value originally in %r12 */
        /* pass argument to strlen: already in %rdi */
        call strlen 
        movq $0, %r10 /* index value */ 
    
    .Lstr_upcase_loop: 
        cmpq %rax, %r10 /* strlen_s current index */ 