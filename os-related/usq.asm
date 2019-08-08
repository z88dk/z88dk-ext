
; USQ119.ASM
; converted to Z80 mnemonics with {z88dk}/support/8080/toZ80.awk

; To build:
;              z80asm -b -oUSQ.COM -DZ80 usq.asm
;              (z80asm -b -oUSQ.COM usq.asm for 8080 CPU)
;

; Original program size (for 8080 CPU):  1920 bytes.
; 


		org	100h
		

DEFC true  =  -1 
DEFC false  =  0


DEFC eof     =  1ah 
DEFC dle     =  90h 
DEFC bdos    =  5 
DEFC buffer  =  80h 
DEFC fcb     =  50h 

begin:
hell:
		LD	HL,0
		ADD	HL,sp
		LD	(ccpstack),HL
		LD	sp,stack

		LD	HL,(bdos+1)
		LD 	l,0
		LD	DE,-1700h
		ADD	HL,DE
		LD	(topmem),HL
		call	ilprt
		DEFM	13,10,"USQ  Version 1.19     Dave Rand     07/28/1983",0
		LD 	c,25
		call	bdos
		LD	(current),A
		XOR	a		;default to no prompt
		LD	(pract),A
		LD	A,(buffer)
		OR	a
		jr	NZ,ok
					;if no filespec, print instructions
	
inst:
		call	ilprt
		DEFM	13,10,"Use: USQ afn [afn afn ...] [destination drive:]",0


		LD 	a,255		;show prompt mode active
		LD	(pract),A

in1:
		call	ilprt
		DEFM	13,10,"*",0
		LD	HL,buffer
		LD 	(HL),120
		EX	DE,HL
		LD 	c,10
		call	bdos
		LD	A,(buffer+1)
		OR	a
		jr	Z,in1
		LD	(buffer),A
		LD 	e,a
		LD 	d,0
		LD	HL,buffer+2
		PUSH	HL
		ADD	HL,DE
		LD 	(HL),0
		POP	HL
		LD	DE,buffer+1
in2:
		LD 	a,(HL)
		LD	(DE),A
		OR	a
		jr	Z,ok
		call	convuc
		LD	(DE),A
		INC	HL
		INC	DE
		jr	in2

convuc:
		CP	'a'
		RET	C
		CP	'z'+1
		RET	NC
		AND	5fh
		ret


		
	


ok:
		LD	A,(pract)
		OR	a
		jr	Z,nosel
		LD 	a,13
		call	bdos
		LD	A,(current)
		LD 	e,a
		LD 	c,14
		call	bdos


nosel:
		LD	HL,80h
		LD	DE,locl
		LD	BC,80h
IF Z80
		ldir
ELSE
		call	ldir
ENDIF
		LD 	c,25
		call	bdos
		LD	(current),A
		INC	a
		LD	(destd),A

		LD	DE,locl+1
ex1:
		call	non_blnk	;point to first valid char
		JP	Z,inst		;wups... no char to be had!
mul1:
		EX	DE,HL
		LD	(nxtchr),HL
		LD	HL,0
		LD	(max1),HL

		LD	HL,filespecs	;point to begin of wildcard table

parse:
		PUSH	HL
		call	make_fcb	;make FCB please!
		LD	A,(fcb+1)
		CP	' '
		jr	NZ,par2
		call	ilprt
		DEFM	13,10,"Output drive = ",0
		LD	A,(fcb)
		LD	(destd),A
		ADD	A,'0'
		call	conout
		call	ilprt
		DEFM	":",0
		jr	par1
par2:
		POP	HL
		call	buildam		;build amb file table
		LD	(lastmem),HL
		PUSH	HL
		LD	HL,(max1)
		ADD	HL,DE
		LD	(max1),HL
par1:
		LD	HL,(nxtchr)
pl1:
		LD 	a,(HL)
		CP	' '
		INC	HL
		jr	Z,pl2
		OR	a
		jr	Z,pl3
		jr	pl1
pl2:
		LD	(nxtchr),HL
		EX	DE,HL
		call	non_blnk
pl3:
		POP	HL
		jr	NZ,parse		;all done?

gt1:
					;Name ok, any wildcards match?
		LD	HL,(max1)
		LD 	a,l
		OR	h
		jr	NZ,cont		;yep, can continue
		call	errext
		DEFM	13,10,"No file(s) found.",0




cont:
		LD	HL,(lastmem)
		LD	(sob),HL
		LD	(eob),HL
		EX	DE,HL
		LD	HL,(topmem)
		LD 	a,h
		sub	d
		LD 	h,a
		LD 	a,l
		SBC	e
		LD 	l,a
