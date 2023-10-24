;
;	ZCAT - Expand GZIP text files on the fly
;   and print them on screen.
;
;
; This is a z88dk-z80asm conversion of UNZIP156
; later heavily edited to work on GZIP archives.

;
; Can be assembled with z88dk-z80asm with:
;
;    z80asm -b -oZCAT.COM zcat.asm

; Opt flags:


FASTQUIT  equ	1	; Quit as soon as end of data section
CCPQUIT   equ   1	; Returns to CCP instead of WARM


;
; System addresses
;
defc wboot	=	0
defc bdos	=	5
defc infcb	=	5Ch
defc altfcb	=	6Ch
;
; BDOS service functions
;
defc conout	=	2
defc dircon	=	6
defc fopen	=	15
defc fclose	=	16
defc ferase	=	19
defc fread	=	20
defc fwrite	=	21
defc fmake	=	22
defc getdrv	=	25
defc setdma	=	26
defc setusr	=	32
;
; Other
;
defc STRSIZ		=	256		; must be 256 exactly, see plfh for why
defc DLE		=	144
defc max_bits	=	13
defc init_bits 	=	9
;defc hsize		=	8192
defc first_ent	=	257
;defc clear		=	256
defc maxcmax	=	1 << max_bits
defc maxSF		=	256
;defc _entry		=	4
defc _sf_tree_ 	=	4 + 4 * maxSF
defc maxcl		=	16
defc nrcl		=	19
defc nrlit		=	288
defc nrdist		=	32

;; ; index offsets
;; defc _code		=	0
;; defc _value		=	2
;; defc _bitlength =	3
;; defc _entries 	=	0
;; defc _maxlength =	2

;
; ASCII
;
defc CtrlC	=	03h
defc CR		=	0Dh
defc LF		=	0Ah
defc CtrlZ	=	1Ah



;
;	aseg
	org	100h
;
	jp	start

;
	defm	"Z3ENV"
	defb	1

Z3EAdr:	defw	0
;
start:	ld	(oldstk),sp	; save old stack here for future use
if CCPQUIT
	ld	hl,(6)		; get top of memory
	ld	de,0f800h	; allow space for CCP
	add	hl,de
	ld	sp,hl		; set the stack pointer
else
	ld	sp,(6)		; set the stack pointer
endif
	ld	a,(infcb+1)	; filename?
	cp	' '
	jp	z,usage		; no filename - display usage
	cp	'/'
	jp	z,usage		; UNZIP /?  - display usage
;
; Check TPA size (this will need adjusting if warm-boot-only exit
; is changed).
;
	ld	hl,-128		; allow for a decent stack size
	add	hl,sp
	ld	de,endaddr
	or	a
	sbc	hl,de		; check endaddr is less (i.e. hl is >=)
	jp	c,nomem
;
	ld	hl,0080h	; command buffer
	ld	l,(hl)		; get length
	set	7,l		; point after end of line
	inc	hl
	ld	(hl),0		; put terminator in
	ld	l,81h		; hl=>start of line
optlp1:	ld	b,h		; b=0, options
	ld	a,(hl)		; get character
	inc	hl
	cp	' '
	jr	z,optlp1	; skip spaces
	jr	c,optdone	; end of command line
	cp	'['
	jr	z,options	; start of options string
optlp3:	ld	a,(hl)
	inc	hl
	cp	' '
	jr	z,optlp1
	jr	nc,optlp3	; skip filename
	jr	optdone		; end of command line
options:
	ld	a,(hl)
	inc	hl
	cp	' '
	jr	z,optlp1	; another filename
	cp	'?'
	jp	z,usage		; UNZIP [?] - display usage
	and	05Fh		; upper case
	jr	z,optdone	; end of command line
	cp	']'
	jr	nc,optdone	; end of options
	or	b
	ld	b,a
	jr	options
optdone:
	ld	(opts-1),bc	; note options
