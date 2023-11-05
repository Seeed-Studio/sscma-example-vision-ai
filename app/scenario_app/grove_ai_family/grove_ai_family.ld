MEMORY {
    ICCM0 : ORIGIN = 0x00000000, LENGTH = 0x00010000
    ICCM1 : ORIGIN = 0x10000000, LENGTH = 0x00050000
    SYSTEM0 : ORIGIN = 0x20010800, LENGTH = (0x2011d650 - 0x20010800)
    VERSION : ORIGIN = 0x2011d650, LENGTH = (0x2011d670 - 0x2011d650)
    DCCM : ORIGIN = 0x80000000, LENGTH = 0x00040000
    XCCM : ORIGIN = 0x90000000, LENGTH = 0x00008000
    YCCM : ORIGIN = 0xa0000000, LENGTH = 0x00008000
}
ENTRY(_arc_reset)
SECTIONS {
    .ivt DEFINED (ivtbase_addr) ? ivtbase_addr : 0x10000004 :
    {
        *(.init_vector)
        *(.init_bootstrap)
    } > ICCM1
    .vector : ALIGN(1024)
    {
        . = ALIGN(1024);
        *(.vector)
    } > ICCM1
    .hash : { *(.hash) }
    .dynsym : { *(.dynsym) }
    .dynstr : { *(.dynstr) }
    .gnu.version : { *(.gnu.version) }
    .gnu.version_d : { *(.gnu.version_d) }
    .gnu.version_r : { *(.gnu.version_r) }
    .rel.init : { *(.rel.init) }
    .rela.init : { *(.rela.init) }
    .rel.text : { *(.rel.text .rel.text.* .rel.gnu.linkonce.t.*) }
    .rela.text : { *(.rela.text .rela.text.* .rela.gnu.linkonce.t.*) }
    .rel.fini : { *(.rel.fini) }
    .rela.fini : { *(.rela.fini) }
    .rel.rodata : { *(.rel.rodata .rel.rodata.* .rel.gnu.linkonce.r.*) }
    .rela.rodata : { *(.rela.rodata .rela.rodata.* .rela.gnu.linkonce.r.*) }
    .rel.data : { *(.rel.data .rel.data.* .rel.gnu.linkonce.d.*) }
    .rela.data : { *(.rela.data .rela.data.* .rela.gnu.linkonce.d.*) }
    .rel.tdata : { *(.rel.tdata .rel.tdata.* .rel.gnu.linkonce.td.*) }
    .rela.tdata : { *(.rela.tdata .rela.tdata.* .rela.gnu.linkonce.td.*) }
    .rel.tbss : { *(.rel.tbss .rel.tbss.* .rel.gnu.linkonce.tb.*) }
    .rela.tbss : { *(.rela.tbss .rela.tbss.* .rela.gnu.linkonce.tb.*) }
    .rel.ctors : { *(.rel.ctors) }
    .rela.ctors : { *(.rela.ctors) }
    .rel.dtors : { *(.rel.dtors) }
    .rela.dtors : { *(.rela.dtors) }
    .rel.got : { *(.rel.got) }
    .rela.got : { *(.rela.got) }
    .rel.sdata : { *(.rel.sdata .rel.sdata.* .rel.gnu.linkonce.s.*) }
    .rela.sdata : { *(.rela.sdata .rela.sdata.* .rela.gnu.linkonce.s.*) }
    .rel.sbss : { *(.rel.sbss .rel.sbss.* .rel.gnu.linkonce.sb.*) }
    .rela.sbss : { *(.rela.sbss .rela.sbss.* .rela.gnu.linkonce.sb.*) }
    .rel.sdata2 : { *(.rel.sdata2 .rel.sdata2.* .rel.gnu.linkonce.s2.*) }
    .rela.sdata2 : { *(.rela.sdata2 .rela.sdata2.* .rela.gnu.linkonce.s2.*) }
    .rel.sbss2 : { *(.rel.sbss2 .rel.sbss2.* .rel.gnu.linkonce.sb2.*) }
    .rela.sbss2 : { *(.rela.sbss2 .rela.sbss2.* .rela.gnu.linkonce.sb2.*) }
    .rel.bss : { *(.rel.bss .rel.bss.* .rel.gnu.linkonce.b.*) }
    .rela.bss : { *(.rela.bss .rela.bss.* .rela.gnu.linkonce.b.*) }
    .text : ALIGN(4)
    {
        . = ALIGN(4);
        PROVIDE (_f_text = .);
        *(TYPE bss)
        KEEP (*(SORT_NONE(.init)))
        . = ALIGN(4);
        KEEP (*crt0.o(.text.__startup))
        . = ALIGN(4);
        *(.text .stub .text.* .gnu.linkonce.t.*)
        *(.gnu.warning)
        . = ALIGN(4);
        *(.mli_lib.*)
        KEEP (*(SORT_NONE(.fini)))
        PROVIDE (__etext = .);
        PROVIDE (_etext = .);
        PROVIDE (etext = .);
        PROVIDE (_e_text = .);
    } > ICCM1
    .tensor_arena : ALIGN(16)
    {
        _f_bss_tensor_arena = .;
        *(.bss.tensor_arena)
        _e_bss_tensor_arena = .;
    } > SYSTEM0
    .cdm_buf : ALIGN(16)
    {
        _f_bss_cdm_buf = .;
        *(.bss.cdm_buf)
        _e_bss_cdm_buf = .;
    } > SYSTEM0
    .jcr : ALIGN(4)
    {
        KEEP (*(.jcr))
    } > DCCM
    .eh_frame : ALIGN(4)
    {
        KEEP (*(.eh_frame))
    } > DCCM
    .gcc_except_table : ALIGN(4)
    {
        *(.gcc_except_table) *(.gcc_except_table.*)
    } > DCCM
    .plt : ALIGN(4)
    {
        *(.plt)
    } > DCCM
    .jlitab : ALIGN(4)
    {
        __JLI_TABLE__ = .;
        jlitab*.o:(.jlitab*) *(.jlitab*)
    } > DCCM
    .read_only_data : ALIGN(4)
    {
        PROVIDE (_f_rodata = .);
        *(.rodata) *(.rodata.*) *(.gnu.linkonce.r.*)
        *(.rodata1)
        . = ALIGN(4);
        __CTOR_LIST__ = .;
        LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
        KEEP(*(SORT_BY_NAME(".ctors*")))
        LONG(0)
        __CTOR_END__ = .;
        . = ALIGN(4);
        __init_array_start = .;
        KEEP(*(SORT_BY_NAME(".init_array*")))
        __init_array_end = .;
        . = ALIGN(4);
        __DTOR_LIST__ = .;
        LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
        KEEP(*(SORT_BY_NAME(".dtors*")))
        LONG(0)
        __DTOR_END__ = .;
        PROVIDE (_e_rodata = .);
    } > SYSTEM0
    .data : ALIGN(4)
    {
        PROVIDE (__data_image = .);
        PROVIDE (__data_start = .) ;
        KEEP (*(.data))
        *(.data.* .gnu.linkonce.d.*)
        SORT(CONSTRUCTORS)
        KEEP (*crtbegin*.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend*.o ) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
        KEEP (*crtbegin*.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend*.o ) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
    } > DCCM
    .got : ALIGN(4)
    {
        *(.got.plt) *(.got)
    } > DCCM
    .sdata : ALIGN(8)
    {
        __SDATA_BEGIN__ = . ;
        _f_sdata = .;
        *(.sdata .sdata.* .gnu.linkonce.s.*)
        _edata = .;
        PROVIDE (edata = .);
        *(.sdata2 .sdata2.* .gnu.linkonce.s2.*)
        *(.noinit*)
        . = ALIGN(32 / 8);
    } > DCCM
    .sbss_all : ALIGN(4)
    {
        PROVIDE (__sbss_start = .);
        PROVIDE (___sbss_start = .);
        *(.dynsbss)
        *(.sbss .sbss.* .gnu.linkonce.sb.*)
        *(.scommon)
        PROVIDE (__sbss_end = .);
        PROVIDE (___sbss_end = .);
        *(.sbss2 .sbss2.* .gnu.linkonce.sb2.*)
    } > DCCM
    .bss_all : ALIGN(4)
    {
        PROVIDE (_f_bss = .) ;
        *(.dynbss)
        *(.bss .bss.* .gnu.linkonce.b.*)
        *(COMMON)
        . = ALIGN(32 / 8);
        _end = .;
        PROVIDE (end = .);
        PROVIDE (_e_bss = .) ;
    } > SYSTEM0
    .stack : ALIGN(8)
    {
        __start_stack = . ;
        _f_stack = .;
        . = . + 8192;
        _e_stack = .;
        __end_stack = . ;
    } > DCCM
    .heap : ALIGN(8)
    {
        __start_heap = . ;
        _f_heap = .;
        . = . + 32768;
        _e_heap = .;
        __end_heap = . ;
    } > DCCM
    .Zdata : ALIGN(16)
    {
        . = ALIGN(16);
        *(.Zdata*)
    } > DCCM
    .Xdata : ALIGN(4)
    {
        *(.Xdata*)
    } > XCCM
    .Ydata : ALIGN(4)
    {
        *(.Ydata*)
    } > YCCM
    .ver : ALIGN(16)
    {
        *(.data*)
    } > VERSION
    PROVIDE (_load_addr_text = _f_text);
    PROVIDE (_load_addr_rodata = _f_rodata);
    .stab 0 : { *(.stab) }
    .stabstr 0 : { *(.stabstr) }
    .stab.excl 0 : { *(.stab.excl) }
    .stab.exclstr 0 : { *(.stab.exclstr) }
    .stab.index 0 : { *(.stab.index) }
    .stab.indexstr 0 : { *(.stab.indexstr) }
    .comment 0 : { *(.comment) }
    .note.gnu.build-id : { *(.note.gnu.build-id) }
    .debug 0 : { *(.debug) }
    .line 0 : { *(.line) }
    .debug_srcinfo 0 : { *(.debug_srcinfo) }
    .debug_sfnames 0 : { *(.debug_sfnames) }
    .debug_aranges 0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    .debug_info 0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev 0 : { *(.debug_abbrev) }
    .debug_line 0 : { *(.debug_line .debug_line.* .debug_line_end) }
    .debug_frame 0 : { *(.debug_frame) }
    .debug_str 0 : { *(.debug_str) }
    .debug_loc 0 : { *(.debug_loc) }
    .debug_macinfo 0 : { *(.debug_macinfo) }
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames 0 : { *(.debug_varnames) }
    .debug_pubtypes 0 : { *(.debug_pubtypes) }
    .debug_ranges 0 : { *(.debug_ranges) }
    .debug_macro 0 : { *(.debug_macro) }
    .debug_addr 0 : { *(.debug_addr) }
    .arcextmap 0 : { *(.arcextmap.*) }
}
