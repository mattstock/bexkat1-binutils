#define TC_BEXKAT1 1
#ifndef TARGET_BYTES_BIG_ENDIAN
#define TARGET_BYTES_BIG_ENDIAN 1
#endif
#define WORKING_DOT_WORD

#define TARGET_FORMAT (target_big_endian ? "elf32-bexkat1-be" : "elf32-bexkat1-le")

#define TARGET_ARCH bfd_arch_bexkat1

#define md_undefined_symbol(NAME)	0

#define md_estimate_size_before_relax(A, B) (as_fatal(_("estimate size\n")),0)
#define md_convert_frag(B, S, F) (as_fatal(_("convert_frag\n")))

#define MD_PCREL_FROM_SECTION(FIX, SEC) md_pcrel_from(FIX)

extern long md_pcrel_from(struct fix *);

#define md_section_align(SEGMENT, SIZE) (SIZE)
