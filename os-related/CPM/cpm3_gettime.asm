;	title	Tell The Time in English (or whatever)
;
;	name	('TIME')
;
;	vers	Equ	10	;current version
;----------------------------------------------------------------------
; EPSON QX-10 Utility to tell the time in a way that one can understand
;
; Author: A. Clarke, 11 Sun Street, London E.C.2.
; Date: 6th May 1984
;----------------------------------------------------------------------
;copyright A.R.M.C. 1984
;	cseg
;

;	** CP/M 3 ONLY **
;	potential bugfix for Y2K:  add -DY2K and test on your CP/M Plus system

;    z88dk-z80asm -b -ogettime.com -DY2K cpm3_gettime.asm


;
;	DATES - RETURN ASCII DATE, TIME TO AREA ADDRESSED BY HL ON ENTRY
;

ORG 100h

DATES:	;Start here, folks

	LD		c,12		; BDOS return version number
	CALL	bdos
	CP		024h		; Check that the version of CP/M is reasonably hight
	JP		C,notcpm3
	LD		A,H
	OR		a

	JR		onwards_confidently

;; 	JP		NZ,notcpm3
;; ;check for QX+
;; 	LD		C,50		; BDOS Direct BIOS calls
;; 	LD		DE,BIOSPB
;; 	LD		A,30		; Customised BIOS entry point
;; 	LD		(DE),A		; requires no other registers
;; 	CALL	bdos		; Check that the right machine 
;; 	CP		'A'		; Epson BIOS returned version
;; 	JP		Z,onwards_confidently
;-----------------------------------------;
; Program not loaded under CP/M3 on Epson ;
;-----------------------------------------;
notcpm3:
	LD		HL,cpm3msg
	CALL	strout

	LD		C,0
	JP		bdos		; and terminate program

cpm3msg:
;;	defm	"Epson Time and Utility -"
;;	defm	" Requires CP/M + on Epson QX-10",0dH,0aH,0
	defm	" Requires CP/M + or MP/M II",0dH,0aH,0
;silly wally

onwards_confidently:
	LD		DE,todblk
	LD		C,105	; BDOS Get Date and Time
	CALL	bdos
	LD		(todsec),A	; and seconds

;--------------------------------------------------------------;
; convert date with day 1 = jan 1st 1978 into day month & year ;
;--------------------------------------------------------------;

	LD		HL,(todday)		; BDOS days
; now subtract 365 or 366 for each year after 1978
	LD		BC,1978
dmy1:
	LD		DE,-365
	LD 		A,C
	AND		011b		; check if divisible by 4
	JP		NZ,dmy2
	DEC		DE		; yes so year has 365 days
dmy2:
	ADD		HL,DE
	JP		NC,dmy3		; result less than 0
	LD		A,L	;
	OR		H	;
	JP	Z,dmy3	;A.R.M.Clarke code
	INC	BC
	JP	dmy1
dmy3:
; DE = HL-DE
	LD 	 A,L
	SUB  E
	LD 	 E,A
	LD 	 A,H
	SBC	 D
	LD 	 D,A
	
;----	ora	a
;----	sbc	hl,de		; subtracty 1 year so <HL> > 0
;----	exch		; <DE> -> days remaining in year

	PUSH	DE
	LD		HL,-1900
	ADD		HL,BC		; <HL> = years > 1900
	LD		DE,-100		; but if <HL> > 99, subtract 100
dmy4:
	LD		A,H
	OR		A
	JP		NZ,dmy5
	LD		A,L
	CP		100		; check range
	JP		C,dmy6
dmy5:
	ADD		HL,DE		; adjust by 100
	JP		dmy4
dmy6:
	LD		(todyy),A		; save year
	POP		DE

; on entry <BC> = absolute year, <DE> days remaining in year

	LD		HL,month+12*2-1
	LD		B,12
	LD		A,C
	AND		11b		; test  if current year is a leap year
	JP		NZ,dmy7
	DEC	DE		; so decrement days to allow for Feb 29th
