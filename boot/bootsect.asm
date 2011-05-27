        ;; Program: bootsect.asm
        ;; Purpose: This is a boot sector.
        ;; Author:  mofaph <mofaph@gmail.com>
        ;; Date:    2011/2/26

        ;; 下面的表格是 BIOS 规定的 0~1M 内存的作用：

        ;; 0x00000~0x003FF         Interrupt Vector(256*4B=1024B)
        ;; 0x00400~0x004FF         BIOS Data Area
        ;; ------------------------------------------------------------
        ;; 0x00500~0x07BFF         Free Memory Area
        ;; 0x07C00~0x07DFF         Boot Sector(512B)
        ;; 0x07E00~0x9FC00         Free Memory Area
        ;; ------------------------------------------------------------
        ;; 0x9FC01~0x9FFFF         Extended BIOS Data Area (usually 1K)
        ;; 0xA0000~0xAFFFF         EGA/VGA/XGA/XVGA Graphic Video Buffer
        ;; 0xB0000~0xB7FFF         Mono text video buffer
        ;; 0xB8000~0xBFFFF         CGA/EGA+ Chroma text video buffer
        ;; 0xC0000~0xFDFFF         BIOS Routine & Card BIOS
        ;; 0xFE000~0xFFFF0         BIOS Boot Block
        ;; 0xFFFF1~0xFFFFF

        jmp 0x07c0:start
        nop                     ; 真的需要这个延时吗？

        ;; 把 bootsect.asm 从 0x07c00 移动到内存物理地址 0x00600 处
start:
        mov ax, 0x07c0
        mov ds, ax
        mov ax, 0x0060
        mov es, ax
        xor si, si
        xor di, di
        mov cx, 0x200           ; 移动 512 字节

        cld
        rep movsb

        jmp 0x0060:real_start

real_start:
        mov ax, 0x0060
        mov ds, ax
        mov fs, ax
        mov ax, 0x0050
        mov ss, ax              ; 堆栈暂时设在 0x00500 处
        mov ax, 0xb800
        mov gs, ax              ; Mono text video buffer
        mov sp, 0x0100          ; 栈顶指针是 0x00600 -- 256B

        call load_setup
        call load_setup32
        call load_kernel

        jmp  0x0080:0x0000      ; 跳转到 setup 执行

load_setup:
        mov ax, 0x0080          ; 0x0800 -- setup 段基址 2KB 开始处
        mov es, ax              ; 0x0000 -- setup 段偏移
        mov bx, 0x0000
        mov al, 10
        mov [read_per_time], al
        mov ax, 0
        mov [track], ax
        mov ax, 0
        mov [head], ax
        mov al, 2
        mov [sector], al

        call read_track
        call update_orthl
        ret

load_setup32:
        mov ax, 0x0500
        mov es, ax              ; 0x5000 -- setup32 段基址
        xor bx, bx              ; 0x0000 -- setup32 段偏移
        mov ax, 88
        mov [total], ax
        mov ax, 0
        mov [readed], ax
.go:
        mov ax, [total]
        mov cx, [readed]
        sub ax, cx              ; ax = total - readed
        ja  .continue           ; total > readed, continue
        jb  load_setup_failed   ; total < readed, error
.done:
        ret                     ; total = readed, done
.continue:
        push ax                 ; 保存未读扇区数
        mov ax, [logic_sector]
        xor cx, cx
        mov cl, 18
        div cl                  ; ax / 18，ah = 余数，al = 商
        sub cl, ah              ; 现在 cl 中保存的是每次要读的扇区数
        inc ah
        mov [sector], ah        ; 读软盘磁道的起始扇区

        pop ax                  ; 弹出未读扇区数
        cmp cx, ax              ; cx=read_per_time, ax=total-readed
        jbe .next               ; read_per_time <= total - readed
        mov [read_per_time], al ; read_per_time = total - readed
        jmp .ready
.next:
        mov [read_per_time], cl ; read_per_time = 18 - (logic_sector mod 18)
.ready:
        call read_track
        call update_orthl
        jmp  load_setup32.go

        ;; load_kernel -- 从软盘第 126 个扇区开始，加载 1024 个扇区入内存（0x10000~0x8FFFF）

load_kernel:    
        xor ax, ax
        mov es, ax