;hl now has total memory free. Divide in half.
		XOR	a
		LD 	a,h
		RRA
		LD 	h,a
		LD 	a,l
		RRA
		LD 	l,a
;see if enuf memory.
		DEC	h
		LD 	a,h
		OR	a
		jr	NZ,memok
		call	errext
		DEFM	13,10,13,10,"Out of memory. Use more specific filenames.",0

memok:
		EX	DE,HL
		LD	HL,(lastmem)
		ADD	HL,DE
		LD	(endmem),HL
		INC	h
		INC	h
		LD 	l,0
		LD	(sob1),HL
		LD	(sob1a),HL	
		LD	HL,(topmem)
		LD	(eob1),HL

main:
		LD	HL,filespecs

main1:
		LD	DE,ifcb
		LD	BC,12
IF Z80
		ldir
ELSE
		call	ldir
ENDIF
		PUSH	HL
		PUSH	DE
		POP	HL
		INC	DE
		LD 	(HL),0
		LD	BC,38-13
IF Z80
		ldir
ELSE
		call	ldir
ENDIF
		LD	DE,ifcb
		LD 	c,15
		call	bdos
		INC	a
		jr	Z,mainr
sysok:
		call	ilprt
		DEFM	13,10,0
		call	pfcb
		LD	HL,(lastmem)
		LD	(sob),HL
		LD	(eob),HL
		PUSH	HL
		call	getw
		LD	DE,0ff76h
		call	cmpdehl
		POP	HL
		jr	Z,usq
		call	ilprt
		DEFM	" is not a squeezed file.",13,10,0
mainr:
		LD	sp,stack-2
		LD	HL,(max1)
		DEC	HL
		LD	(max1),HL
		LD 	a,h
		OR	l
		POP	HL
		jr	NZ,main1
		JP	usq7


;this is start of baseline USQ code

usq:
		XOR	a		;force init char read
		LD	(numlft),A
		LD	(rcnt),A		;and zero repeats
usq1:
		call	getw		;get cksum, and store
		LD	(filecrc),HL
		call	ilprt
		DEFM	" -> ",0
		LD	HL,buffer	;get name of orig. file,
usq2:
		PUSH	HL
		call	get1		;display, and store it
		POP	HL		;for filename parse
		PUSH	AF
		call	convuc
		LD 	b,a
		POP	AF
		LD 	a,b
		LD 	(HL),a
		jr	NZ,mainr
		OR	a
		jr	Z,usq3
		PUSH	HL
		call	conout
		POP	HL
		INC	HL
		jr	usq2

usq3:
		LD	HL,buffer		;parse orig. name from
		LD	(nxtchr),HL			;buffer. Create FCB
		call	make_fcb
		LD	HL,fcb
		LD	DE,dfcb
		LD	BC,1+8+3
IF Z80
		ldir
ELSE
		call	ldir
ENDIF
		LD	A,(destd)
		LD	(dfcb),A
		LD	HL,dfcb+1+8+3
		LD	DE,dfcb+1+8+3+1
		LD	BC,38-13
		LD 	(HL),0
IF Z80
		ldir
ELSE
		call	ldir
ENDIF
		LD	DE,dfcb
		PUSH	DE
		LD 	c,19
		call	bdos
		POP	DE
		LD 	c,22
		call	bdos
		INC	a
		jr	NZ,usq3a
		call	errext
		DEFM	13,10,"No directory space. Aborting.",0
usq3a:
		call	getw
		LD	(numvals),HL
		LD	DE,258
		call	cmpdehl
		jr	C,usq3b
		call	errext
		DEFM	13,10,"Files has illegal decode size. Aborting.",0
usq3b:
		LD	DE,table
usq4:
		LD	(max),HL
		LD 	a,h
		OR	l
		jr	Z,usq5
		PUSH	DE
		call	getw
		POP	DE
		EX	DE,HL
		LD 	(HL),e
		INC	HL
		LD 	(HL),d
		INC	HL
		PUSH	HL
		call	getw
		EX	DE,HL
		POP	HL
		LD 	(HL),e
		INC	HL
		LD 	(HL),d
		INC	HL
		EX	DE,HL
		LD	HL,(max)
		DEC	HL
		jr	usq4

usq5:
		LD	HL,0
usq6:
		PUSH	HL
		call	getnxt
		POP	HL
		jr	NZ,usq8
		LD 	e,a
		LD 	d,0
		ADD	HL,DE
		PUSH	HL
		call	put1
		POP	HL
		jr	usq6

