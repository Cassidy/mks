        ;; Program: kernel_start.asm
        ;; Purpose: 因为二进制文件不能进行符号引用，所以加多一个临时性的文件。这个文件是 elf 格式的，
        ;;          通过链接器可以指定它的入口地址，还能进行外部符号引用。
        ;; Author:  mofaph <mofaph@gmail.com>
        ;; Date:    2011/5/14

        [BITS 32]
        [global kernel_start]
        [extern main]

        ;; kernel_start: 把 main 函数的参数和返回地址压入栈，这样就人工地制造了一个函数的返回地址
        ;;
        ;; 注意！现在已经开启了分页，所以要注意把线性地址和物理地址对应起来。

        ;; Intel CPU 采用了下面所有函数必须遵守的寄存器用法统一惯例。
        ;; 调用者应该保存的寄存器：
        ;;     eax    edx    ecx
        ;; 被调用者必须保存的寄存器：
        ;;     ebx    esi    edi    ebp    esp

kernel_start:
        ;; 压入栈的参数应该遵守 C 语言的函数调用约定：
        ;; 参数 n
        ;; ...
        ;; 参数 2
        ;; 参数 1
        ;; 函数的返回地址

        push 0                  ; envp
        push 0                  ; argv
        push 0                  ; argc
        push main_ret_addr      ; main 函数的返回地址
        push main               ; main 函数的起始地址
        ret

main_ret_addr:
        jmp main_ret_addr       ; main 函数不应该返回到这里

        ;; kernel_start.asm ends here
