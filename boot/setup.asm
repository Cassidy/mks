        ;; Program: setup.asm
        ;; Purpose: 设置好保护模式的环境，跳转到保护模式。
        ;; Date: 2011-3-9 19:42
        ;; Author: mofaph <mofaph@gmail.com>

        jmp short setup_start

        RAM_size_addr   equ     0x90000 ; 段基址
        RAM_size_offset equ     0x0000  ; 段偏移

msg_ARDS:               db      "BaseAddrHigh BaseAddrLow LengthHigh LengthLow  Type",0
msg_RAM_size:           db      "RAM Total Size: ",     0
msg_memory_size:        db      "RAM Size: ",           0

ARDS:                   dw      0
pos:                    dw      0
RAM_size:               dd      0

setup_start:
        mov ax, cs              ; cs = 0x0080
        mov ds, ax
        mov es, ax
        mov ax, 0x9000
        mov ss, ax
        mov sp, 0xf000

        call open_a20
        call get_memory_size
        call display_memory_size
        call setup_idt
        call setup_gdt
        call init_8259A
        call enable_protect_mode

        jmp  dword 0x08:0x5000  ; 跳转到 setup32 中执行

        jmp $                   ; 注意！本程序到此结束
        
open_a20:
        in al, 0x92
        or al, 0x02
        out 0x92, al
        
        ret

        ;; get_memory_size -- 使用中断 int 0x15 获得内存地址

        ;; 输入：
        ;; eax --> 0xe820
        ;; ebx --> 0
        ;; ecx --> es:di 所指向的地址范围描述符结构的大小，以字节为单位。
        ;; edx --> 0x534d4150 ('SMAP') -- BIOS 将会使用此标志，对调用者将要请求的系统
        ;;         映像进行校验，这些信息会被 BIOS 放置到 es:di 所指向的结构中。
        ;; es:di --> 指向一个地址范围描述符结构 ARDS
        ;;           (Address Range Descriptor Structure)，BIOS 将会填充此结构
        ;; 输出：
        ;; CF --> CF=0 表示没有错误，否则存在错误
        ;; eax --> 0x534d4150 ('SMAP')
        ;; ebx --> 下一个地址描述符所需要的后续值
        ;; ecx --> BIOS 填充在地址范围描述符中的字符数量，被 BIOS 所返回的最小值是 20 字节
        ;; es:di --> 返回的地址范围描述符结构指针，和输入值相同

        ;; ARDS 的结构：
        ;; Type                 16
        ;; LengthHigh           12
        ;; LengthLow            8
        ;; BaseAddrHigh         4
        ;; BaseAddrLow          0

get_memory_size:        
        mov ax, 0x9000
        mov es, ax
        mov ebx, 0
        mov di, bx
.go:
        mov eax, 0xe820
        mov ecx, 20
        mov edx, 0x534d4150
        int 0x15
        jc  .get_memory_failed
        add di, 20
        inc word [ARDS]
        cmp ebx, 0
        je  .end
        jmp .go
.end:
        ret

.get_memory_failed:
        mov word [ARDS], 0
        jmp $                   ; 获取内存容量失败，死机

display_memory_size:
        push cx
        mov cx, msg_ARDS
        call disp_str
        pop cx
        push ax
        push dx
        call print_nr
        pop dx
        pop ax
        
        mov cx, [ARDS]
        mov di, 0
.display_BaseAddrHigh:
        mov eax, dword [es:di+4]
        push cx
        call print_hex
        pop cx
        mov ax, 0x0020
        call print_char
        call print_char
        call print_char
.display_BaseAddrLow:
        mov dword eax, [es:di+0]
        push cx
        call print_hex
        pop cx
        mov ax, 0x0020
        call print_char
        call print_char
.display_LengthHigh:
        mov dword eax, [es:di+12]
        push cx
        call print_hex
        pop cx
        mov ax, 0x0020
        call print_char
.display_LengthLow:
        mov dword eax, [es:di+8]
        push cx
        call print_hex
        pop cx
        mov ax, 0x0020
        call print_char
.display_Type:
        mov dword eax, [es:di+16]
        push cx
        call print_hex
        pop cx
        
        push ax
        push dx
        call print_nr
        pop dx
        pop ax

.calculate_RAM_size:        
        ;; cmp eax, 1
        ;; jne .next_ARDS
        mov dword eax, [es:di+8]
        add dword [RAM_size], eax

.next_ARDS:        
        add di, 20
        dec cx
        cmp cx, 0
        jne .display_BaseAddrHigh

.total_RAM:
        push cx
        mov cx, msg_RAM_size
        call disp_str
        mov dword eax, [RAM_size]

        ;; 把 RAM 总量放在内存地址 0x90000 处
        push fs
        push eax
        mov eax, RAM_size_addr  ; RAM_size_addr = 0x90000(4个0)
        shr eax, 4              ; 右移 4 位，现在 ax = 0x9000(3个0)
        mov fs, ax              ; 现在 fs = 0x9000
        pop eax                 ; eax = RAM 总量
        mov [fs:RAM_size_offset], eax
        pop fs

        call print_hex
        pop cx

        ret