usq8:
		EX	DE,HL
		LD	HL,(filecrc)
		call	cmpdehl
		PUSH	AF
		call	flush
		LD	DE,dfcb
		LD 	c,16
		call	bdos
		INC	a
		jr	NZ,usq9
		call	errext
		DEFM	13,10,"Close failed...",0

usq9:
		POP	AF
		JP	Z,mainr
		call	ilprt
		DEFM	13,10,"ERROR - Checksum error in file ",0
		call	pfcb
		
usq7:
		LD	sp,stack
		LD	A,(pract)
		OR	a
		JP	NZ,in1

		LD	sp,0
DEFC ccpstack  =  $-2 
		ret

errext:
		POP	HL
		LD 	a,(HL)
		OR	a
		jr	Z,usq7
		INC	HL
		PUSH	HL
		call	conout
		jr	errext

conout:
		AND	127
		LD 	e,a
		LD 	c,2
		call	bdos
		ret


cmpdehl:
	LD 	a,h
		CP	d
		RET	NZ
		LD 	a,l
		CP	e
		ret

ilprt:
		POP	HL
		LD 	a,(HL)
		OR	a
		INC	HL
		PUSH	HL
		RET	Z
		call	conout
		jr	ilprt

get1:
		LD	HL,(eob)
		EX	DE,HL
		LD	HL,(sob)
		call	cmpdehl
		jr	Z,get1r
		LD 	a,(HL)
		INC	HL
		LD	(sob),HL
		CP	a
		ret

get1r:
		LD	HL,(lastmem)
		LD	(sob),HL
		LD	(eob),HL
get1r1:
		PUSH	HL
		EX	DE,HL
		LD 	c,26
		call	bdos
		LD	DE,ifcb
		LD 	c,20
		call	bdos
		POP	HL
		OR	a
		jr	NZ,get1r2
		LD	DE,128
		ADD	HL,DE
		EX	DE,HL
		LD	HL,(endmem)
		call	cmpdehl
		EX	DE,HL
		jr	NC,get1r1
get1r2:
		LD	(eob),HL
		EX	DE,HL
		LD	HL,(sob)
		call	cmpdehl
		jr	NZ,get1
		LD 	a,255
		OR	a
		ret



put1:
		LD 	c,a
		LD	HL,(eob1)
		EX	DE,HL
		LD	HL,(sob1)
		call	cmpdehl
		jr	Z,put1s
		LD 	(HL),c
		INC	HL
		LD	(sob1),HL
		ret

put1s:
		PUSH	BC
		call	flush
		POP	BC
		LD 	a,c
		jr	put1

flush:
		LD	HL,(sob1a)
		EX	DE,HL
		LD	HL,(sob1)
		call	cmpdehl
		RET	Z
		EX	DE,HL
put1sa:
		PUSH	HL
		EX	DE,HL
		LD 	c,26
		call	bdos
		LD 	c,21
		LD	DE,dfcb
		call	bdos
		OR	a
		jr	NZ,put1sc
		POP	HL
		LD	DE,128
		ADD	HL,DE
		EX	DE,HL
		LD	HL,(sob1)
		EX	DE,HL
		call	cmpdehl
		jr	C,put1sa
		LD	HL,(sob1a)
		LD	(sob1),HL
		ret


put1sc:
		call	errext
		DEFM	13,10,"Disk full. Aborting.",0


getw:
		call	get1
		jr	NZ,badr
		PUSH	AF
		call	get1
		jr	NZ,badr
		LD 	h,a
		POP	AF
		LD 	l,a
		ret

badr:
		call	ilprt
		DEFM	13,10,"Premature EOF on file... aborted.",0
		JP	mainr

getnxt:
		LD	A,(rcnt)		;see if in the middle of
		OR	a		;repeat sequence...
		jr	Z,getn7
		DEC	a
		LD	(rcnt),A
		LD	A,(last)
		CP	a
		ret
getn7:
		call	getn4
		CP	dle
		jr	NZ,getn5
		call	getn4
		OR	a
		jr	NZ,getn6
		LD 	a,dle		;dle is encoded as dle,0
		CP	a
		ret
getn6:
		DEC	a
		DEC	a
		LD	(rcnt),A
		LD	A,(last)
		CP	a
		ret
getn5:
		LD	(last),A
		CP	a
		ret


getn4:
		LD	DE,0		;pointer 0 sot
		LD	A,(char)
		LD 	c,a
getn1:
		LD	A,(numlft)
		OR	a
		jr	NZ,getn2
		PUSH	DE
		call	get1
		jr	NZ,badr
		POP	DE
		LD 	c,a
		LD 	a,8
