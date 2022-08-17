
;
;	UNZIP 1.8a - Dissolves MS-DOS ZIP files created with PKZIP version 1.xx
;   ('Implode' and 'Store' formats tested with v1.01, v1.02, and v1.10)
;
;   Adapted to z88dk-z80asm by Stefano Bodrato on Nov. 2020
;   based on an original work by "David P. Goodenough"
;

;
; To build:
;
;              z80asm -b -oUNZIP.COM -DZ80 unzip18.asm
;              (8080 CPU support is NOT complete, just keep "-DZ80" !)
;
; Smaller version:
;				z80asm -b -oUNZIP.COM -DZ80 -DNOSHRINK unzip18.asm
;
; Smallest version (valid for a limited number of archives):
;				z80asm -b -oUNZIP.COM -DZ80 -DNOREDUCE -DNOSHRINK -DSILENT unzip18.asm
;
; The original program size 3417 was bytes, rounded to 3456 by the CP/M system
; the flags above reduce the program size up to 2781, probably to be to 2816
; The "smaller" version is still usable in many cases and will be in the 3072 bytes range.


;DEFC Vers	=	18


;   ------------------------------------------------------------------------
;   -- Taken from "UNZIP18.DOC"
;   ------------------------------------------------------------------------
;   
;   
;                                 UNZIP
;                              Version 1.3
;   
;                             Gene Pizzetta
;                             May 12, 1991
;   
;   
;   UNZIP dissolves MS-DOS ZIP files.  This version incorporates some
;   quick and dirty mods to make a more useful utility.  The original
;   by David Goodenough has to be the most God-awful source code I've
;   ever come across.  It was totally uncommented, and I have no idea
;   what kind of strange assembler it was written for.  This code now
;   assembles with SLR's Z80ASM and it's a little more orderly.
;   
;   USAGE:
;   
;        UNZIP {dir:}zipfile {dir:}
;   
;   Under ZCPR3 "dir" can be a DU or DIR spec.  Under vanilla CP/M
;   only a drive is recognized.
;   
;   If a destination drive is given, member files are extracted to
;   it.  With no destination given, member files are merely checked
;   and listed to the screen...slowly.
;   
;   You can now abort this thing with ^C (and the partial output
;   file, if any, will be closed and erased).  The usage screen now
;   responds to "//".
;   
;   UNZIP still needs a lot of work.  It really ought to be possible
;   to extract member files selectively, but it's all or nothing at
;   present.  In addition, listing files should be a lot faster;   
;   checking CRC's in that mode seems unnecessary.  Typing a member
;   file to the screen would also be a nice enhancement.  I have only
;   limited use for an UNZIP utility, so my purpose here was to do
;   just enough to make its operation acceptable.  Maybe someone else
;   will interested in going further.  In the meantime, Steve
;   Greenberg's ZIPDIR and Carson Wilson's ZTYPE can fill the gap.
;   
;   This version is probably not bullet-proof.  Testing has been very
;   limited, and frankly I don't understand much of the code, but so
;   far it seems to work.
;   
;        Gene Pizzetta
;        481 Revere St.
;        Revere, MA 02151
;   
;        Voice:  (617) 284-0891
;        Newton Centre Z-Node:  (617) 965-7259
;        Ladera Z-Node Central:  (213) 670-9465
;   
;   ------------------------------------------------------------------------
;   Version 1.4 modifications by Bruce Morgen, 5/16/91
;   
;   In addition to fixing a bug in the ZCPR3 detection code at
;   "setusr," Version 1.4 adds output filename specification with
;   wildcards.  Now the use of an output d:/dir: and/or an output
;   filename will trigger extract mode.  If the output filespec
;   contains no wildcards (a d:/dir: alone is equivalent to "*.*"),
;   UNZIP will now exit after the first extraction (made sense to me,
;   any objections?).  And no, Gene, I am NOT "adopting" UNZIP -- I
;   just tweaked it far enough that it ceased to annoy me
;   significantly in the very limited use I have for it...
;   
;   ------------------------------------------------------------------------
;   Version 1.5 modifications by Howard Goldstein, 6/1/91
;   
;   This release contains no new features or options.  Its purpose is
;   merely to correct some serious bugs that were introduced in the
;   previous version.  See the source file for details.
;   
;   Like Bruce and Gene, I have no intention of doing any further
;   develompment on this program.  I will, however, attempt to fix
;   anything that does not work as documented.
;   
;   ------------------------------------------------------------------------
;   Version 1.6 modifications by Howard Goldstein, 4/3/92
;   
;   OK, I lied.
;   
;   I began using UNZIP a bit more recently and found it to be much
;   too slow.  I therefore took Gene's suggestion and changed the
;   program so it no longer expands a file and checks its CRC when
;   that file is not being extracted.  I also replaceed the disk read
;   and write routines.  The old ones could handle only 128 bytes at
;   a time;    the new ones, from ZSLIB, can handle much larger
;   ;   buffers.  This increases speeed and decreases wear and tear on
;   the drives.
;   
;   
;   ------------------------------------------------------------------------
;   Version 1.7 modifications by Howard Goldstein, 4/8/92
;   
;   This version corrects a couple of bugs that were noticed
;   shortly after version 1.6 was released.  The program now works
;   correctly on files containing long ZIP comments.  A typo in the
;   usage screen was also fixed.
;   
;   ------------------------------------------------------------------------
;   Version 1.8 modifications by Howard Goldstein, 4/16/92
;   Another bug fix, (the last, I hope).  In version 1.6 I managed to
;   break the section of the program that expands "reduced" files.
;   Version 1.8 fixes it.
;   
;   ------------------------------------------------------------------------
;   -- END of "UNZIP18.DOC"
;   ------------------------------------------------------------------------


ORG 100h

;
;
; Version 1.8 -- April 16, 1992 -- Howard Goldstein
;	Fixed "unreducing" code which I inadvertently broke in version
;	1.6.
; Version 1.7 -- April 8, 1992 -- Howard Goldstein
;	Fixed problem where long ZIP comments were clobberiog data and
;	causing strange errors.  Fixed typo in help screen.  Can now be
;	assembled with M80 again.
; Version 1.6 -- April 3, 1992 -- Howard Goldstein
;	Improved performance by incorporating an 8k output buffer and a
;	dynamically allocated input buffer of up to 8k.  The program no
;	longer expands and checks the CRC of a Zipfile member that is
;	being skipped.  The keyboard is checed for control-c less
;	frequently which also speeds things up a bit.  All console output
;	is now done via direct console IO making control-c checking more
;	reliable.  I also made the program reentrant and changed the order
;	of events so that the destination directory is checked for a
;	file's existence AFTER it has been determined that the file is to
;	be extracted, not before as was the case previously.
; Version 1.5 -- june 1, 1991 -- Howard Goldstein
;	Fixed bug at WILDLP which was causing an output spec of "dir:
;	not to work correctly.  Corrected problems that were causing
;	writes to disk when a non-matching member file was being skipped.
;	Changed "disk full" logic to close and erase partial output file
;	and abort the program immediately.  Made several minor changes to
;	allow assembly with ZMAC or M80.

