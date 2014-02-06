OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(reset)
/* Memory Spaces Definitions */
MEMORY
{
    sram (W!RX) : ORIGIN = 0x300000, LENGTH = 0x20000 /* sram, 128K */
}
SECTIONS
{
    .fixed0 :
    {
        . = ALIGN(4);
        _sfixed = .;
        KEEP(*(.vectors .vectors.*))
        *(.textEntry)
        *(.text .text.* .gnu.linkonce.t.*)
        *(.glue_7t) *(.glue_7)
        *(.CP15_*)
        *(.rodata .rodata* .gnu.linkonce.r.*)
        *(.ARM.extab* .gnu.linkonce.armextab.*)

        /* Support C constructors, and C destructors in both user code
           and the C library. This also provides support for C++ code. */
        . = ALIGN(4);
        KEEP(*(.init))
        . = ALIGN(4);
        __preinit_array_start = .;
        KEEP (*(.preinit_array))
        __preinit_array_end = .;

        . = ALIGN(4);
        __init_array_start = .;
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        __init_array_end = .;

        . = ALIGN(0x4);
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*crtend.o(.ctors))

        . = ALIGN(4);
        KEEP(*(.fini))

        . = ALIGN(4);
        __fini_array_start = .;
        KEEP (*(.fini_array))
        KEEP (*(SORT(.fini_array.*)))
        __fini_array_end = .;

        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*crtend.o(.dtors))
        *(.data .data.*);
        . = ALIGN(4);
        _efixed = .;            /* End of text section */
    } > sram

        . = ALIGN(4);
	.text : { 
		_stext = .;
		*(.text)
		*(.rodata)                 /* read-only data (constants) */
		*(.rodata*)
		. = ALIGN(4);
		*(.glue_7)
		. = ALIGN(4);		
		*(.eh_frame)
		. = ALIGN(4);
 		_etext = . ;
	}

	/* collect all initialized .data sections */
	/* .data : AT ( ADDR (.text) + SIZEOF (.text) SIZEOF (.ARM.*) {  */

	. = ALIGN(4);
	.dummy : {
		_edummy = .;
	}

	.data : AT (LOADADDR(.dummy)) {
		_sdata = .;
		*(.vectors)
		*(.data)
		_edata = .;
	}

	/* collect all uninitialized .bss sections */
	.bss (NOLOAD) : { 
		. = ALIGN(4);
		_sbss = .;
		*(.bss)
		_ebss = .;
	}
}
end = .;  /* define a global symbol marking the end of application */