getn2:
		DEC	a
		LD	(numlft),A
		LD 	a,c
		RRCA
		LD 	c,a
		LD	HL,table
		jr	NC,getn3
		INC	HL
		INC	HL		;add 2 to point to right node
getn3:
		ADD	HL,DE
		ADD	HL,DE
		ADD	HL,DE
		ADD	HL,DE		;ok.. pointing close to right plc..
		LD 	e,(HL)
		INC	HL
		LD 	d,(HL)
		LD 	a,d
		AND	128
		jr	Z,getn1
		LD 	a,c
		LD	(char),A
		LD 	a,d
		CP	254		;is special eof?
		LD 	a,eof
		jr	Z,geteof		;yup
		LD 	a,e
		CPL
		CP	a
		ret

geteof:
		POP	HL
		OR	a
		ret


;end of baseline USQ code

DEFC buildam  =  $ 
		LD	DE,0		;none found yet
		PUSH	DE
		PUSH	HL
		LD	A,(fcb)
		OR	a
		jr	Z,build1
		LD 	e,a
		DEC	e
		LD 	c,14
		call	bdos
build1:
		LD 	c,17
		LD	DE,fcb
		call	bdos
		POP	HL
		POP	DE
		INC	a		;any found?
		jr	NZ,loop
buildr:
		PUSH	HL
		PUSH	DE
		LD	A,(current)
		LD 	e,a
		LD 	c,14
		call	bdos
		POP	DE
		POP	HL
		ret

loop:
		INC	DE
		PUSH	DE
		PUSH	HL
		DEC	a
		ADD	A,a
		ADD	A,a
		ADD	A,a
		ADD	A,a
		ADD	A,a
		LD	HL,buffer
		LD 	e,a
		LD 	d,0
		ADD	HL,DE
		POP	DE
		INC	HL
		LD	A,(fcb)
		LD	(DE),A
		INC	DE
IF Z80
		LD	BC,11
		ldir
ELSE
		LD 	b,11
ldir2:
		LD 	a,(HL)
		LD	(DE),A
		INC	HL
		INC	DE
		DEC	b
		jr	NZ,ldir2
ENDIF
		EX	DE,HL
		PUSH	HL
		LD 	c,18
		LD	DE,fcb
		call	bdos
		POP	HL
		POP	DE
		INC	a
		jr	NZ,loop
		jr	buildr


pfcb:
		LD	A,(ifcb)
		OR	a
		jr	Z,print1
		LD 	b,a		;New!
		LD	A,(current)
		INC	a
		CP	b
		jr	Z,print1
		LD 	a,b		;New...
		ADD	A,'A'-1
		call	conout
		LD 	a,':'
		call	conout
print1:
		LD	HL,ifcb+1
		LD 	c,8
print1a:
	PUSH	HL
		PUSH	BC
		LD 	a,(HL)
		CP	' '
		jr	Z,print1b
		call	conout
print1b:
	POP	BC
		POP	HL
		INC	HL
		DEC	c
		jr	NZ,print1a
		LD 	a,'.'
		call	conout
		LD	HL,ifcb+1+8
		LD 	c,3
print2a:
	PUSH	HL
		PUSH	BC
		LD 	a,(HL)
		CP	' '
		jr	Z,print2b
		call	conout
print2b:
	POP	BC
		POP	HL
		INC	HL
		DEC	c
		jr	NZ,print2a
		ret

;
;Create a FCB in FCB
;'NEXT$CHAR' is saved pointing to the next character
;following the string set up as a file NAME.TYPE.
;
;For example, the SAVE command finds the ascii string
;corresponding to the ntmber of decimal records to write
;as a file name in the first 16 bytes of the fcb, and
;the name of the file to created in the second 16 bytes
;of the fcb.
;
make_fcb:
	LD	HL,fcb		;point to ccp's fcb
	PUSH	HL		;save char pointer once
	LD	HL,(nxtchr)		;get pointer to next char in buffer
	EX	DE,HL			;put buffer pointer in <DE>
	CALL	non_blnk	;get next non-blank char in acc
	POP	HL
	LD	A,(DE)
	OR	A
	jr	Z,no_drv
	SBC	'0'
	LD 	B,A
	INC	DE
	LD	A,(DE)
	CP	':'
	jr	Z,yes_drv
	DEC	DE
no_drv: LD	A,(current)
	INC	a		;01.02
	LD 	(HL),A
	jr	get_name
;
yes_drv:
	LD 	(HL),b
	INC	DE
;
;The next 8 characters in the CCP$FCB are to be a file
;name.	Transfer the contents of the CON$BUF, checking
;for reserved characters and ambigious name char ('*' or '?')
;filling with blanks or '?' as required.
;
get_name:
	LD 	B,8