; Version 1.4 -- May 16, 1991 -- Bruce Morgen
;	Fixed bug at "setusr" and added output filename wildcard
;	support.  If the selected output filespec is NOT wild,
;	(and d: or dir: alone IS wild) UNZIP will exit after the
;	first extraction.  Boy, do I have a headache....

; Version 1.3 -- May 12, 1991 -- Gene Pizzetta
;	Some quick and dirty mods to make this utility more useful.
;	The original has to be the most God-awful source code I've
;	ever come across.  It is totally uncommented, and I have
;	no idea what kind of strange assembler it was written for.
;	This code now assembles with SLR's Z80ASM and it's a little
;	more orderly.
;
;	New syntax:
;		UNZIP {dir:}zipfile {dir:}{afn}
;	Under ZCPR3 "dir" can be a DU or DIR spec; otherwise, just
;	a drive.  If no destination is given, member files are checked
;	and listed.  If a destination is given, member files are
;	extracted if they match "afn" if given, otherwise the entire
;	ZIPfile is extracted.
;
;	You can now abort this thing with ^C (and the partial output
;	file, if any, will be closed and erased).  Usage screen now
;	responds to "//".  This program still needs a lot of work.
;	It's probably not bullet-proof and testing has been very
;	limited, but it seems to work.
;
; Version 1.2 -- July 3, 1990 -- David P. Goodenough


; Opt flags:
;  -DNOREDUCE, disables decoding support for the "reduce" format
;  -DNOSHRINK, disables decoding support for the "shrink" format
;  -DSILENT, remove welcome messages

;
; User equates
;
DEFC obfsiz	=	32		; 32-page (8k) fixed output buffer
DEFC ibfmax	=	32		; 32-page (8k) max. for input buffer
;
; System addresses
;
DEFC wboot	=	0
DEFC bdos	=	5
DEFC dfcb	=	5Ch
DEFC altfcb	=	6Ch
;
; BDOS service functions
;
DEFC dircon	=	6
DEFC fsearch	=	17
DEFC getdrv	=	25
DEFC setusr	=	32
;
; Other
;
DEFC STRSIZ	=	256
DEFC DLE	=	144
DEFC max_bits =	13
DEFC init_bits =	9
DEFC hsize	=	8192
DEFC first_ent =	257
DEFC clear	=	256
;DEFC maxcmax	=	1 shl max_bits
DEFC maxcmax	=	2 ** max_bits
DEFC maxSF	=	256
DEFC _code	=	0
DEFC _value	=	2
DEFC _bitlength =	3
DEFC _entries =	0
DEFC _maxlength =	2
DEFC _entry	=	4
DEFC _sf_tree_ =	4 + 4 * maxSF
;
; ASCII
;
DEFC CtrlC	=	03h
DEFC CR	=	0Dh
DEFC LF	=	0Ah
DEFC CtrlZ	=	1Ah
;
;	.request zslib
;	ext	fxropen,fxwopen,fxrclose,fxwclose,fxget,fxput
;
;	.request z3lib
;	ext	z3log
;
;	.request syslib
;	ext	f_close,f_delete,initfcb
;
;	public	$memry
;
	jp	start
;
	defm	"Z3ENV"
	defb	1
Z3EAdr:
	defw	0
;
start:
	ld	sp,locstk	; set the stack pointer
	call	ilprt
IF !SILENT
	defm	"UNZIP  Version 1.8a "
	defb	CR,LF
	defm	"For PKZIP 1.xx formats only"
	;  db	Vers/10+'0','.',Vers mod 10+'0','  -  DPG',CR,LF,0
ELSE
	defm	"UNZIP v1.8a "
ENDIF
	defb	CR,LF,0
	
	ld	a,(dfcb+1)	; filename?
	cp	' '
	jp	z,usage
	cp	'/'
	jp	z,usage
;
wasfil:
	ld	hl,0
	ld	(zipeof),hl		; init "zipeof" and "counting"
	ld	(opnflg),hl	; init "opnflg" and "conckct"
;
; Determine buffer addresses.  Output buffer size is defined by Obfsiz
; and input buffer size is either remaining memory or Ibfmax, whichever is
; less.
;
	ld	hl,(memry)	; top of dseg
	ld	(opbuf),hl	; store as output buffer address
;	ld	de,obfsiz shl 8	; output buffer size (pages)
	ld	de,obfsiz *256	; output buffer size (pages)
	ld	a,d
	add	a,a		; convert to records
	ld	(ocb),a		; and store in output control block
	add	hl,de		; determine address of input buffer
	ld	(ipbuf),hl
	ex	de,hl		; get buffer address in de
	inc	d		; assume 1 page for input buffer
	ld	hl,(bdos+1)	; top of memory
	xor	a		; clear accumulator and carry
	sbc	hl,de		; hl = memory available less 1 page
	jp	c,nomem
	inc	h		; now = total memory available
	rl	l		; shift possible extra record to carry
	adc	a,h
	add	a,h		; a = max input buffer records
	ld	b,ibfmax * 2
	cp	b		; compare computed to optimum value
	jr	c,sticb		; computed value smaller, use it
	ld	a,b		; else use optimum
sticb:
	ld	(icb),a		; store in input control block
;
	ld	a,(altfcb)
	ld	(mode),a	; set the mode (non-zero = extract)
	call	getusr		; set default user if not ZCPR3
	ld	hl,mtchfcb
	ld	bc,11
	ld	de,altfcb + 1
	ld	a,(de)
	cp	20h
	jr	z,wildfill
	ex	de,hl
IF Z80
		ldir
ELSE
		call	ldir
ENDIF
	ld	a,(altfcb)
	or	a
	jr	nz,filldn
	ld	c,getdrv
	call	bdos
	inc	a
	ld	(altfcb),a	; store it for Z3LOG
	ld	(mode),a	; set the mode (non-zero = extract)
	jr	filldn
wildfill:

	ld	b,c
wildlp:
	ld	(hl),'?'
	inc	hl
	djnz	wildlp
filldn:
	ld	a,(dfcb+9)	; check for filetype
	cp	20h
	jr	nz,wasext
;	ld	hl,+('I' shl 8) + 'Z'	; set default type to ZIP
	ld	hl,+('I' *256) + 'Z'	; set default type to ZIP
	ld	(dfcb+9),hl
	ld	a,'P'
	ld	(dfcb+11),a
wasext:
	ld	de,dfcb
	call	z3log		; log input drive/user
	ex	de,hl
	inc	hl
	ld	bc,11
	ld	de,infcb+1
IF Z80
		ldir
ELSE
		call	ldir
ENDIF
	; move filename to working fcb
	ld	de,icb
	call	fxropen		; try to open zipfile
	jr	nz,openok	; ok
	call	ilprt
	defm	"Couldn't find ZIP file"
	defb	CR,LF,0
	jr	exit
;
nomem:
	call	ilprt		; complain and fall through to exit
	defm	"Not enough memory!"
	defb	CR,LF,0