; bit 1 = [O]verwrite
; bit 4 = [Q]uiet
;
	ld	de,altfcb
	ld	a,(de)		; output drive given?
	ld	(opfcb),a	; store it in output file control block
	ld	(mode),a	; set the mode (non-zero = extract)
	call	getusr		; get input and output users, if ZCPR3
	ld	hl,mtchfcb
	ld	bc,11
	inc	de
	ld	a,(de)
	cp	20h
	jr	z,wildfill
	ex	de,hl
	ldir
	ld	a,(altfcb)
	or	a
	jr	nz,filldn
	ld	c,getdrv
	call	bdos
	inc	a
	ld	(opfcb),a	; store it in output file control block
	ld	(mode),a	; set the mode (non-zero = extract)
	jr	filldn
wildfill:
	ld	b,c
wildlp:	ld	(hl),'?'
	inc	hl
	djnz	wildlp
filldn:	ld	a,(infcb+9)	; check for filetype
;	cp	20h
;	jr	nz,wasext
;	ld	hl,+('I' << 8) + 'Z'	; set default type to ZIP
;	ld	(infcb+9),hl
;	ld	a,'P'
;	ld	(infcb+11),a
wasext:	call	setin		; log input user
	ld	de,infcb
	ld	c,fopen
	call	bdos		; try and open ZIP file
	inc	a
	jr	nz,openok	; ok
	call	ilprt0
	defm	"Zipfile not found."
	defb	CR,LF,0
	jr	exit
;

nomem:	call	ilprt0		; complain and fall through to exit
	defm	"Not enough memory."
	defb	CR,LF,0
;
; All exits point here for possible future enhancements, such
; as elimination of warm boot.
;
exit:	
if CCPQUIT
	ld	sp,(oldstk)
	ret
else
	jp	wboot
endif
;
sigerr:	call	ilprt0
	defm	"Invalid GZip file."
	defb	CR,LF,0
	jr	exit


;
;  Verify we have a valid GZip archive
;
openok:
	call	getword
	ld	de,-((0x8b << 8) + 0x1f)		; magic number
	add	hl,de
	ld	a,h
	or	l
	jr	nz,sigerr

	call	getbyte     ; CM (Compression Method)
	sub     8           ; il must be 8 (Deflate)
	jr	nz,sigerr

	call	getbyte		; File Flags  (see table below)

	call	getword		; 32-bit timestamp
	call	getword

	call	getbyte		; Compression flags
;	push    af
	call	getbyte		; Operating system (see table below)
;	pop     af
	
;	and     4 ; FEXTRA?
;    ... if so we should skip the extra field


;  We have the original filename here, let's skip it for now
fnameloop:
	call	getbyte
	and     a
	jr nz,fnameloop


	call	undeflate

	jp	exit


; File Flags 
; -----------------------------
; 0x01  FTEXT      If set the uncompressed data needs to be treated as text instead of binary data.
;                  This flag hints end-of-line conversion for cross-platform text files but does not enforce it.
; 0x02  FHCRC      The file contains a header checksum (CRC-16)
; 0x04  FEXTRA     The file contains extra fields
; 0x08  FNAME      The file contains an original file name string
; 0x10  FCOMMENT   The file contains comment
; 0x20  Reserved
; 0x40  Reserved
; 0x80  Reserved


; Operating System flags
; -----------------------------
; 0    FAT filesystem (MS-DOS, OS/2, NT/Win32)
; 1    Amiga
; 2    VMS (or OpenVMS) 
; 4    VM/CMS
; 5    Atari TOS
; 6    HPFS filesystem (OS/2, NT)
; 7    Macintosh
; 8    Z-System
; 9    CP/M
; 10   TOPS-20
; 11   NTFS filesystem (NT)
; 12   QDOS
; 13   Acorn RISCOS
; 255  unknown




;
rd16bits:
	ld	a,16
;
readbits:
	ld	b,a
	ld	c,80h		; bits rotate into C and A
	xor	a		; (rra is 4 cycles vs 8 for others)
	ld	hl,(bitbuf)	; keep bitbuf in L, bleft in H
getbit:
	dec	h
	jp	p,getbt2	; skip if new byte not needed yet
	push	af
	push	bc
	call	getbyte
	ld	l,a		; new bitbuf
	ld	h,7		; 8 bits left, pre-dec'd
	pop	bc
	pop	af