get1_name:
	CALL	test_4_res
	jr	Z,fill_spc
	INC	HL
	CP	'*'
	jr	NZ,not_amb
	LD 	(HL),3FH
	jr	keep_cnt
;
not_amb:
	LD 	(HL),A
	INC	DE
keep_cnt:
	DEC	B
	jr	NZ,get1_name
find_res:
	CALL	test_4_res
	jr	Z,put_byte
	INC	DE
	jr	find_res
;
fill_spc:
	INC	HL
	LD 	(HL),' '
	DEC	B
	jr	NZ,fill_spc
;
;The next three characters in the CCP$FCB are to be the
;file type.  Transfer the contents of CON$BUF checking
;for reserved characters and ambigious characters ('*' or '?')
;Fill with '?'s as required.
;
put_byte:
	LD 	B,3
	CP	'.'

	jr	NZ,fil_2_spc
	INC	DE
put2_type:
	CALL	test_4_res
	jr	Z,fil_2_spc
	INC	HL
	CP	'*'
	jr	NZ,xfer_type
	LD 	(HL),'?'
	jr	keep2_cnt
;
xfer_type:
	LD 	(HL),A
	INC	DE
keep2_cnt:
	DEC	B
	jr	NZ,put2_type
;
;We have a FILENAME.TYPE, so now find the next reserved
;character in the command string so we can save NEXT$CHAR
;below
;
find1_res:
	CALL	test_4_res
	jr	Z,fill_null
	INC	DE
	jr	find1_res
;
fil_2_spc:
	INC	HL
	LD 	(HL),' '
	DEC	B
	jr	NZ,fil_2_spc
;
;Set the file extent (byte 12 of fcb) and the
;unused bytes (13 and 14) of the fcb to zero
;
fill_null:
	LD 	B,3
fill1_null:
	INC	HL
	LD 	(HL),0
	DEC	B
	jr	NZ,fill1_null
;
;We are almost finished.  Save pointer of the next character
;in the console buffer, count the number of ambigious char's
;in the filename.type, and return with the count in acc and
;the flags set
;
	EX	DE,HL
	LD	(nxtchr),HL
	RET

;
;Test char at <DE> for reserved characters 'SPACE',
;'EQUALS', 'UNDERLINE', 'PERIOD', 'COLON', 'SEMI-COLON',
;'LEFT-ARROW', 'RIGHT-ARROW', and return with zero set,
;if found.  If the character is less than an ascii SPACE,
;and exit is made to the ECHO$BUF routine which will
;print the error prompt and echo the buffer
;
test_4_res:
	LD	A,(DE)		;get (DE) in acc
	OR	A		;set the flags
	RET	Z			;get back if null
	CP	' '		;is it less than a SPACE?
	RET	Z			;if ' ', then get back
	CP	'='
	RET	Z			;if '=', get back
	CP	'_'
	RET	Z			;if '_', get back
	CP	'.'
	RET	Z			;if '.', get back
	CP	':'
	RET	Z			;if ':', get back
	CP	';'
	RET	Z			;if ';', get back
	CP	'<'
	RET	Z			;if '<', get back
	CP	'>'
	Ret			;if '>', get back
;
;Search the character string pointed by <DE> until
;a non-blank char or null is found.  If a null is
;found, return with ZERO flag set.  Otherwise return
;with the char in the acc and <DE> pointing to it.
;(null is placed at end of command string by convert
;routine)
;
non_blnk:
	LD	A,(DE)		;get next char
	OR	A		;set flags
	RET	Z			;get back if null
	CP	' '		;is it a space?
	RET	NZ			;no, then get back
	INC	DE		;bump the pointer
	jr	non_blnk	;loop

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



DEFVARS ASMPC+100h   ; Dynamic data structure
{
numvals		ds.w 1
max			ds.w 1
numlft		ds.b 1
char		ds.b 1
last		ds.b 1
rcnt		ds.b 1
lastmem		ds.w 1
max1		ds.w 1
nxtchr		ds.w 1
current		ds.b 1
endmem		ds.w 1
topmem		ds.w 1
sob			ds.w 1
eob			ds.w 1
sob1		ds.w 1
sob1a		ds.w 1
eob1		ds.w 1
destd		ds.b 1
pract		ds.b 1
filecrc		ds.w 1

ifcb		ds.b 40
dfcb		ds.b 40
locl		ds.b 80h

extra_foo	ds.b 100
stack		ds.b 1

table		ds.b 258*4

filespecs	ds.b 1
}