;
; All exits point here for possible future enhancements, such
; as elimination of warm boot.
;
exit:
	rst	0		; warmboot
;
sigerr:
	call	ilprt
	defm	"Bad signature in ZIP file"
	defb	CR,LF,0
	jr	exit
;
openok:
	call	getword
;	ld	de,-(('K' shl 8) + 'P')
	ld	de,-(('K' * 256) + 'P')
	add	hl,de
	ld	a,h
	or	l
	jr	nz,sigerr
	call	getword
	dec	l
	jr	nz,nocfhs
	dec	h
	dec	h
	jr	nz,sigerr
	call	pcfh
	jr	openok
;
nocfhs:
	dec	l
	dec	l
	jr	nz,nolfhs
	ld	a,h
	sub	4
	jr	nz,sigerr
	call	plfh
	jr	openok
;
nolfhs:
	dec	l
	dec	l
	jr	nz,sigerr
	ld	a,h
	sub	6
	jr	nz,sigerr
	call	pecd
clsxit:

	ld	de,icb
	call	fxrclose
	jr	exit
;
pcfh:
	ld	b,12
pcfhl1:
	push	bc
	call	getword
	pop	bc
	djnz	pcfhl1
	call	getword
	push	hl
	call	getword
	push	hl
	call	getword
	pop	de
	pop	bc
	push	hl
	push	de
	push	bc
	ld	b,6
pcfhl2:
	push	bc
	call	getword
	pop	bc
	djnz	pcfhl2
	pop	hl
	call	skpstring
	pop	hl
	call	skpstring
	pop	hl
	call	skpstring
	ret
;
pecd:
	ld	b,8
pecdl:
	push	bc
	call	getword
	pop	bc
	djnz	pecdl
	call	getword
	call	skpstring
	ret
;
plfh:
	ld	de,lfh
	ld	hl,endlfh-lfh
	call	getstring
	ld	de,junk
	ld	hl,(fnl)
	push	de
	call	getstring
	ld	de,junk + 20
	ld	hl,(efl)
	call	getstring
	ld	de,opfcb
	call	initfcb
	ex	de,hl
	inc	hl
	pop	de
	ld	b,8
	call	scanfn
	ld	a,(de)
	cp	'.'
	jr	nz,nodot
	inc	de
nodot:
	ld	b,3
	call	scanfn
	ld	hl,init
	ld	de,vars
	ld	bc,endinit-init
IF Z80
		ldir
ELSE
		call	ldir
ENDIF
	ld	a,(mode)
resmod:
	ld	(curmode),a
	or	a
	jp	z,pjunk
mtched:
	ld	b,11
	ld	hl,opfn
	ld	de,mtchfcb
mtchlp:
	ld	a,(de)
	ld	c,(hl)
	inc	hl
	inc	de
	cp	'?'
	jr	z,mtch1
	cp	c
	jr	nz,nomtch
mtch1:
	djnz	mtchlp
	call	setout		; log output drive/user
	ld	de,opfcb	; see if output file already exists
	ld	c,fsearch
	call	bdos
	inc	a
	jr	nz,exists
	jr	creok
nomtch:
	ld	hl,junk
	call	pstr
	call	ilprt
	defm	" doesn't match"
	defb	0
	jr	noex
exists:
	ld	hl,junk		; it exists, so skip it
	call	pstr
	call	ilprt
	defm	" exists"
	defb	0
	xor	a
	ld	(nodest),a
noex:
	call	ilprt
	defm	" -- not extracting "
	defb	0
	xor	a
	jr	resmod
;
creok:
	ld	de,ocb		; create output file
	call	fxwopen
	jr	nz,opnok1
	call	ilprt
	defm	"Error creating "
	defb	0
	ld	hl,junk
	call	pstr
	jr	noex
;
opnok1:
	ld	(opnflg),a
	call	ilprt
	defm	"Extracting "
	defb	0
pjunk:
	ld	hl,junk
	call	pstr
doext:
	ld	hl,counting
	inc	(hl)
	ld	a,(curmode)
	or	a
	jr	z,case0x
	call	ilprt
	defm	" -- "
	defb	0
	ld	a,(cm)
	or	a
	jr	nz,case1
	jr	case0w
case0x:
	ld	a,(nodest)
	or	a
	call	z,wildck
case0w:
	ld	a,(zipeof)
	and	1
	jr	nz,closeo
savcs0:
	call	getbyte
	call	outbyte
	jr	case0w
;
case1:
	dec	a
IF !NOSHRINK
	jr	nz,case2p
	call	unshrink
	jr	closeo
ENDIF
;
;
case2p:
	dec	a
IF !NOREDUCE
	cp	4
	jr	nc,tryimp
	call	unreduce
	jr	closeo
ENDIF
;
tryimp:
	jr	nz,badzip
	call	unimplode
	jr	closeo
;
badzip:
	call	ilprt
	defm	"Unknown compression method"
	defb	CR,LF,0
	ret
;
closeo:
	ld	hl,zipeof
	dec	(hl)
	inc	hl
	dec	(hl)
	ld	a,(curmode)
	or	a
	jr	nz,close1
	call	ilprt
	defb	CR,LF,0
	ret
close1:
	ld	de,ocb
	call	fxwclose
	jp	z,wrterr
	xor	a
	ld	(opnflg),a
	ld	hl,crc32
	ld	de,crc
	scf
;	ld	bc,4 shl 8
	ld	bc,4 *256
crcclp:
	ld	a,(de)
	adc	a,(hl)
	push	af
	or	c
	ld	c,a
	pop	af
	inc	hl
	inc	de
	djnz	crcclp
	ld	a,c
	or	a
	jr	z,crcok
	call	ilprt
	defm	"CRC error"
	defb	CR,LF,0
	jr	wildck
;
crcok:
	call	ilprt
	defm	"CRC OK"
	defb	CR,LF,0
wildck:
	ld	hl,mtchfcb
	ld	bc,11
	ld	a,'?'
	cpir
	jp	nz,clsxit
	ret
;
getchla:

	call	getcode
	ld	(code),hl
	ld	a,(zipeof)
	and	1
	ret
;
savstk:
	ld	hl,(stackp)
	dec	hl
	ld	(stackp),hl
	ld	(hl),a
	ret
;
getcode:

	ld	a,(codesize)
readbits:

	ld	hl,8000h
bitlp:
	push	af
	push	hl
getbit:
	ld	hl,bleft
	dec	(hl)
	jp	m,readbt
	dec	hl
	rr	(hl)
	pop	hl
	rr	h
	rr	l
	pop	af
	dec	a
	jr	nz,bitlp
finbit:
	srl	h
	rr	l
	jr	nc,finbit
	ld	a,l
	ret
;
readbt:
	push	hl
	call	getbyte
	pop	hl
	ld	(hl),8
	dec	hl
	ld	(hl),a
	jr	getbit
;
scanfn:
	ld	a,(de)
	cp	'.'
	jr	z,nocopy
	or	a
	jr	z,nocopy
	inc	de
	dec	b
	jp	m,scanfn
	and	7fh
	ld	(hl),a
	inc	hl
	jr	scanfn