getbt2:
	rr	l
	rr	c
	rra
	jr	c,bitret
	djnz	getbit
finbit:
	srl	c
	rra
	jp	nc,finbit	; jp likely faster in this case
bitret:
	ld	(bitbuf),hl	; update bitbuf/bleft
	ld	h,c		; return bits in HL and A
	ld	l,a
	ret
;
; rdbybits - faster version of readbits for <=8 bits.
; Due to the implementation this must not ever be called with A>8.
; (No caller seems to require saving BC, so I removed that for both
; this and readbits.)
;
rdbyte:
	ld	a,8

rdbybits:
	ld	(rdbyop+1),a	; modify jr instruction at rdbyop
	ld	b,a
	xor	a		; bits rotate into A (rra faster)
	ld	hl,(bitbuf)	; keep bitbuf in L, bleft in H
rdbylp:	dec	h
	jp	p,rdby1		; skip if new byte not needed yet
	ld	c,a
	push	bc
	call	getbyte
	ld	l,a		; new bitbuf
	ld	h,7		; 8 bits left, pre-dec'd
	pop	bc
	ld	a,c
rdby1:	rr	l
	rra
	djnz	rdbylp
	ld	(bitbuf),hl	; update bitbuf/bleft
	or	a
rdbyop:	jr	rdbyr8
rdbyr8:	rra			; 8x rra, not all are used in practice but
	rra			; this arrangement simplifies code above
	rra
	rra
	rra
	rra
	rra
	rra
	ld	h,b		; B still zero after the final djnz
	ld	l,a		; return bits in HL and A
	ret
;
; rd1bit - faster version which reads a single bit only.
; The jp instruction here is awkward, due to differing
; local-symbol syntax between assemblers.
;
rd1bit	macro
	ld	hl,(bitbuf)	; keep bitbuf in L, bleft in H
	dec	h
	jp	p,$+9		; jump to "xor a", past jp op plus 6 bytes:
	call	getbyte		; (3 bytes)
	ld	l,a		; (1 byte)  new bitbuf
	ld	h,7		; (2 bytes) 8 bits left, pre-dec'd
	xor	a		; jp op above jumps here
	rr	l
	ld	(bitbuf),hl	; update bitbuf/bleft
	ld	h,a		; A still zero
	rla			; return bit in HL and A
	ld	l,a
	endm
;
; scans up to B characters, padding if less, skipping any extras
scanfn:	ld	a,(de)
	cp	'.'
	jr	z,nocopy
	or	a
	jr	z,nocopy
	inc	de
	dec	b
	jp	m,scanfn
	and	7fh		; remove high bit
	cp	'a'		; convert to UPPERCASE
	jr	c,scfu		; use same scheme as CCP
	cp	'z'+1		; only convert letters
	jr	nc,scfu
	sub	20h
scfu:	ld	(hl),a
	inc	hl
	jr	scanfn
;
nocopy:	dec	b
	ret	m
	ld	(hl),' '
	inc	hl
	jr	nocopy
;
ilprt0:	xor	a
	ld	(opts),a	; force [Q]uiet off
ilprt:	pop	hl
	call	pstr
	jp	(hl)
;
pstr:	ld	bc,(opts-1)	; d=options
pstrlp:	ld	a,(hl)
	or	a
	ret	z
	bit	4,b
	jr	nz,pskip	; [Q]uiet
	push	hl
	ld	e,a
	ld	c,conout
	call	bdos		; B preserved
	pop	hl
pskip:	inc	hl
	jr	pstrlp
;
; getstring must return DE pointing just past the last byte read.
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
	ld	a,(counting)
	or	a
	jr	nz,dodci
	ld	a,(zipeof)
	and	1
	ld	a,CtrlZ
	ret	nz
	jr	skpdci
dodci:	ld	hl,(cs)
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
	ld	hl,readpt
	ld	a,(hl)
	add	a,a
	jr	nc,ptok
	ld	(hl),a
	ld	de,80h
	push	hl
	ld	c,setdma
	call	bdos
	call	setin
	ld	de,infcb
	ld	c,fread
	call	bdos
	or	a
	pop	hl
	jr	nz,ateof

