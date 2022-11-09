/* hello.S */

.section .rodata 

sHelloMsg: .string "Hello, world\n"

.section .text 

        .globl main //make main visible to other modules

main: 
    subq $8, %rsp 
    movl $0, %eax
    movq $sHelloMsg, %rdi 
    call printf 

    addq $8, %rsp
    ret 