;
nocopy:
	dec	b
	ret	m
	ld	(hl),' '
	inc	hl
	jr	nocopy
;
ilprt:
	pop	hl
	call	pstr
	jp	(hl)
;
pstr:
	ld	a,(hl)
	or	a
	ret	z
	push	hl
	ld	e,a
	ld	c,dircon
	call	bdos
	pop	hl
	inc	hl
	jr	pstr
;
getstring:

	ld	a,h
	or	l
	ld	(de),a
	ret	z
	push	de
	push	hl
	call	getbyte
	pop	hl
	pop	de
	ld	(de),a
	inc	de
	dec	hl
	jr	getstring
;
skpstring:

	ld	a,h
	or	l
	ret	z
	push	hl
	call	getbyte
	pop	hl
	dec	hl
	jr	skpstring
;
getword:

	call	getbyte
	push	af
	call	getbyte
	pop	hl
	ld	l,h
	ld	h,a
	ret
;
getbyte:

	ld	a,(zipeof)
	and	1
	ld	a,CtrlZ
	ret	nz
	ld	a,(counting)
	or	a
	jr	z,skpdci
	ld	hl,(cs)
	ld	de,(cs + 2)
	ld	a,d
	or	e
	or	h
	or	l
	jr	nz,noteof
	ld	hl,zipeof
	inc	(hl)
	ld	a,CtrlZ
	ret
;
noteof:
	ld	a,h
	or	l
	dec	hl
	ld	(cs),hl
	jr	nz,skpdci
	dec	de
	ld	(cs + 2),de
skpdci:
	call	ckcon		; check console for abort
	ld	de,icb
	call	fxget
	ret	nz
	ld	a,CtrlZ
	ret
;
outb:
	ld	hl,(outpos)
	push	hl
	push	af
	ld	a,h
	and	1fh
	ld	h,a
	pop	af
	ld	de,outbuf
	add	hl,de
	ld	(hl),a
	pop	hl
	inc	hl
	ld	(outpos),hl
	push	af
	ld	a,h
	or	l
	jr	nz,nopos
	ld	hl,(outpos + 2)
	inc	hl
	ld	(outpos + 2),hl
nopos:
	pop	af
outbyte:

	push	af
	ld	c,a
	ld	a,(curmode)
	or	a
	call	nz,updcrc
	ld	hl,(ucs)
	ld	de,(ucs + 2)
	ld	a,h
	or	l
	dec	hl
	ld	(ucs),hl
	jr	nz,tsthl0
	dec	de
	ld	(ucs + 2),de
tsthl0:
	ld	a,h
	or	l
	or	d
	or	e
	jr	nz,noeof
	ld	hl,zipeof
	inc	(hl)
noeof:
	ld	a,(curmode)
	or	a
	jr	nz,noeof1
	pop	af
	ret
;
noeof1:
	pop	af
	ld	de,ocb
	call	fxput
	ret	nz
wrterr:
	call	ilprt
	defm	"Write Error (Disk full)"
	defb	CR,LF,0
	jp	ckcon0
;
;
updcrc:
	ld	hl,(crc32)
	ld	de,(crc32 + 2)
	ld	b,8
crclp:
	ld	a,l
	xor	c
	srl	c
	srl	d
	rr	e
	rr	h
	rr	l
	rra
	jr	nc,noxor
	ld	a,d
	xor	0edh
	ld	d,a
	ld	a,e
	xor	0b8h
	ld	e,a
	ld	a,h
	xor	83h
	ld	h,a
	ld	a,l
	xor	20h
	ld	l,a
noxor:
	djnz	crclp
	ld	(crc32),hl
	ld	(crc32 + 2),de
	ret
;
IF !NOSHRINK
unshrink:
	ld	a,init_bits
	ld	(codesize),a
;	ld	hl,+(1 shl init_bits) - 1;
	ld	hl,+(2 ** init_bits) - 1;
	ld	(maxcode),hl
	ld	hl,first_ent
	ld	(free_ent),hl
	ld	hl,prefix_of
	ld	de,prefix_of + 1
	ld	bc,512
	ld	(hl),c
IF Z80
		ldir
ELSE
		call	ldir
ENDIF
	ld	bc,16386 - 512
	ld	(hl),-1
IF Z80
		ldir
ELSE
		call	ldir
ENDIF
	ld	hl,suffix_of
sol:
	ld	(hl),c
	inc	hl
	inc	c
	jr	nz,sol
	call	getchla
	ld	(oldcode),hl
	ret	nz
	ld	a,l
	ld	(finchar),a
	call	outbyte
unshlp:
	ld	hl,stack
	ld	(stackp),hl
	ld	a,(zipeof)
	and	1
	ret	nz
clrlp:
	call	z,getchla
	ret	nz
	ld	a,h
	dec	a
	or	l
	jr	nz,noclr
	call	getchla
	ld	a,h
	or	a
	jr	nz,clrlp
	dec	l
	jr	z,bumpcs
	dec	l
	call	z,partial_clear
	jr	clrlp
;
bumpcs:
	ld	hl,codesize
	inc	(hl)
	ld	a,(hl)
	cp	max_bits
	ld	hl,maxcmax
	jr	z,atmax
	ld	hl,1
maxclp:
	add	hl,hl
	dec	a
	jr	nz,maxclp
	dec	hl
atmax:
	ld	(maxcode),hl
	jr	clrlp
;
noclr:
	ld	(incode),hl
	add	hl,hl
	ld	de,prefix_of
	add	hl,de
	ld	a,(hl)
	inc	hl
	and	(hl)
	inc	a
	ld	hl,(code)
	jr	nz,noKwKw
	ld	a,(finchar)
	call	savstk
	ld	hl,(oldcode)
noKwKw:
	ex	de,hl
staklp:
 ld	hl,suffix_of
	add	hl,de
	ld	a,(hl)
	call	savstk
	ld	hl,100h
	or	a
	sbc	hl,de
	jr	nc,unstak
	ld	hl,prefix_of
	add	hl,de
	add	hl,de
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	jr	staklp
;
unstak:
	ld	(finchar),a
	ld	de,(stackp)
unslp:
	ld	hl,stack
	or	a
	sbc	hl,de
	jr	z,newent
	ld	a,(de)
	inc	de
	push	de
	call	outbyte
	pop	de
	jr	unslp
;
newent:
	ld	hl,(free_ent)
	ld	(code),hl
	ex	de,hl
	ld	hl,1fffh
	or	a
	sbc	hl,de
	jr	c,full
	ld	hl,prefix_of
	add	hl,de
	add	hl,de
	ld	bc,(oldcode)
	ld	(hl),c
	inc	hl
	ld	(hl),b
	ld	hl,suffix_of
	add	hl,de
	ld	a,(finchar)
	ld	(hl),a