dmy7:
	LD 		A,(HL)		; get days remaining
	DEC		HL
	CP		D		; test (days to months) with <DE>
	JP		C,dmy9		; have month
	JP		NZ,dmy8
	LD 		A,(HL)		;get low byte
	CP		E
	JP		C,dmy9
;	jz		dmy9		; it goes better with this out
dmy8:
	DEC		B		; decrement month
	DEC		HL		; decrement days in month pointer
	LD 		A,B
	CP		2		; are we into FEB
	JP		NZ,dmy7
	LD 		A,C
	AND		11b		; are we in leap year
	JP		NZ,dmy7
	INC		DE		; YES so correct days adjustment
	JP		dmy7
dmy9:
	LD 		A,B
	LD		(todmm),A		; save month
	LD 		A,E
	SUB		(HL)		; p.s. who cares about <D> - (hl+1)
	LD		(toddd),A		; save day

;-----------------------------------------------------------

	CALL	print_time	
	CALL	print_day
	CALL	print_date
	CALL	print_month
	CALL	print_year
	LD 		A,' '
	CALL	tyo
	
	LD 		A,'('
	CALL	tyo
	LD		A,(toddd)	;GET Day
	CALL	numtocon
	LD	 	A,'/'
	CALL	tyo

	LD		A,(todmm)	;GET Month
	CALL	numtocon
	LD	 	A,'/'
	CALL	tyo

	LD		A,(todyy)	;GET Year
	CALL	numtocon
	LD 		A,' '
	CALL	tyo

	LD		A,(todhr)	; Get hours
	CALL	bcdasc
	LD 		A,':'
	CALL	tyo

	LD		A,(todmin)	;GET MINUTES
	CALL	bcdasc
	LD 		A,':'
	CALL	tyo

	LD		A,(todsec)	; SECONDS
	CALL	bcdasc
	LD 		A,')'
	CALL	tyo
	JP		0000H	;reboot
;	ret
;
;
numtocon:		; Convert binary to BCD
	CP		10
	JP		C,bcdasc	; Binary same as BCD
	LD 		B,A
	XOR		A
NUM0:
	INC		A
	DAA
	DEC		B
	JP		NZ,NUM0
bcdasc:			; Convert packed BCD number into ascii
	PUSH	AF
	AND	0F0H	;CONVERT PACKED DECIMAL NUMBER AND Print it
	RRCA
	RRCA
	RRCA
	RRCA
	AND	A
	CALL	NZ,NUM2
	POP		AF
	AND		0FH
NUM2:
	ADD		A,'0'
	CP		'9'+1
	JP		C,NUM3
	ADD		'A'-('9'+1)
NUM3:
	CALL	tyo
	RET
;
print_day:	;of the week on the console
	LD		HL,(todday)
	LD 		C,7	;days in the week
	CALL	divide
	ADD		A	;as they are string pointer tables
	LD		HL,day_name_table
	CALL	addhl
;HL points to pointer to string
	LD		E,(HL)
	INC		HL
	LD		D,(HL)
	EX		DE,HL
	CALL	strout	;print out the day of the week
	RET

print_date:
	LD	A,(toddd)	;GET Day
	PUSH	AF
	CALL	numtocon
	POP		AF
	LD 		B,0
__div10:
	INC		B
	SUB		10
	JP		NC,__div10
	DEC		B
	ADD		A,10
;B=quotient, A=remainder
	LD		HL,th_string
	CP		4
	JP		NC,__its_th
	AND		A
	JP		Z,__its_th
	DEC		B
	JP		Z,__its_th
	LD		HL,th_array
	DEC		A
	ADD		A
	CALL	addhl
	LD 		E,(HL)
	INC		HL
	LD 		D,(HL)
	EX		DE,HL
__its_th:
	CALL	strout
	RET

		
print_month:

	LD		A,(todmm)
	DEC		A
	ADD		A
	LD		HL,month_name_table
	CALL	addhl
	LD 		E,(HL)
	INC		HL
	LD 		D,(HL)
	EX		DE,HL
	CALL	strout
	RET

print_year:

IF Y2K
	LD 		A,20
ELSE
	LD 		A,19
