#as: -a64 -mpower10
#ld: --no-plt-localentry -T ext.lnk
#objdump: -d
#target: powerpc64*-*-*

.*

Disassembly of section \.text:

.* <.*\.long_branch\.f1>:
.*:	(18 00 41 f8|f8 41 00 18) 	std     r2,24\(r1\)
.*:	(.. .. 00 48|48 00 .. ..) 	b       .* <f1>

.* <.*\.long_branch\.g1>:
.*:	(18 00 41 f8|f8 41 00 18) 	std     r2,24\(r1\)
.*:	(.. .. 00 48|48 00 .. ..) 	b       .* <g1>

.* <.*\.plt_branch\.ext>:
.*:	(00 20 60 3d|3d 60 20 00) 	lis     r11,8192
.*:	(00 00 6b 61|61 6b 00 00) 	ori     r11,r11,0
.*:	(ff ef 13 06|06 13 ef ff) 	pla     r12,-268435736	# 0
.*:	(e8 fe 80 39|39 80 fe e8) 
.*:	(46 17 6b 79|79 6b 17 46) 	sldi    r11,r11,34
.*:	(14 62 8b 7d|7d 8b 62 14) 	add     r12,r11,r12
.*:	(a6 03 89 7d|7d 89 03 a6) 	mtctr   r12
.*:	(20 04 80 4e|4e 80 04 20) 	bctr
.*:	(00 80 82 e9|e9 82 80 00) 	ld      r12,-32768\(r2\)
.*:	(a6 03 89 7d|7d 89 03 a6) 	mtctr   r12
.*:	(20 04 80 4e|4e 80 04 20) 	bctr

.* <.*\.long_branch\.f2>:
.*:	(00 00 00 60|60 00 00 00) 	nop
.*:	(00 00 10 06|06 10 00 00) 	pla     r12,108	# .* <f2>
.*:	(6c 00 80 39|39 80 00 6c) 
.*:	(.. .. 00 48|48 00 .. ..) 	b       .* <f2>

.* <.*\.long_branch\.g2>:
.*:	(00 00 00 60|60 00 00 00) 	nop
.*:	(00 00 10 06|06 10 00 00) 	pla     r12,144	# .* <g2>
.*:	(90 00 80 39|39 80 00 90) 
.*:	(.. .. 00 48|48 00 .. ..) 	b       .* <g2>
#...

.* <f1>:
.*:	(01 00 00 48|48 00 00 01) 	bl      .* <f1>
.*:	(.. .. ff 4b|4b ff .. ..) 	bl      .* <.*\.long_branch\.f2>
.*:	(.. .. 00 48|48 00 .. ..) 	bl      .* <g1>
.*:	(.. .. ff 4b|4b ff .. ..) 	bl      .* <.*\.long_branch\.g2>
.*:	(.. .. ff 4b|4b ff .. ..) 	bl      .* <.*\.plt_branch\.ext>
.*:	(20 00 80 4e|4e 80 00 20) 	blr

.* <g1>:
.*:	(.. .. ff 4b|4b ff .. ..) 	bl      .* <.*\.long_branch\.f2>
.*:	(.. .. ff 4b|4b ff .. ..) 	bl      .* <f1>
.*:	(.. .. ff 4b|4b ff .. ..) 	bl      .* <.*\.long_branch\.g2>
.*:	(.. .. ff 4b|4b ff .. ..) 	bl      .* <g1>
.*:	(20 00 80 4e|4e 80 00 20) 	blr

.* <f2>:
.*:	(02 10 40 3c|3c 40 10 02) 	lis     r2,4098
.*:	(00 90 42 38|38 42 90 00) 	addi    r2,r2,-28672
.*:	(.. .. ff 4b|4b ff .. ..) 	bl      .* <.*\.long_branch\.f1>
.*:	(18 00 41 e8|e8 41 00 18) 	ld      r2,24\(r1\)
.*:	(.. .. ff 4b|4b ff .. ..) 	bl      .* <f2\+0x8>
.*:	(00 00 00 60|60 00 00 00) 	nop
.*:	(.. .. ff 4b|4b ff .. ..) 	bl      .* <.*\.long_branch\.g1>
.*:	(18 00 41 e8|e8 41 00 18) 	ld      r2,24\(r1\)
.*:	(.. .. 00 48|48 00 .. ..) 	bl      .* <g2\+0x8>
.*:	(00 00 00 60|60 00 00 00) 	nop
.*:	(.. .. ff 4b|4b ff .. ..) 	bl      .* <.*\.plt_branch\.ext\+0x20>
.*:	(00 00 00 60|60 00 00 00) 	nop
.*:	(20 00 80 4e|4e 80 00 20) 	blr

.* <g2>:
.*:	(02 10 40 3c|3c 40 10 02) 	lis     r2,4098
.*:	(00 90 42 38|38 42 90 00) 	addi    r2,r2,-28672
.*:	(.. .. ff 4b|4b ff .. ..) 	bl      .* <f2\+0x8>
.*:	(00 00 00 60|60 00 00 00) 	nop
.*:	(.. .. ff 4b|4b ff .. ..) 	bl      .* <.*\.long_branch\.f1>
.*:	(18 00 41 e8|e8 41 00 18) 	ld      r2,24\(r1\)
.*:	(.. .. ff 4b|4b ff .. ..) 	bl      .* <g2\+0x8>
.*:	(00 00 00 60|60 00 00 00) 	nop
.*:	(.. .. ff 4b|4b ff .. ..) 	bl      .* <.*\.long_branch\.g1>
.*:	(18 00 41 e8|e8 41 00 18) 	ld      r2,24\(r1\)
.*:	(20 00 80 4e|4e 80 00 20) 	blr

.* <_start>:
.*:	(00 00 00 48|48 00 00 00) 	b       .* <_start>
#...

Disassembly of section \.text\.ext:

8000000000000000 <ext>:
8000000000000000:	(02 10 40 3c|3c 40 10 02) 	lis     r2,4098
8000000000000004:	(00 90 42 38|38 42 90 00) 	addi    r2,r2,-28672
8000000000000008:	(00 00 00 60|60 00 00 00) 	nop
800000000000000c:	(20 00 80 4e|4e 80 00 20) 	blr