getfre:
	inc	de
	ld	hl,1fffh
	or	a
	sbc	hl,de
	jr	c,full1
	ld	hl,prefix_of
	add	hl,de
	add	hl,de
	ld	a,(hl)
	inc	hl
	and	(hl)
	inc	a
	jr	nz,getfre
full1:
	ld	(free_ent),de
full:
	ld	hl,(incode)
	ld	(oldcode),hl
	jp	unshlp
;

partial_clear:
	ld	de,first_ent
l8:
	ld	hl,(free_ent)
	or	a
	sbc	hl,de
	jr	z,br8
	ld	hl,prefix_of + 1
	add	hl,de
	add	hl,de
	set	7,(hl)
	inc	de
	jr	l8
;
br8:
	ld	de,first_ent
l9:
	ld	hl,(free_ent)
	or	a
	sbc	hl,de
	jr	z,br9
	ld	hl,prefix_of
	add	hl,de
	add	hl,de
	push	de
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	res	7,d
	ld	hl,first_ent - 1
	or	a
	sbc	hl,de
	jr	nc,ei10
	ld	hl,prefix_of + 1
	add	hl,de
	add	hl,de
	res	7,(hl)
ei10:
	pop	de
	inc	de
	jr	l9
;
br9:
	ld	de,first_ent
l10:
	ld	hl,(free_ent)
	or	a
	sbc	hl,de
	jr	z,br10
	ld	hl,prefix_of + 1
	add	hl,de
	add	hl,de
	bit	7,(hl)
	jr	z,ei11
	ld	(hl),-1
	dec	hl
	ld	(hl),-1
ei11:
	inc	de
	jr	l10
;
br10:
	ld	de,first_ent
l11:
	ld	hl,maxcmax
	or	a
	sbc	hl,de
	jr	z,br11
	ld	hl,prefix_of
	add	hl,de
	add	hl,de
	ld	a,(hl)
	inc	hl
	and	(hl)
	inc	a
	jr	z,br11
	inc	de
	jr	l11
br11:
	ld	(free_ent),de
	ret
ENDIF

;
IF !NOREDUCE
loadfollowers:
	ld	hl,Slen + 255
	ld	b,0
lflp:
	push	bc
	push	hl
	ld	a,6
	call	readbits
	pop	hl
	pop	de
	ld	(hl),a
	push	de
	push	hl
	dec	d
	ld	hl,followers
	call	shftadd
	ld	b,a
	or	a
	jr	z,nofoll
ldfllp:
	push	hl
	push	bc
	ld	a,8
	call	readbits
	pop	bc
	pop	hl
	ld	(hl),a
	inc	hl
	djnz	ldfllp
nofoll:
	pop	hl
	pop	bc
	dec	hl
	djnz	lflp
	ret

unreduce:
	ld	e,a
	ld	d,0
	ld	hl,_L_table
	add	hl,de
	ld	a,(hl)
	ld	(L_table),a
	ld	hl,_D_shift
	add	hl,de
	ld	a,(hl)
	ld	(D_shift),a
	xor	a
	ld	(ExState),a
	ld	(lchar),a
	call	loadfollowers
ur1:
	ld	a,(zipeof)
	and	1
	ret	nz
	call	slenlch
	or	a
	jr	nz,ur2
ur4:
	ld	a,8
	call	readbits
	jr	ur3
;
ur2:
	ld	a,1
	call	readbits
	dec	l
	jr	z,ur4
	call	slenlch
	dec	a
	or	1
	ld	l,a
	xor	a
btlp:
	inc	a
	srl	l
	jr	nz,btlp
	call	readbits
	ld	de,followers
	add	hl,de
	ld	de,(lchar - 1)
	call	shftadd
	ld	a,(hl)
ur3:
	ld	(nchar),a
	ld	l,a
	ld	a,(ExState)
	or	a
	jr	nz,ur5
	ld	a,l
	cp	DLE
	jr	nz,ur9
	ld	a,1
	ld	(ExState),a
	jr	ur6
;
ur5:
	dec	a
	jr	nz,ur7
	ld	a,l
	or	a
	jr	z,ur10
	ld	(_V),a
	ld	a,(L_table)
	ld	h,a
	and	l
	cp	h
	ld	l,a
	ld	h,0
	ld	(Len),hl
	jr	nz,ur12
	ld	a,2
	jr	ur11
;
ur10:
	ld	(ExState),a
	ld	a,DLE
ur9:
	call	outb
	jr	ur6
;
ur7:
	dec	a
	jr	nz,ur8
	ld	a,l
	ld	hl,Len
	add	a,(hl)
	ld	(hl),a
	jr	nc,ur12
	inc	hl
	inc	(hl)
ur12:
	ld	a,3
	jr	ur11
;
ur8:
	dec	a
	jr	nz,ur13
	ld	a,(D_shift)
	ld	b,a
	ld	a,(_V)
ur14:
	srl	a
	djnz	ur14
	ld	h,a
	inc	hl
	ld	bc,(Len)
	inc	bc
	inc	bc
	inc	bc
	call	callback
ur13:
	xor	a
ur11:
	ld	(ExState),a
ur6:
	ld	a,(nchar)
	ld	(lchar),a
	jp	ur1
;
slenlch:
	ld	hl,(lchar)
	ld	h,0
	ld	de,Slen
	add	hl,de
	ld	a,(hl)
	ret
ENDIF			; NOREDUCE

;
shftadd:
	ld	e,0
	ld	b,3

shftloop:
	srl	d
	rr	e
	djnz	shftloop
	add	hl,de
	ret
;
callback:
	push	bc
	push	hl
	ld	hl,(outpos)
	ld	de,(outpos + 2)
	pop	bc
	or	a
	sbc	hl,bc
	jr	nc,cb2
	dec	de

cb2:
	pop	bc
cb3:
	bit	7,d
	jr	z,cb4
	ld	a,b
	or	c
	jr	z,cb4
	xor	a
	call	outbp
	inc	hl
	ld	a,h
	or	l
	jr	nz,cb5
	inc	de
cb5:
	dec	bc
	jr	cb3
;
cb4:
	ex	de,hl
cb6:
	ld	a,b
	or	c
	ret	z
	ld	a,d
	and	1fh
	ld	d,a
	ld	hl,outbuf
	add	hl,de
	ld	a,(hl)
	call	outbp
	inc	de
	dec	bc
	jr	cb6
;
outbp:
	push	hl
	push	de
	push	bc
	call	outb
	pop	bc
	pop	de
	pop	hl
	ret
;
readlengths:
	ld	a,8
	call	readbits
	ld	d,h
	ld	e,d
	inc	hl
	ld	b,h
	ld	c,l
	ld	(ix + _maxlength),e
	ld	(ix + _maxlength + 1),d
	push	ix
	pop	hl
	inc	hl
	inc	hl
	inc	hl
rl1:
	ld	a,b
	or	c
	ret	z
	push	bc
	push	de
	push	hl
	ld	a,4
	call	readbits
	inc	a
	push	af
	ld	a,4
	call	readbits
	inc	a
	ld	b,a
	pop	af
	ld	c,a
	pop	hl
	pop	de
	ld	a,(ix + _maxlength)
	cp	c
	jr	nc,rl2
	ld	(ix + _maxlength),c