ptok:
	ld	a,(hl)
	inc	(hl)
	ld	l,a
	ld	h,0
	ld	de,(inbufp)
	add	hl,de
	ld	a,(hl)
	ret
;
ateof:
	ld	a,CtrlZ
	ret


;;;
outb:	ld	hl,(outpos)
	push	hl
	push	af
	ld	a,(omask)
	and	h
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

	push   bc
	push   de
	push   hl
	ld     e,a
	ld     c,conout
	call   bdos		; B preserved
	pop    hl
	pop    de
	pop    bc
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
cb2:	pop	bc
cb3:	bit	7,d
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
cb5:	dec	bc
	jr	cb3
;
cb4:	ex	de,hl
cb6:	ld	a,b
	or	c
	ret	z
	ld	a,(omask)
	and	d
	ld	d,a
	ld	hl,outbuf
	add	hl,de
	ld	a,(hl)
	call	outbp
	inc	de
	dec	bc
	jr	cb6
;
outbp:	push	hl
	push	de
	push	bc
	call	outb
	pop	bc
	pop	de
	pop	hl
	ret

;
nextsymbol:
	ld	(treep),hl
nsloop:
	push	hl
	rd1bit
	pop	hl
	or	a
	jr	z,nsleft
	inc	hl
	inc	hl
nsleft:
	ld	e,(hl)
	inc	hl
	ld	d,(hl)

	ld	a,d
	cp	10h
	jr	nc,nsleaf
	or	e
	ret	z

	ld	hl,(treep)
	add	hl,de
	add	hl,de
	add	hl,de
	add	hl,de
	jr	nsloop

nsleaf:	and	0fh
	ld	d,a
	ret
;
buildcode:
	ld	(lenp),hl
	ld	(nodes),de
	ld	(nrsym),bc

	ld	hl,blcnt
	ld	de,blcnt + 1
	ld	bc,2 * maxcl + 1
	ld	(hl),b
	ldir

	ld	bc,(nrsym)
	ld	de,(lenp)
bclp1:
	ld	a,(de)
	add	a,a
	jr	z,bcnol
	ld	hl,blcnt
	add	a,l
	ld	l,a
	jr	nc,bcnc1
	inc	h
bcnc1:
	ld	a,(hl)
	inc	a
	ld	(hl),a
	jr	nz,bcnol
	inc	hl
	inc	(hl)
bcnol:
	inc	de
	dec	bc
	ld	a,b
	or	c
	jr	nz,bclp1

	ld	hl,0
	push	hl

	ld	bc,1
bclp2:
	ld	a,c
	sub	maxcl + 1
	jr	nc,bccn2

	ld	hl,blcnt
	add	hl,bc
	add	hl,bc
	dec	hl
	ld	d,(hl)
	dec	hl
	ld	e,(hl)

	pop	hl
	add	hl,de
	add	hl,hl
	push	hl

	ex	de,hl
	ld	hl,ncode
	add	hl,bc
	add	hl,bc
	ld	(hl),e
	inc	hl
	ld	(hl),d

	inc	c
	jr	bclp2
bccn2:
	pop	hl

	ld	hl,(nrsym)
	add	hl,hl
	add	hl,hl
	ld	b,h
	ld	c,l
	ld	hl,(nodes)
	ld	d,h
	ld	e,l
	inc	de
	dec	bc
	ld	(hl),0
	ldir

	ld	hl,1
	ld	(nnode),hl

	ld	bc,0
bclp3:
	ld	hl,(lenp)
	add	hl,bc
	ld	a,(hl)
	or	a
	jr	z,bccn3

	push	bc

	push	af
	ld	hl,ncode
	add	a,a
	add	a,l
	ld	l,a
	jr	nc,bc4
	inc	h
bc4:
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	ld	(bcode),de
	inc	de
	ld	(hl),d
	dec	hl
	ld	(hl),e
	pop	af

	ld	hl,1
bclp4:
	dec	a
	or	a
	jr	z,bccn4
	add	hl,hl
	jr	bclp4
