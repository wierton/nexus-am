MEMORY {
  ddr : ORIGIN = 0x80000000, LENGTH = 128M
}

SECTIONS {
  .text : {
	_ex_entry = .;
	*(.ex_entry)
	_ex_entry_end = .;
    *(loader)
    *(.text)
    etext = .;
    _etext = .;
  } > ddr
  .data : {
    *(.data)
    edata = .;
    _edata = .;
  } > ddr
  .bss : {
    *(.bss*)
    *(.sbss)
    *(.scommon)
    end = .;
    _end = .;
  } > ddr
  _ex_entry_size = _ex_entry_end - _ex_entry;
  _heap_start = ALIGN(4096);
  _stack_pointer = ORIGIN(ddr) + LENGTH(ddr);
  _heap_end = _stack_pointer - 4 * 1024 * 1024;
}