rl2:
	inc	hl
	inc	hl
	inc	hl
	ld	(hl),e
	inc	hl
	ld	(hl),c
	inc	e
	djnz	rl2
	pop	bc
	dec	bc
	jr	rl1
;
sortlengths:
	ld	h,(ix + _entries + 1)
	ld	l,(ix + _entries)
	ld	b,h
	ld	c,l
	ld	(entrs),hl
sl7:
	srl	b
	rr	c
sl1:
	ld	a,b
	or	c
	ret	z
	ld	(noswps),a
	push	ix
	ld	de,4
	add	ix,de
	push	ix
	pop	iy
	add	iy,bc
	add	iy,bc
	add	iy,bc
	add	iy,bc
	ld	hl,(entrs)
	or	a
	sbc	hl,bc
sl2:
	ld	a,(ix + _bitlength)
	cp	(iy + _bitlength)
	jr	c,sl4
	jr	nz,sl3
	ld	a,(iy + _value)
	cp	(ix + _value)
	jr	nc,sl4
sl3:
	ld	d,e
sl5:
	ld	a,(ix)
	push	af
	ld	a,(iy)
	ld	(ix),a
	pop	af
	ld	(iy),a
	inc	ix
	inc	iy
	dec	d
	jr	nz,sl5
	ld	a,d
	ld	(noswps),a
	jr	sl6
;
sl4:
	add	ix,de
	add	iy,de
sl6:
	dec	hl
	ld	a,h
	or	l
	jr	nz,sl2
	pop	ix
	ld	a,(noswps)
	or	a
	jr	nz,sl7
	jr	sl1
;
generatetrees:
	ld	l,(ix + _entries)
	ld	h,(ix + _entries + 1)
	ld	c,l
	ld	b,h
	push	ix
	pop	de
	add	hl,hl
	add	hl,hl
	add	hl,de
	push	hl
	pop	iy
	xor	a
	ld	d,a
	ld	e,a
	ld	h,a
	ld	l,a
	ld	(lbl),a
gt1:
	ld	a,b
	or	c
	ret	z
	dec	bc
	add	hl,de
	ld	a,(lbl)
	cp	(iy + _bitlength)
	jr	z,gt2
	ld	a,(iy + _bitlength)
	ld	(lbl),a
	sub	16
	ex	de,hl
	ld	hl,1
	jr	z,gt3
gt4:
	add	hl,hl
	inc	a
	jr	nz,gt4
gt3:
	ex	de,hl
gt2:
	ld	(iy + _code),l
	ld	(iy + _code + 1),h
	push	de
	ld	de,-4
	add	iy,de
	pop	de
	jr	gt1
;
ldtrees:
	ld	a,(gpbf)
	rra
	ld	l,a
	and	1
	add	a,6
	ld	(dictb),a
	ld	a,l
	rra
	and	1
	ld	(ltp),a
	set	1,a
	ld	(mml),a
	ld	ix,lit_tree
	ld	hl,256
	call	nz,ld_tree
	ld	hl,64
	ld	ix,len_tree
	call	ld_tree
	ld	hl,64
	ld	ix,dist_tre
ld_tree:
	ld	(ix + _entries),l
	ld	(ix + _entries + 1),h
	call	readlengths
	call	sortlengths
	call	generatetrees
reversebits:
	push	ix
	pop	hl
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
rb1:
	inc	hl
	inc	hl
	inc	hl
	ld	c,(hl)
	ld	b,8
rb2:
	srl	c
	adc	a,a
	djnz	rb2
	push	af
	inc	hl
	ld	c,(hl)
	ld	b,8
rb3:
	srl	c
	adc	a,a
	djnz	rb3
	dec	hl
	ld	(hl),a
	pop	af
	inc	hl
	ld	(hl),a
	dec	de
	ld	a,d
	or	e
	jr	nz,rb1
	ret
;
readtree:
	push	ix
	pop	iy
	ld	de,4
	add	iy,de
	ld	b,d
	ld	e,d
	ld	h,d
	ld	l,d
rt1:
	push	hl
	push	de
	push	bc
	ld	a,1
	call	readbits
	pop	af
	push	af
	or	a
	jr	z,rt2
rt3:
	add	hl,hl
	dec	a
	jr	nz,rt3
rt2:
	pop	bc
	pop	de
	add	hl,de
	ex	de,hl
	inc	b
	pop	hl
rt4:
	ld	a,(iy + _bitlength)
	cp	b
	jr	nc,rt5
	push	de
	ld	de,4
	add	iy,de
	pop	de
	inc	hl
	ld	a,(ix + _entries)
	sub	l
	jr	nz,rt4
	ld	a,(ix + _entries + 1)
	sub	h
	jr	nz,rt4
rt6:
	dec	a
	ret
;
rt5:
	ld	a,(iy + _bitlength)
	cp	b
	jr	nz,rt1
	ld	a,(iy + _code)
	cp	e
	jr	nz,rt7
	ld	a,(iy + _code + 1)
	cp	d
	jr	nz,rt7
	ld	a,(iy + _value)
	ret
;
rt7:
	push	de
	ld	de,4
	add	iy,de
	pop	de
	inc	hl
	ld	a,(ix + _entries)
	sub	l
	jr	nz,rt5
	ld	a,(ix + _entries + 1)
	sub	h
	jr	nz,rt5
	jr	rt6
;
unimplode:
	call	ldtrees
ui1:
	ld	a,(zipeof)
	and	1
	ret	nz
	inc	a
	call	readbits
	or	a
	jr	z,ui2
	ld	a,(ltp)
	or	a
	jr	z,ui3
	ld	ix,lit_tree
	call	readtree
	jr	ui4
;
ui3:
	ld	a,8
	call	readbits
ui4:
	call	outb
	jr	ui1
;
ui2:
	ld	a,(dictb)
	call	readbits
	push	hl
	ld	ix,dist_tre
	call	readtree
	ld	bc,(dictb - 1)
ui5:
	add	hl,hl
	djnz	ui5
	pop	bc
	add	hl,bc
	push	hl
	ld	ix,len_tree
	call	readtree
	ld	l,a
	ld	h,0
	cp	63
	jr	nz,ui6
	push	hl
	ld	a,8
	call	readbits
	pop	de
	add	hl,de
ui6:
	ld	de,(mml)
	ld	d,0
	add	hl,de
	ld	b,h
	ld	c,l
	pop	hl
	inc	hl
	call	callback
	jr	ui1
;
; ckcon -- checks console for character; aborts if ^C
;
ckcon:
	ld	hl,conckct
	dec	(hl)		; decrement console check counter
	ld	a,(hl)
	and	7fh
	ret	nz		; only check every 128 calls
	ld	e,0FFh		; check for character
	ld	c,dircon
	call	bdos
	or	a
	ret	z
	cp	CtrlC		; ^C ?
	ret	nz		; (no, continue)
	ld	a,(opnflg)	; is a file open?
	or	a
	jr	z,ckcon1	; (no)