.go:
        mov ax, es
        add ax, 0x1000
        cmp ax, 0x9000
        jae load_kernel.done    ; jae 指令不能完成段间跳转，只能使用 jmp 完成
        
        mov es, ax
        xor bx, bx
        mov ax, 128
        mov [total], ax         ; 总共读取 64KB，128 个扇区
        mov ax, 0
        mov [readed], ax
.in_segment:
        mov ax, [total]
        mov cx, [readed]
        sub ax, cx              ; ax = total - readed

        je  load_kernel.go      ; total == readed，说明读完了整段 64KB
        jb  load_kernel_failed  ; total < readed

        push ax                 ; 保存未读扇区数
        mov ax, [logic_sector]
        xor cx, cx
        mov cl, 18
        div cl                  ; ax / cl, ah=余数, al=商
        sub cl, ah              ; cl 保存了每次要读的扇区数
        inc ah
        mov [sector], ah        ; 起始扇区

        pop ax
        cmp ax, cx
        jb  .last_read_in_segment
        mov [read_per_time], cl
        jmp .ready
.last_read_in_segment:
        mov [read_per_time], al
.ready:
        call read_track
        call update_orthl
        jmp  .in_segment
.done:
        ret
        
        ;; load_setup_failed: 显示红色的字符串“loading setup failed.”
load_setup_failed:
        push cx
        mov cx, msg_loader_fail
        call disp_str
        pop cx
        
        push ax
        push dx
        call print_nr
        pop dx
        pop ax

        jmp $

        ;; load_kernel_failed: 显示红色的字符串“loading kernel failed.”
load_kernel_failed:
        push cx
        mov cx, msg_kernel_fail
        call disp_str
        pop cx
        
        push ax
        push dx
        call print_nr
        pop dx
        pop ax

        jmp $
        
read_track:
        ;; int 0x13
        ;; ah=2, al=要读的扇区数
        ;; ch=柱面号（磁道）号低 8 位， cl --> （位 7,6 是磁道号高 2 位，位 5~0 起始扇区号）
        ;; dh=磁头号 dl=驱动器号（0 表示 A 盘）
        ;; es:bx --> 数据缓冲区

        ;; 下面的一段是设置 cx （磁道号和起始扇区号）
        mov ax, [track]
        mov ch, al
        and ah, 0x03
        shl ah, 6
        add ah, [sector]
        mov cl, ah

        ;; 设置其他的寄存器
        mov dh, [head]
        mov dl, 0
        mov al, [read_per_time]
        mov ah, 2

        int 0x13

        jc load_setup_failed
        ret

        ;; update_orthl: 更新 offset, readed, track, head, logic_sector
update_orthl:
        xor ax, ax
        mov al, [read_per_time]
        shl ax, 9               ; 一个扇区 512 字节，左移 9 位等于本次所读的字节数
        add bx, ax

        mov ax, [readed]
        add al, [read_per_time]
        mov [readed], ax

        xor ax, ax
        mov al, [read_per_time]
        add [logic_sector], ax

        ;; 只有读完一个磁道（18个扇区），才需要更新磁头号、磁道号
        mov ax, [logic_sector]
        mov cl, 18
        div cl
        cmp ah, 0
        jne update_orthl.done
        
        mov al, [head]
        cmp al, 1
        jne .update_head
        inc word [track]
.update_head:
        xor byte [head], 1
.done:
        ret

        ;; disp_str: 将首地址为 cx 的字符串写入显存
disp_str:
        mov si, cx
loop:
        mov ah, 0x0c            ; 0000: 黑底 1100: 红字
        cld                     ; DF=0，lodsb时使si增1
        lodsb                   ; ds:si --> al
        test al, al
        jz end_loop
        mov [gs:di], ax
        add di, 2
        jmp loop

end_loop:
        ret

        ;; print_nr: 回车
print_nr:
        mov ax, di
        mov dl, 160
        div dl                  ; ax = dl * 商 + 余数， al<-商，ah<-余数
        inc al
        mul dl                  ; ax<-al*dl
        mov di, ax
        ret

msg_loader_fail:        db "loading setup failed.",    0
msg_kernel_fail:        db "loading kernel failed.",   0

read_per_time:  db      0       ; 每次需读扇区数
sector:         db      2       ; 当前扇区号
track:          dw      0       ; 当前磁道号
head:           db      0       ; 当前磁头号
readed:         dw      0       ; 已读扇区号
total:          dw      10      ; 总共需读扇区数
logic_sector:   dw      1       ; 读软盘时的逻辑起始扇区编号，从 0 开始计数
        
        times 510-($-$$) db 0

        dw 0xaa55