setup_idt:
        lidt [idt_descr]
        ret

        ;; init_8259A: 初始化 8251A 中断控制器
        ;; 摘自 《Orange'S: 一个操作系统的实现》 第三章

init_8259A:
        mov al, 0x11
        out 0x20, al            ; 主 8259， ICW1
        call io_delay

        out 0xa0, al            ; 从 8259， ICW1
        call io_delay

        mov al, 0x20            ; IRQ0 对应中断向量 0x20
        out 0x21, al            ; 主 8259， ICW2
        call io_delay

        mov al, 0x28            ; IRQ8 对应中断向量 0x28
        out 0xa1, al            ; 从 8259， ICW2
        call io_delay

        mov al, 0x04            ; IR2 对应从 8259
        out 0x21, al            ; 主 8259， ICW3
        call io_delay

        mov al, 0x02            ; 对应主 8259 的 IR2
        out 0xa1, al            ; 从 8259， ICW3
        call io_delay

        mov al, 0x01
        out 0x21, al            ; 主 8259， ICW4
        call io_delay

        out 0xa1, al            ; 从 8259， ICW4
        call io_delay

        mov al, 0xff            ; 0xff = (1111 1111) 屏蔽主 8259 所有中断
        out 0x21, al            ; 主 8259， OCW1
        call io_delay

        out 0xa1, al            ; 从 8259， OCW1
        call io_delay

        ret

        ;; io_delay: 延时
io_delay:
        nop
        nop
        nop
        nop
        ret
        
setup_gdt:
        lgdt [gdt_descr]
        ret
enable_protect_mode:
        cli                     ; 关中断
        mov eax, cr0
        or  eax, 1
        mov cr0, eax
        ret

        ;; 下面是一些子程序
        
        ;; disp_str: 将首地址为 cx 的字符串写入显存
disp_str:
        push si
        push di
        mov si, cx
        mov di, [pos]
.loop:
        mov ah, 0x0c            ; 0000: 黑底 1100: 红字
        cld                     ; DF=0，lodsb时使si增1
        lodsb                   ; ds:si --> al
        test al, al
        jz .end_loop
        mov [gs:di], ax
        add di, 2

        jmp .loop

.end_loop:
        mov [pos], di           ; 保存显示位置
        pop di
        pop si
        
        ret

        ;; print_char: 显示在 ax 中的字符
print_char:
        push di
        mov di, [pos]
        cld
        mov [gs:di], ax
        add di, 2
        mov [pos], di
        pop di
        ret

        ;; print_nr: 回车
print_nr:
        push di
        mov di, [pos]
        mov ax, di
        mov dl, 160
        div dl                  ; ax = dl * 商 + 余数， al<-商，ah<-余数
        inc al
        mul dl                  ; ax<-al*dl
        mov di, ax
        mov [pos], di
        pop di
        ret
        
        ;; print_hex: 显示在 al 中的十六进制数字
print_hex:
        push eax
        mov ah, 0x0f
        mov al, 0x30          ; '0'
        call print_char
        mov al, 0x78          ; 'x'
        call print_char
        pop eax

        mov cx, 8
.loop:
        rol eax, 4
        push eax
        and al, 0x0f
        mov ah, 0x0f
        add al, 48              ; al is a digit
        cmp al, 57
        jbe .digit              ; al <= 57
        add al, 65-48-10
.digit:        
        call print_char
        pop eax
        dec cx
        cmp cx, 0
        jne .loop

        ret

        align 4                 ; 4 字节对齐
        dw 0                    ; 为了提高效率，这里空出两个字节
idt_descr:
        dw 256*8-1              ; IDT 的 16 位表限长
        dd 0x800+idt            ; IDT 的 32 位线性基地址

        align 4
        dw 0
gdt_descr:
        dw 256*8-1              ; GDT 的 16 位表限长
        dd 0x800+gdt            ; GDT 的 32 位线性基地址
        
idt:
        times 512 dd 0          ; 建立一个空的中断描述符表
gdt:
        dd 0x00000000
        dd 0x00000000
        
        ;; 32MB 可读非一致代码段
        ;; 段基址：0x00000000 段界限：0x02000
        ;; G=1 D/B=1 AVL=0 P=1 DPL=00 TYPE=1010
        dw 0x1fff
        dw 0x0000
        dw 0x9a00
        dw 0x00c0

        ;; 32MB 可写数据段
        ;; 段基址：0x00000000 段界限：0x02000
        ;; G=1 D/B=1 AVL=0 P=1 DPL=00 TYPE=0010
        dw 0x1fff
        dw 0x0000
        dw 0x9200
        dw 0x00c0

        times 5120-($-$$) db 0
        ;; end of setup.asm