ckcon0:
	call	setout
	ld	de,opfcb
	call	f_close		; close it
	call	f_delete	; and delete it
	call	ilprt
	defm	"Partial file erased --"
	defb	0
ckcon1:
	call	ilprt
	defm	" Aborted"
	defb	0
	jp	clsxit
;
; getusr -- stuffs current user into default FCBs if not ZCPR3 (for Z3LOG)
;
getusr:
	ld	hl,(Z3EAdr)	; ZCPR3?
	ld	a,h
	or	l
	ret	nz		; (yes, skip this)
	ld	c,setusr
	ld	e,0ffh
	call	bdos
	ld	(dfcb+13),a
	ld	(altfcb+13),a
	ret
;
; setout -- set output drive/user
;
setout:
	ld	de,altfcb
	jp	z3log
	
;
; usage -- show syntax for ZCPR3 ("dir:") or vanilla CP/M ("d:")
;
usage:
IF !SILENT
	call	ilprt
	defm	"Usage:"
	defb	CR,LF
	defm	"   UNZIP {d"
	defb	0
	ld	hl,(Z3EAdr)
	ld	a,h
	or	l
	push	af
	jr	z,usage2
	call	ilprt
	defm	"ir"
	defb	0
usage2:
	call	ilprt
	defm	":}zipfile {d"
	defb	0
	pop	af
	jr	z,usage3
	call	ilprt
	defm	"ir"
	defb	0
usage3:
	call	ilprt
	defm	":}{afn.typ}"
	defb	CR,LF
	defm	"If a destination is given, files are extracted."
	defb	CR,LF
	defm	"If not, filenames are listed."
	defb	CR,LF
	defb	0
ELSE
	call	ilprt
	defm	"Parameter error."
	defb	CR,LF
	defb	0
ENDIF
	jp	exit
	
IF Z80
		; On a Z80 LDIR is directly used in place of this subroutine
ELSE
ldir:
		LD 	a,(HL)
		LD	(DE),A
		INC	HL
		INC	DE
		DEC	BC
		LD 	a,b
		OR	c
		jr	NZ,ldir
		ret
ENDIF

;
; data storage . . .  (original position: $0BEE)
;
init:

	defb	0
	defb	1
	defw	0,0
	defw	-1,-1
endinit:

_L_table:

	defb	7fh, 3fh, 1fh, 0fh
_D_shift:

	defb	07h, 06h, 05h, 04h

; (original position: $0C00)
memry:
	defw	data_end


;-----------------------------------------------


fxropen:
  PUSH BC
  PUSH HL
  PUSH DE
  CALL save_vars
  XOR A
  LD (opn_flg),A
  CALL get_uid_drv
  LD (uid_drv),BC
  EX DE,HL
  CALL initfcb
fxropen_1:
  CALL bdos_open
  JR NZ,bdos_error
  CALL file_bottom
fx_exit:
  CALL restore_vars
  OR $FF
  JR opn_exit
 
 
fxwopen:
  PUSH BC
fxwopen_1:
  PUSH HL
  PUSH DE
  CALL save_vars
  PUSH HL
  CALL init_wr_pos
  CALL get_uid_drv
  LD (uid_drv),BC
  POP DE
  CALL initfcb
  CALL bdos_replace
  INC A
fxwopen_2:
  JR NZ,fx_exit
bdos_error:
  XOR A
  JR opn_exit


fxget:
  PUSH BC
  PUSH HL
  PUSH DE
  EX DE,HL
  INC HL
  INC HL
  INC HL
  INC HL
  LD C,(HL)
  INC HL
  LD B,(HL)
  LD A,B
  OR C
  JR Z,get_byte
  DEC BC
  INC HL
  LD E,(HL)
  INC HL
  LD D,(HL)
  LD A,(DE)
  LD (cur_byte),A

get_put_exit:
  INC DE
  LD (HL),D
  DEC HL
  LD (HL),E
  DEC HL
  LD (HL),B
  DEC HL
  LD (HL),C
  JR get_put_exit_2
  
get_byte:
  POP DE
  PUSH DE
  CALL save_vars
  CALL restore_uid_drv
  CALL file_bottom
  JR NZ,bdos_error
  LD HL,(buf_ptr)
  LD A,(HL)
  LD (cur_byte),A
  INC HL
  LD (buf_ptr),HL
  LD HL,(byte_count)
  DEC HL
  LD (byte_count),HL
get_put_restore:
  CALL restore_vars
get_put_exit_2:
  OR $FF
  LD A,(cur_byte)

opn_exit:
  POP DE
  POP HL
  POP BC
  RET


fxput:
  PUSH BC
  PUSH HL
  PUSH DE
  LD (cur_byte),A
  EX DE,HL
  INC HL
  INC HL
  INC HL
  INC HL
  LD C,(HL)
  INC HL
  LD B,(HL)
  LD A,B
  OR C
  JR Z,put_byte
  DEC BC
  INC HL
  LD E,(HL)
  INC HL
  LD D,(HL)
  LD A,(cur_byte)
  LD (DE),A
  JR get_put_exit
  
put_byte:
  POP DE
  PUSH DE
  CALL save_vars
  CALL restore_uid_drv
  CALL set_wr_pos
  JR NZ,bdos_error
  LD HL,(buf_ptr)
  LD A,(cur_byte)
  LD (HL),A
  INC HL
  LD (buf_ptr),HL
  LD HL,(byte_count)
  DEC HL
  LD (byte_count),HL
  JR get_put_restore


fxrclose:
  PUSH BC
  PUSH HL
  PUSH DE
  CALL save_vars
  CALL restore_uid_drv
fxrclose_1:
  LD DE,(rc_ptr)
  CALL f_close
  JP NZ,bdos_error
  JP fx_exit


fxwclose:
  PUSH BC
  PUSH HL
  PUSH DE
  CALL save_vars
  CALL restore_uid_drv
fxwclose_1:
  LD HL,(byte_count)
  LD A,L
  AND $7F
  JR Z,fxwclose_2
  DEC HL
  LD (byte_count),HL
  LD HL,(buf_ptr)
  LD (HL),$1A
  INC HL
  LD (buf_ptr),HL
  JR fxwclose_1
fxwclose_2:
  CALL set_wr_pos
  JR fxrclose_1

save_vars:
  EX DE,HL
  LD (varptr),HL
  LD DE,fx_vars
  LD BC,$000A
IF Z80
		ldir
ELSE
		call	ldir
ENDIF
  LD (rc_ptr),HL
  RET
  
restore_vars:
  LD DE,(varptr)
  INC DE
  LD HL,uid_drv
  LD BC,$0007
IF Z80
		ldir
ELSE
		call	ldir
ENDIF
  RET

file_bottom:
  LD A,(opn_flg)
  OR A
  RET NZ
  LD HL,(buf_ptr2)
  LD A,(sectors)
  LD B,A
