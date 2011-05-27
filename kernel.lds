OUTPUT_FORMAT("binary")
ENTRY(kernel_start)

SECTIONS{
        .text 0x00010000 : { *(.text); }

        .rodata : { *(.rodata) }

        .data : { *(.data) }

        .bss : {
             sbss = .;
             *(COMMON)
             *(.bss)
             ebss = .;
        }

        /DISCARD/ : { *(.comment) }
}