bccn4:
	ld	(bmask),hl

	ld	hl,(nodes)
bclp5:
	ld	de,(bcode)
	ld	bc,(bmask)
	ld	a,d
	and	b
	ld	d,a
	ld	a,e
	and	c
	ld	e,a
	or	d
	jr	z,bcleft
	inc	hl
	inc	hl
bcleft:
	srl	b
	rr	c
	ld	(bmask),bc
	ld	a,b
	or	c
	jr	z,bccn5

	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	ld	a,d
	or	e
	jr	nz,bc6
	ld	de,(nnode)
	ld	(hl),d
	dec	hl
	ld	(hl),e
	inc	de
	ld	(nnode),de
	dec	de

bc6:
	ld	hl,(nodes)
	add	hl,de
	add	hl,de
	add	hl,de
	add	hl,de
	jr	bclp5

bccn5:
	pop	bc

	ld	(hl),c
	inc	hl
	ld	a,b
	or	10h
	ld	(hl),a

bccn3:
	inc	bc
	ld	hl,(nrsym)
	or	a
	sbc	hl,bc
	jr	nz,bclp3
	ret

;
huffman:
	ld	a,5
	call	rdbybits
	inc	a
	ld	l,a
	ld	h,1
	ld	(hlit),hl

	ld	a,5
	call	rdbybits
	inc	a
	ld	(hdist),a

	ld	a,4
	call	rdbybits
	add	a,4
	ld	c,a

	ld	b,0
	ld	de,clord
hmlp1:
	ld	a,b
	cp	c
	jr	nc,hmcn1
	push	bc
	push	de
	ld	a,3
	call	rdbybits
	pop	hl
	ld	c,(hl)
	ld	b,0
	ex	de,hl
	ld	hl,lenld
	add	hl,bc
	ld	(hl),a
	pop	bc
	inc	de
	inc	b
	jr	hmlp1

hmcn1:
	xor	a
	ld	c,a
hmlp2:
	ld	a,b
	cp	nrcl
	jr	nc,hmcn2
	ld	a,(de)
	ld	hl,lenld
	add	a,l
	ld	l,a
	jr	nc,hmnc2
	inc	h
hmnc2:
	ld	(hl),c
	inc	de
	inc	b
	jr	hmlp2

hmcn2:
	ld	hl,lenld
	ld	de,cltr
	ld	bc,nrcl
	call	buildcode

	ld	hl,(hlit)
	ld	a,(hdist)
	add	a,l
	ld	c,a
	ld	a,h
	adc	a,0
	ld	b,a
	ld	hl,lenld
hmlp3:
	push	bc
	push	hl
	ld	hl,cltr
	call	nextsymbol
	ld	a,e

	cp	010h
	jr	nz,hmn16
	ld	a,2
	call	rdbybits
	pop	hl
	pop	bc
	add	a,3
	ld	d,a
	dec	hl
	ld	e,(hl)
	inc	hl
hmlp4:
	ld	(hl),e
	inc	hl
	dec	bc
	dec	d
	jr	nz,hmlp4
	jr	hmcn3

hmn16:
	cp	011h
	jr	nz,hmn17
	ld	a,3
	call	rdbybits
	pop	hl
	pop	bc
	add	a,3
	ld	d,a
	xor	a
hmlp5:
	ld	(hl),a
	inc	hl
	dec	bc
	dec	d
	jr	nz,hmlp5
	jr	hmcn3

hmn17:
	cp	012h
	jr	nz,hmn18
	ld	a,7
	call	rdbybits
	pop	hl
	pop	bc
	add	a,11
	ld	d,a
	xor	a
hmlp6:
	ld	(hl),a
	inc	hl
	dec	bc
	dec	d
	jr	nz,hmlp6
	jr	hmcn3

hmn18:
	pop	hl
	pop	bc
	ld	(hl),a
	inc	hl
	dec	bc

