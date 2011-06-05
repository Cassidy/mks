        ;; Program: setup32.asm
        ;; Purpose: 因为是在 32 位保护模式下，重新设置 IDT，GDT，然后开启分页，
        ;;          最后跳转到 kernel_start.asm 中执行
        ;; Author:  mofaph <mofaph@gmail.com>
        ;; Date:    2011/5/2 18:52

        [BITS 32]
        [ORG 0x5000]

        pos             equ     1600
        page_dir        equ     0x00007000
        page0           equ     0x00008000
        page1           equ     0x00009000
        page2           equ     0x0000a000
        page3           equ     0x0000b000
        page4           equ     0x0000c000
        page5           equ     0x0000d000
        page6           equ     0x0000e000
        page7           equ     0x0000f000

setup32_start:
        mov eax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
        mov esp, 0x9f000

        call load_idt
        call load_gdt

        ;; 重新加载 CS 寄存器，使它指向新的代码段描述符
        jmp dword 0x08:reload_segment
reload_segment:
        mov eax, 0x10           ; 0x10 指向新的数据段描述符
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
        mov esp, 0x9f000

        call setup_paging

        jmp 0x00010000          ; 跳转到 kernel_start 执行

load_idt:
        lea edx, [ignore_int]   ; edx 是中断门高 32 位
        mov eax, 0x00080000     ; eax 是中断门低 32 位
        mov ax, dx
        mov dx, 0x8e00          ; 中断门类型，特权级为 0
        lea edi, [idt]
        mov ecx, 256
.repeat:
        mov [edi], eax
        mov [edi + 4], edx
        add edi, 8
        dec ecx
        jne .repeat
        lidt [idt_descr]
        ret

ignore_int:
        push ds
        push eax
        mov eax, 0x10
        mov ds, ax
        mov eax, 0x6A           ; 'D' -- ascii
        call write_char
        pop eax
        pop ds
        iret

write_char:
        ;; 注意：一个字符在显存中占用两个字节。偶地址字节存放字符代码，奇地址字节存放显示属性。
        push gs
        push ebx

        mov ebx, 0x18
        mov gs, bx
        mov bx, [pos]
        shl ebx, 1              ; 字符的显存位置
        mov [gs:ebx], al
        shr ebx, 1              ; 字符的屏幕位置
        inc ebx
        cmp ebx, 2000
        jb .1
        mov ebx, 0
.1:
        mov [pos], ebx          ; 保存字符位置

        pop ebx
        pop gs
        ret

load_gdt:
        cli
        lgdt [gdt_descr]
        ret

        ;; 开启分页，注意页表项应该填入物理地址
        ;; 页目录项和页表项的结构：高 20 位是物理地址高 20 位，低 12 位是属性标志
setup_paging:
        ;; 我们首先设置页目录，它的物理地址的起始地址是 0x7000
        mov eax, page_dir
        mov ebx, page0+7        ; 第一个页表的起始位置在物理地址 0x8000
        mov ecx, 8              ; 循环次数，填入 8 个页表的高 20 位物理地址
.page_dir:
        cmp ecx, 0
        je  .in_page
        mov [eax], ebx          ; 设置页目录项
        add ebx, 0x00001000     ; 页表是连续存放的，每个页表 4KB(0x1000)
        add eax, 4              ; 每个页目录项 4 字节
        dec ecx
        jmp .page_dir

        ;; 现在，我们设置页表项。总共 8 个页表，8192 个页表项（1024 * 8）
        ;; 页表项的内容含义：
        ;; 比如，第一个页表的页表项是 0x00000007
        ;; 则第一个页表所映射的物理地址的基地址是： 0x00000007 & 0xfffff000 = 0x1000
        ;; 第一个页表所映射的属性：           0x00000007 & 0x00000fff = 0x07
        ;; 0x07: 0000 0000 0111 ==>> P=1 R/W=1 U/S=1
        ;; 表示页面存在，可读写，对所有特权级上的程序都可以访问该页面

.in_page:
        mov eax, page0          ; 页表的起始地址是 0x8000
        mov ebx, 0x00000007
        mov ecx, 8192           ; 循环次数是 8192
.loop:
        cmp ecx, 0
        je  .done
        mov [eax], ebx
        add eax, 4
        add ebx, 0x00001000
        dec ecx
        jmp .loop
.done:
        ;; 设置页目录表基地址寄存器 cr3 的值，指向页目录表。
        ;; cr3 中保存的是页目录表的物理地址。
        mov eax, page_dir
        mov cr3, eax
        ;; 设置启动使用分页处理（cr0 的 PG 标志，位 31）
        mov eax, cr0
        or  eax, 0x80000000
        mov cr0, eax
        ret
        ;; end of setup_paging

        align 4
        dw 0
idt_descr:
        dw 256*8-1              ; IDT 的 16 位表限长
        dd idt                  ; IDT 的 32 位线性基地址

        align 4
        dw 0
gdt_descr:
        dw 256*8-1              ; GDT 的 16 位表限长
        dd gdt                  ; GDT 的 32 位线性基地址

        times 0x1000-($-$$) db 0

        ;; ************************************************************

        ;; IDT 的位置在 0x6000 ~ 0x67ff (2KB)
idt:
        times 512 dd 0

        ;; GDT 的位置在 0x6800 ~ 0x6fff (2KB)
gdt:
        dd 0x00000000
        dd 0x00000000

        ;; 64MB 可读非一致代码段
        ;; 段基址：0x00000000 段界限：0x02000
        ;; G=1 D/B=1 AVL=0 P=1 DPL=00 TYPE=1010
        dw 0x0000
        dw 0x0000
        dw 0x9a00
        dw 0x00c1

        ;; 64MB 可读写数据段
        ;; 段基址：0x00000000 段界限：0x02000
        ;; G=1 D/B=1 AVL=0 P=1 DPL=00 TYPE=0010
        dw 0x0000
        dw 0x0000
        dw 0x9200
        dw 0x00c1

        times 506 dd 0          ; 其余 253 个段描述符留空