rc_loop:
  CALL bdos_sdma
  LD DE,(rc_ptr)
  CALL rd_next_rc
  JR NZ,set_rd_pos
  LD DE,$0080
  ADD HL,DE
  DJNZ rc_loop
  JR set_rd_pos_1
  
set_rd_pos:
  LD A,$FF
  LD (opn_flg),A
set_rd_pos_1:
  LD HL,(buf_ptr2)
  LD (buf_ptr),HL
  LD A,(sectors)
  SUB B
  LD H,A
  LD L,$00
  SRL H		; HL=sectors*128
  RR L
  LD (byte_count),HL
  PUSH HL
  LD HL,$0080
  CALL bdos_sdma
  POP HL
  LD A,H
  OR L
  JR Z,ret_byte
ret_zero:
  XOR A
  RET
  
set_wr_pos:
  LD HL,(byte_count)
  ADD HL,HL
  LD A,(sectors)
  SUB H
  LD B,A
  LD HL,(buf_ptr2)
set_wr_pos_1:
  LD A,B
  OR A
  JR Z,init_wr_pos
  DEC B
  CALL bdos_sdma
  LD DE,(rc_ptr)
  CALL wr_next_rc
  LD DE,$0080
  ADD HL,DE
  OR A
  JR Z,set_wr_pos_1
ret_byte:
  OR $FF
  RET

init_wr_pos:
  LD HL,(buf_ptr2)
  LD (buf_ptr),HL
  XOR A
  LD (opn_flg),A
  LD A,(sectors)
  LD H,A
  LD L,$00
  SRL H
  RR L
  LD (byte_count),HL
  LD HL,$0080
  CALL bdos_sdma
  JR ret_zero
  
restore_uid_drv:
  LD BC,(uid_drv)
  JP set_uid_drv


; log in drive/user, de=fcb
; de=fcb
;
z3log:
  PUSH HL
  PUSH BC
  PUSH AF
  CALL get_uid_drv
  INC B
  LD A,(DE)
  OR A
  JR Z,z3log1
  LD B,A
z3log1:
  DEC B
  LD HL,$000D
  ADD HL,DE
  LD C,(HL)
  CALL set_uid_drv
  POP AF
  POP BC
  POP HL
  RET

set_uid_drv:
  PUSH AF
  PUSH DE
  PUSH HL
  PUSH BC
  LD E,C
  LD C,$20		; set/get user id
  CALL bdos_c
  LD E,B
  LD C,$0E		; log in and select disk
  CALL bdos_c
  POP BC
  POP HL
  POP DE
  POP AF
  RET

get_uid_drv:
  PUSH AF
  PUSH DE
  PUSH HL
  LD C,$19		; interrogate drive number
  CALL bdos_c
  PUSH AF
  LD E,$FF
  LD C,$20		; set/get user id
  CALL bdos_c
  LD C,A
  POP AF
  LD B,A
  POP HL
  POP DE
  POP AF
  RET

bdos_replace:
  CALL f_delete
  LD A,$16		; create file
  CALL bdos_a
  RET M
  CP A
  RET

f_close:
  LD A,$10		; close file
  JP bdos_sub

f_delete:
  LD A,$13		; delete file
  JP bdos_sub

bdos_open:
  LD A,$0F		; open file
bdos_sub:
  CALL bdos_a
  RET M
  XOR A
  RET

rd_next_rc:
  LD A,$14		; read next record
  JP bdos_a
  
wr_next_rc:
  LD A,$15		; write next record
  JP bdos_a

; Use HL to define the buffer read position
bdos_sdma:
  PUSH AF
  LD A,$1A		; set DMA address for i/o
  EX DE,HL
  CALL bdos_a
  EX DE,HL
  POP AF
  RET

initfcb:
  PUSH HL
  PUSH DE
  PUSH AF
  EX DE,HL
  LD DE,$000C
  LD (HL),D
  ADD HL,DE
  LD E,$15
initfcb_loop:
  LD (HL),D
  INC HL
  DEC E
  JR NZ,initfcb_loop
  POP AF
  POP DE
  POP HL
  RET

bdos_c:
  PUSH DE
  PUSH BC
  CALL bdos
  POP BC
  POP DE
  OR A
  RET

bdos_a:
  PUSH HL
  PUSH DE
  PUSH BC
  LD C,A
  CALL bdos
  OR A
  POP BC
  POP DE
  POP HL
  RET



;
; uninitialized storage
;
;	dseg
;SECTION data
DEFVARS ASMPC+100h   ; Dynamic data structure
{
;$0E59
mode      ds.b	1
zipeof    ds.b	1
counting  ds.b	1
junk      ds.b	STRSIZ
lfh       ds.b	0
vnte      ds.b	2
gpbf      ds.b	2
cm        ds.b	2
lmft      ds.b	2
lmfd      ds.b	2
crc       ds.b	4
cs        ds.b	4
ucs       ds.b	4
fnl       ds.b	2
efl       ds.b	2
endlfh    ds.b	1
icb       ds.b	8
ipbuf     ds.b	2
infcb     ds.b	36
ocb       ds.b	8
opbuf     ds.b	2
opfcb     ds.b	1		; output file control block
opfn      ds.b	8
opext     ds.b	3+24
bitbuf    ds.b	1
mtchfcb   ds.b	11+1

vars      ds.b	0
bleft     ds.b	1
nodest    ds.b	1
outpos    ds.b	4
crc32     ds.b	4
curmode   ds.b	1
opnflg    ds.b	1
conckct   ds.b	1
L_table   ds.b	1
D_shift   ds.b	1
_V        ds.b	1
nchar     ds.b	1
lchar     ds.b	1
ExState   ds.b	1
Len       ds.b	2
ltp       ds.b	1
mml       ds.b	1
dictb     ds.b	1
noswps    ds.b	1
entrs     ds.b	2
lbl       ds.b	1
oldcode   ds.b	2
offset    ds.b	2
codesize  ds.b	1
maxcode   ds.b	2
free_ent  ds.b	2
finchar   ds.b	1
stackp    ds.b	2
incode    ds.b	2
code      ds.b	2
	
outbuf    ds.b	0
suffix_of ds.b	8192

prefix_of ds.b	0
Slen      ds.b  0
lit_tree  ds.b	_sf_tree_
len_tree  ds.b	_sf_tree_
dist_tre  ds.b	_sf_tree_
_dist_tre ds.b	16384 + 2 - (3 * _sf_tree_)
followers ds.b	8192

stack     ds.b	60
; ----------  ----------  ----------  ----------  ----------
locstk    ds.b	0		; Initial SP position on the stack
; ----------  ----------  ----------  ----------  ----------

;----------   fx variables (file control library) ----------
cur_byte    ds.b	1

varptr      ds.w	1

fx_vars     ds.b	0
sectors     ds.b	1
uid_drv     ds.w	1
opn_flg     ds.b	1
byte_count  ds.w	1
buf_ptr     ds.w	1
buf_ptr2    ds.w	1
rc_ptr      ds.w	1

; ----------  ----------  ----------  ----------  ----------
data_end    ds.b    0	; Free memory
; ----------  ----------  ----------  ----------  ----------
}