hmcn3:
	ld	a,b
	or	c
	jr	nz,hmlp3

	ld	hl,lenld
	ld	de,littr
	ld	bc,(hlit)
	call	buildcode

	ld	hl,(hlit)
	ld	de,lenld
	add	hl,de
	ld	de,disttr
	ld	a,(hdist)
	ld	c,a
	ld	b,0
	call	buildcode

	ld	hl,(inbps)
	ld	a,l
	or	h
	jr	z,hmnext
	ld	(inbufp),hl
	ld	a,(rdpts)
	ld	(readpt),a

	ld hl,(bitbs)	; H=bleft=blfts, L=bitbuf=bitbs
	ld (bitbuf),hl

	;ld	a,(blfts)
	;ld	(bleft),a
	;ld	a,(bitbs)
	;ld	(bitbuf),a
	ld	hl,counting
	inc	(hl)

hmnext:
	ld	hl,littr
	call	nextsymbol
	ld	a,d
	dec	a
	or	e
	ret	z
	ld	a,(zipeof)
	and	1
	ret	nz

	ld	a,d
	or	a
	jr	nz,hmsym
	ld	a,e
	call	outb
	jr	hmnext

hmsym:
	dec	e
	ld	d,0
	ld	hl,lenex
	add	hl,de
	ld	a,(hl)
	ld	hl,0
	or	a
	jr	z,hmnlen
	push	de
	call	readbits
	pop	de
hmnlen:
	push	hl
	ld	hl,lenbas
	add	hl,de
	add	hl,de
	ld	a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a
	pop	de
	add	hl,de
	push	hl

	ld	hl,disttr
	call	nextsymbol
	ld	hl,dstex
	add	hl,de
	ld	a,(hl)
	ld	hl,0
	or	a
	jr	z,hmndst
	push	de
	call	readbits
	pop	de
hmndst:
	push	hl
	ld	hl,dstbas
	add	hl,de
	add	hl,de
	ld	a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a
	pop	de
	add	hl,de

	pop	bc
	call	callback
	jr	hmnext

;
undeflate:
	ld	a,7fh
	ld	(omask),a

udloop:
	ld	a,(zipeof)
	and	1
	ret	nz

	rd1bit
	push	af

	ld	a,2
	call	rdbybits
	or	a
	jr	nz,udnt0

	xor	a
	ld	(bleft),a
	call	rd16bits
	push	hl
	call	rd16bits
	pop	bc
	scf
	adc	hl,bc
	jr	nz,udblm

udt0lp:
	ld	a,b
	or	c
	jr	z,udnext
	ld	a,(zipeof)
	and	1
	jr	nz,udpret
	push	bc
	call	getbyte
	call	outb
	pop	bc
	dec	bc
	jr	udt0lp

udnt0:
	dec	a
	jr	nz,udnt1
	ld	hl,(inbufp)
	ld	(inbps),hl
	ld	a,(readpt)
	ld	(rdpts),a

	ld hl,(bitbuf)	; H=bleft=blfts, L=bitbuf=bitbs
	ld (bitbs),hl

;	ld	a,(bleft)
;	ld	(blfts),a
;	ld	a,(bitbuf)
;	ld	(bitbs),a
	ld	hl,counting
	dec	(hl)
	ld	hl,static_pre
	ld	(inbufp),hl
	xor	a
	ld	(readpt),a
	ld	(bleft),a
	call	huffman
	jr	udnext

udnt1:
	dec	a
	jr	nz,udubt
	ld	hl,0
	ld	(inbps),hl
	call	huffman

udnext:
	pop	af
	or	a
	jp	z,udloop
	ret

udpret:
	pop	af
	ret
udbskp:
	ld	a,(zipeof)
	and	1
	jr	nz,udpret
	call	getbyte
	jr	udbskp
udblm:
	call	ilprt
	defm	"Block length mismatch"
	defb	CR,LF,0
	jr	udbskp
udubt:
	call	ilprt
	defm	"Unknown block type"
	defb	CR,LF,0
	jr	udbskp