ENDIF
	CALL	numtocon
	LD		A,(todyy)	;GET Year
	CALL	numtocon
	LD 		A,' '
	CALL	tyo
	RET
	
;----------------------------------------------------------------------------
print_time:
	LD		A,(todhr)
	LD		HL,AM_string
	LD 		B,A
	AND		0f0H
	JP		Z,__its_am
	LD 		A,B
	CP		13h
	JP		C,__its_am
	SUB		12h

	daa			; | <-- fix ?
	nop			; |

	LD		HL,PM_string
	LD		B,A		;replace amended bcd
__its_am:
	LD 		A,B
	PUSH	HL
	CALL	bcdasc
	LD 		A,':'
	CALL	tyo
	LD		A,(todmin)	;GET MINUTES
	CALL	bcdasc
	LD 		A,':'
	CALL	tyo
	LD		A,(todsec)	; SECONDS
	CALL	bcdasc
	POP		HL
	CALL	strout
	RET

;----------------------------------------------------------------------------
divide:	;HL by C, modulus in A
	LD 		A,0
	LD 		B,16
div_1:
	ADD		A
	ADD		HL,HL
	ADC		A,0
	CP		C
	JP		C,div_3
	INC		HL
	SUB		C
div_3:
	DEC		B
	JP		NZ,div_1
	RET

;----------------------------------------------------------------------------
; String Area
day_name_table:
	defw	d1
	defw	d2
	defw	d3
	defw	d4
	defw	d5
	defw	d6
	defw	d7

d1:	defm	"Saturday ",0
d2:	defm	"Sunday ",0
d3:	defm	"Monday ",0
d4:	defm	"Tuesday ",0
d5:	defm	"Wednesday ",0
d6:	defm	"Thursday ",0
d7:	defm	"Friday ",0

month_name_table:
	defw	m1
	defw	m2
	defw	m3
	defw	m4
	defw	m5
	defw	m6
	defw	m7
	defw	m8
	defw	m9
	defw	m10
	defw	m11
	defw	m12

m1:		defm  "January ",0
m2:		defm  "February ",0
m3:		defm  "March ",0
m4:		defm  "April ",0
m5:		defm  "May ",0
m6:		defm  "June ",0
m7:		defm  "July ",0
m8:		defm  "August ",0
m9:		defm  "September ",0
m10:	defm  "October ",0
m11:	defm  "November ",0
m12:	defm  "December ",0	

th_array:
	defw	th1
	defw	th2
	defw	th3

th1:		defm	"st ",0
th2:		defm	"nd ",0
th3:		defm	"rd ",0

th_string:	defm	"th ",0

AM_string:	defm	" AM, ",0
PM_string:	defm	" PM, ",0

;------------------------------------------------------------------
;
;	Various equates
;
;------------------------------------------------------------------

defc vers22	=	22h	; CP/M version number
defc vers31	=	31h	; CP/M version number
defc rev	=	'A'	; Utility version

defc bdos	=	0005h	;BDOS entry point
defc boot	=	0000h	;Home time

;BIOSPB:	defs	8	;for direct BIOS calls

;
todblk:
todday:	defw	0	; Day number 1 = Jan 1 1978
todhr:	defb	0	; hour (BCD)
todmin:	defb	0	; min (BCD)
todsec:	defb	0	; sec (BCD)
;
toddd:	defb	0
todmm:	defb	0
todyy:	defb	0
;
month:	defw	0,31,59,90,120,151,181,212,243,273,304,334,365


;*** BASIC CONSOLE I/O ROUTINES ***

tyo:
	ld e,a
	ld c,2
	jp	bdos

;	LD 	C,A
;	LD	DE,9
;	LD	HL,(0001H)
;	ADD	HL,DE
;	JP	(HL)


strout:
	LD		A,(HL)
	AND		A
	RET		Z
	PUSH	HL
	CALL	tyo
	POP		HL
	INC		HL
	JP		strout


addhl:	;Adds HL to A and returns value in HL
	ADD	A,L
	LD 	L,A
	RET	NC
	INC	H
	RET

;end dates
;the end

