# Linker script for ARM PE.
# These are substituted in as variables in order to get '}' in a shell
# conditional expansion.
INIT='.init : { *(.init) }'
FINI='.fini : { *(.fini) }'
cat <<EOF
OUTPUT_FORMAT($OUTPUT_FORMAT)
${LIB_SEARCH_DIRS}

ENTRY(_mainCRTStartup)

SECTIONS
{
  .text ${RELOCATING+ 0x401000} : 
	{
	    ${RELOCATING+ *(.init);}
	    *(.text)
	    ${RELOCATING+ *(.fini);}
	    ${RELOCATING+ etext  =  .};
	  }


  .rdata BLOCK(0x1000) :
  { 					
	*(.rdata)
	${CONSTRUCTING+ __CTOR_LIST__ = .; LONG (-1); *(.ctors); LONG (0); }
	${CONSTRUCTING+ __DTOR_LIST__ = .; LONG (-1); *(.dtors); LONG (0); }
    ;
  }
  .data BLOCK(0x1000) : {
    *(.data)
    *(.data2)
    ;
  }
  .idata BLOCK(0x1000) :
  { 					
    *(.idata\$2)
    *(.idata\$3)
    *(.idata\$4)
    *(.idata\$5)
    *(.idata\$6)
    *(.idata\$7)
    ;
  }

  .CRT BLOCK(0x1000) :
  { 					
    *(.CRT\$XCA)
    *(.CRT\$XCC)
    *(.CRT\$XCZ)
    *(.CRT\$XIA)
    *(.CRT\$XIC)
    *(.CRT\$XIZ)
    *(.CRT\$XLA)
    *(.CRT\$XLZ)
    *(.CRT\$XPA)
    *(.CRT\$XPX)
    *(.CRT\$XPZ)
    *(.CRT\$XTA)
    *(.CRT\$XTZ)
    ;
  }
  .rsrc BLOCK(0x1000) :
  { 					
    *(.rsrc\$01)
    *(.rsrc\$02)
    ;
  }
  .reloc BLOCK(0x1000) :
  { 					
    *(.reloc)
    ;
  }
  .junk BLOCK(0x1000) :
  { 					
    *(.debug\$S)
    *(.debug\$T)
    *(.debug\$F)
    *(.drectve)
    ;
  }
  .bss BLOCK(0x1000)  :
	{
	*(.bss)
	*(COMMON);
	end = . ;
	}

  .stab  0 :
  {
    [ .stab ]
  }

  .stabstr  0 :
  {
    [ .stabstr ]
  }

${RELOCATING+ stack =  0x800000 ;}
}
EOF