;
; ckcon -- checks console for character; aborts if ^C
; Only really checks every 16 calls, since this is called
; from getbyte for every input byte.
;
ckcon:	ld	a,1		; modified below
	dec	a
	and	15
	ld	(ckcon+1),a	; update LD A instruction above
	ret	nz		; check every 16 calls only
	ld	e,0FFh		; check for character
	ld	c,dircon
	call	bdos
	or	a
	ret	z
	cp	CtrlC		; ^C ?
	ret	nz		; (no, continue)
	ld	a,(curmode)	; are we writing a file?
	or	a
	jr	z,ckcon1	; (no)
	call	setout
ckcon0:	ld	de,opfcb	; ckcon0 jumped to for write error
	ld	c,fclose	; yes, close it
	call	bdos
	ld	de,opfcb
	ld	c,ferase	; and delete it
	call	bdos
	call	ilprt
	defm	"Partial file erased -- "
	defb	0
ckcon1:	call	ilprt0
	defm	"Aborted"
	defb	0
	jp	exit
;
; getusr -- gets and stores source and destination users
;
getusr:	ld	hl,(Z3EAdr)	; ZCPR3?
	ld	a,h
	or	l
	ret	z		; (no, skip this)
	ld	a,(infcb+13)	; get source user
	ld	(inusr),a
	ld	a,(altfcb+13)	; get destination user
	ld	(outusr),a
	ret
;
; setin, setout -- logs to source or destination user
;
setin:	ld	a,(inusr)
	jr	setbth
;
setout:	ld	a,(outusr)
setbth:	ld	e,a
	ld	hl,(Z3EAdr)	; ZCPR3?
	ld	a,h
	or	l
	ret	z		; (no, skip this)
	ld	c,setusr
	jp	bdos		; = call bdos!ret
;
; usage -- show syntax for ZCPR3 ("dir:") or vanilla CP/M ("d:")
;
usage:	call	ilprt0
	DEFM "ZCAT v1.0"
	defb	CR,LF
	defm	"Usage: "
	defm	"ZCAT {d"
	defb	0
	ld	hl,(Z3EAdr)
	ld	a,h
	or	l
	jr	z,usage2
	call	ilprt
	defm	"ir"
	defb	0
usage2:	call	ilprt
	defm	":}<txtfile>[.gz]"
	defb	CR,LF
	defb	CR,LF
	defm	"'type' gzip compressed text files"
	defb	CR,LF,0
	jp	exit
;
; data storage . . .
;	
zipeof:	defb	2
counting:
	defb	0
;init:
;	defb	0	; for bleft
;	defb	0	; for wrtpt
;	defw	0,0	; for outpos
;	;defw	-1,-1	; for crc32
;endinit:
inbufp:	defw	0080h
readpt:	defb	80h
omask:	defb	1fh

IF !NOREDUCE
_L_table:
	defb	7fh, 3fh, 1fh, 0fh
_D_shift:
	defb	07h, 06h, 05h, 04h
ENDIF

clord:
	defb	16, 17, 18
	defb	0, 8
	defb	7, 9
	defb	6, 10
	defb	5, 11
	defb	4, 12
	defb	3, 13
	defb	2, 14
	defb	1, 15
lenbas:
	defw	3, 4
	defw	5, 6
	defw	7, 8
	defw	9, 10
	defw	11, 13
	defw	15, 17
	defw	19, 23
	defw	27, 31
	defw	35, 43
	defw	51, 59
	defw	67, 83
	defw	99, 115
	defw	131, 163
	defw	195, 227
	defw	258
lenex:
	defb	0, 0
	defb	0, 0
	defb	0, 0
	defb	0, 0
	defb	1, 1
	defb	1, 1
	defb	2, 2
	defb	2, 2
	defb	3, 3
	defb	3, 3
	defb	4, 4
	defb	4, 4
	defb	5, 5
	defb	5, 5
	defb	0
dstbas:
	defw	1, 2
	defw	3, 4
	defw	5, 7
	defw	9, 13
	defw	17, 25
	defw	33, 49
	defw	65, 97
	defw	129, 193
	defw	257, 385
	defw	513, 769
	defw	1025, 1537
	defw	2049, 3073
	defw	4097, 6145
	defw	8193, 12289
	defw	16385, 24577
dstex:
	defb	0, 0
	defb	0, 0
	defb	1, 1
	defb	2, 2
	defb	3, 3
	defb	4, 4
	defb	5, 5
	defb	6, 6
	defb	7, 7
	defb	8, 8
	defb	9, 9
	defb	10, 10
	defb	11, 11
	defb	12, 12
	defb	13, 13
static_pre:
	defb	0ffh, 05bh, 000h, 06ch, 003h, 036h, 0dbh, 0b6h
	defb	06dh, 0dbh, 0b6h, 06dh, 0dbh, 0b6h, 0cdh, 0dbh
	defb	0b6h, 06dh, 0dbh, 0b6h, 06dh, 0dbh, 0a8h, 06dh
	defb	0ceh, 08bh, 06dh, 03bh



;
; uninitialized storage
;
DEFVARS ASMPC+100h   ; Dynamic data structure
{
oldstk	ds.w	1		; also end of initialised data

inusr	ds.b	1
outusr	ds.b	1
mode	ds.b	1
opts	ds.b	1
junk	ds.b	STRSIZ
lfh		ds.b	0		; data read from local file header

vnte	ds.w	1		; version
gpbf	ds.w	1		; general purpose bit flag
cm		ds.w	1		; compression method
lmft	ds.w	1		; file last modification time
lmfd	ds.w	1		; file last modification date

crc		ds.b	4	; CRC-32 of uncompressed data
cs		ds.b	4	; compressed size
ucs		ds.b	4	; uncompressed size
fnl		ds.w	1	; file name length
efl		ds.w	1	; extra field length
endlfh	ds.b	1	; marker for end of lfh data; also,
					; zero byte is written here by getstring

opfcb	ds.b	1	; output file control block
opfn	ds.b	8
opext	ds.b	3 + 24

mtchfcb	ds.b	11
; note that as indicated above, bitbuf must be the byte before bleft
bitbuf	ds.b	1

;vars	ds.b	0
bleft	ds.b	1

wrtpt	ds.b	1
outpos	ds.b	4
;crc32	ds.b	4

curmode	ds.b	1
opbuf	ds.b	128

L_table	ds.b	1
D_shift	ds.b	1
urV		ds.b	1
nchar	ds.b	1
lchar	ds.b	1
ExState	ds.b	1

Len		ds.b	2
ltp		ds.b	1
mml		ds.b	1
dictb	ds.b	1
noswps	ds.b	1
entrs	ds.b	2
lbl		ds.b	1

treep	ds.w	1
lenp	ds.w	1
nodes	ds.w	1
nrsym	ds.w	1
nnode	ds.w	1
bcode	ds.w	1
bmask	ds.w	1
blcnt	ds.w	maxcl + 1
ncode	ds.w	maxcl + 1
hlit	ds.w	1

hdist		ds.b	1
inbps		ds.w	1

rdpts		ds.b	1

; Keep bitbs and blfts wrapped together
; so we can pick them up with a single 16bit register
bitbs		ds.b	1
blfts		ds.b	1

codesize	ds.b	1

IF !NOSHRINK
oldcode		ds.w	1
;offset		ds.w	1
maxcode		ds.w	1
free_ent	ds.w	1
finchar		ds.b	1
ENDIF

; Various
stackp		ds.w	1
incode		ds.w	1
code		ds.w	1


outbuf		ds.b	0 
suffix_of	ds.b	8192
prefix_of	ds.b	0 
Slen		ds.b	0 

lit_tree	ds.b	_sf_tree_
len_tree	ds.b	_sf_tree_
dist_tre	ds.b	_sf_tree_	+	16384 + 2 - (3 * _sf_tree_)

followers	ds.b	0 
stack		ds.b	8192 - 2

lenld	ds.b	nrlit + nrdist
cltr	ds.b	4 * nrcl
littr	ds.b	4 * nrlit
disttr	ds.b	4 * nrdist
;endtr	ds.b	8192 + 2 - (endtr - lenld)
endtr	ds.b	8192 + 2 - (nrlit + nrdist + 4 * (nrcl + nrlit + nrdist))

endaddr		ds.b	0 ; must be no vars/data beyond this point

}

