;   .Z80


; How to build:
;    z88dk-z80asm -b -odate.com cpm3_date_y2k.asm

; How to create a CP/M disk image:

; Visual 1050 with CP/M Plus
;    z88dk-appmake +cpmdisk -f v1050 -b date.com

; Epson QX-10 / QC-10 with CP/M Plus
;    z88dk-appmake +cpmdisk -f qc10m1 -b date.com


; Y2K fixed DATE
;
; John Elliott 20/2/1999: Modified to use CP/M3 / Z80DOS / DOS+ calls rather than P2DOS ones. 
;
; The source file available online is strange, few bytes are missing after "BEGIN" 
; which are required to properly save the stack.
; This program still has a bug to be fixed, the displayed date does not consider the seconds,
; still it is an amazing Y2K fix.


org 100h

DEFC SETTIME  =  68h 
DEFC GETTIME  =  69h 
;
;;SETTIME EQU   201
;;GETTIME EQU   200
;
; EQUATES
;
DEFC CR  =  0DH 
DEFC LF  =  0AH 
;
; CALCULATE DAYS AND YEAR,MONTH,DAYS
;
BASE:
    DEFB    0EBh,04h    ;DOS protection... JMPS LABE
    EX  DE,HL
    JP  BEGIN
    DEFB    0B4h,09h    ;DOS protection... MOV AH,9
    DEFB    0BAh
    DEFW    BVMES       ;DOS protection... MOV DX,OFFSET BVMES
    DEFB    0CDh,021h   ;DOS protection... INT 21h.
    DEFB    0CDh,020h   ;DOS protection... INT 20h.


BEGIN:
	LD (SPSAVE),SP
    LD  SP,STACK
    LD  A,(00080H)
    ADD 81H
    LD  H,0
    LD  L,A
    LD  (HL),0
    LD  HL,00081H
    CALL    BLANK
    CP  '[' ;[HELP]
    JP  Z,HELP
    CP  '/'
    JP  NZ,START0
HELP:
    LD  DE,EXPLA
    LD  C,9
    CALL    5
    JP  EXIT
;
EXPLA:
    DEFM    "DATE. Date 20-Feb-1999. Version 5.01"
    DEFB    CR,LF,CR,LF
    DEFM    "With this program you can get and set the current date and time"
    DEFB    CR,LF,CR,LF
    DEFM    "usage :  'DATE'                   gives current date and time"
    DEFB    CR,LF
    DEFM    "         'DATE dd/mm/yy hh:mm:ss' sets current date and time"
    DEFB    CR,LF
    DEFM    "         'DATE s{et}'             sets current date and time in dialog mode"
    DEFB    CR,LF
    DEFM    "         'DATE c{ontinuous}'      display date and time continuous"
    DEFB    CR,LF
    DEFM    "         'DATE [HELP]'            display this help info"
    DEFB    '$'
BVMES:  DEFM    "This is a CP/M program!",CR,LF,'$'

START0: CP  0
    JP  Z,GET
    CP  'C'
    JP  Z,CONT
    CP  'c'
    JP  Z,CONT
    CP  'S'
    JP  Z,SET
    CP  's'
    JP  Z,SET
    PUSH    HL
    LD  DE,TIMEDT
    LD  C,GETTIME
    CALL    5
    POP HL
    CALL    GETDAT
    JP  C,ERROR
    PUSH    HL
    LD  L,E         ; YEAR
    LD  H,019H
    LD  E,B         ; MONTH
    CALL    CONHEX
    CALL    CHKDAT
    JP  C,ERROR
    CALL    DAYS
    LD  (TIMEDT),BC
    POP HL
    CALL    BLANK
    CP  0
    JR  Z,START1
    CALL    GETTIM
    JP  C,ERROR
    CALL    BLANK
    CP  0
    JP  NZ,ERROR
    CALL    CHKTIM
    JP  C,ERROR
    LD  A,B         ; HOUR
    LD  (HOUR),A
    LD  A,C         ; MINUTE
    LD  (MINUTE),A
    LD  A,E         ; SECOND
    LD  (SECOND),A  
START1: LD  DE,PRESS
    LD  C,9
    CALL    5
    LD  C,1
    CALL    5
    LD  DE,TIMEDT
    LD  C,SETTIME
    CALL    5
    LD  A,CR
    CALL    OUTCH
    LD  A,LF
    CALL    OUTCH
;   
GET:    LD  DE,TIMEDT
    LD  C,GETTIME
    CALL    5
    LD  BC,(TIMEDT)
    CALL    DMJ
    CALL    PRDMJ
    JP  EXIT
;
CONT:   LD  DE,TIMEDT
    LD  C,GETTIME
    CALL    5
    LD  BC,(TIMEDT)
    CALL    DMJ
    CALL    PRDMJ
CONT0:  LD  C,11
    CALL    5
    OR  A
    JR  NZ,CONT1
    LD  A,(SECOND)
    PUSH    AF
    LD  DE,TIMEDT
    LD  C,GETTIME
    CALL    5
    POP AF
    LD  B,A
    LD  A,(SECOND)
    CP  B
    JR  Z,CONT0
    LD  A,CR
    CALL    OUTCH
    JR  CONT
CONT1:  LD  C,1
    CALL    5
    JP  EXIT
;
SET:    LD  DE,DATMSG
    LD  C,9
    CALL    5
    LD  DE,00080H
    LD  A,126
    LD  (00080H),A
    LD  C,10
    CALL    5
    LD  A,LF
    CALL    OUTCH
    LD  DE,DATE
    LD  C,GETTIME
    CALL    5
    LD  HL,(DATE)
    LD  (TIMEDT),HL
    LD  A,(00081H)
    ADD 82H
    LD  H,0
    LD  L,A
    LD  (HL),0
    LD  HL,00082H
    CALL    BLANK
    CP  0
    JR  Z,SET0
    CALL    GETDAT
    JP  C,ERROR
    CALL    BLANK
    CP  0
    JP  NZ,ERROR
    LD  L,E         ; YEAR
    LD  H,019H
    LD  E,B         ; MONTH
    CALL    CONHEX
    CALL    CHKDAT
    JP  C,ERROR
    CALL    DAYS
    LD  (TIMEDT),BC
SET0:   LD  A,0FFH
    LD  (TDFLAG),A
    LD  DE,TIMMSG
    LD  C,9
    CALL    5
    LD  DE,00080H
    LD  A,126
    LD  (00080H),A
    LD  C,10
    CALL    5
    LD  A,LF
    CALL    OUTCH
    LD  DE,DATE
    LD  C,GETTIME
    CALL    5
    LD  HL,DATE+2
    LD  DE,HOUR
    LD  BC,3
    LDIR
    LD  A,(00081H)
    ADD 82H
    LD  H,0
    LD  L,A
    LD  (HL),0
    LD  HL,00082H
    CALL    BLANK
    CP  0
    JP  Z,SET1
    CALL    GETTIM
    JP  C,ERROR
    CALL    BLANK
    CP  0
    JP  NZ,ERROR
    CALL    CHKTIM
    JP  C,ERROR
    LD  A,B         ; HOUR
    LD  (HOUR),A
    LD  A,C         ; MINUTE
    LD  (MINUTE),A
    LD  A,E         ; SECOND
    LD  (SECOND),A  
SET1:   LD  DE,TIMEDT
    LD  C,SETTIME
    CALL    5
    JP  GET
;
ERROR:  LD  DE,ERRMSG
    LD  C,9
    CALL    5
    LD  A,(TDFLAG)
    OR  A
    JP  Z,SET
    JP  SET0
;
EXIT:   LD  SP,(SPSAVE)
    RET 
;
ERRMSG: DEFM    "Error in data input"
    DEFB    CR,LF,'$'
;
DATMSG: DEFM    "Enter today's date (DD/MM/YY): $"
;
TIMMSG: DEFM    "Enter the time (HH:MM:SS): $"
;
PRESS:  DEFM    "Press any key to set time $"
;
; GET DATE
;
GETDAT: LD  A,(HL)
    INC HL
    OR  A
    SCF
    RET Z
    CALL    DIGIT
    JR  C,GETDAT
    DEC HL
    CALL    NUMBER
    RET C
    LD  C,E         ; DAY
GETDT0: LD  A,(HL)
    INC HL
    OR  A
    SCF
    RET Z
    CALL    DIGIT
    JR  C,GETDT0
    DEC HL
    CALL    NUMBER
    RET C
    LD  B,E         ; MONTH
GETDT1: LD  A,(HL)
    INC HL
    OR  A
    SCF
    RET Z
    CALL    DIGIT
    JR  C,GETDT1
    DEC HL
    CALL    NUMBER          ; YEAR
    RET 
;
; GET TIME
;
GETTIM: LD  A,(HL)
    INC HL
    OR  A
    SCF
    RET Z
    CALL    DIGIT
    JR  C,GETTIM
    DEC HL
    CALL    NUMBER
    RET C
    LD  B,E         ; HOUR
GETT0:  LD  A,(HL)
    INC HL
    OR  A
    SCF
    RET Z
    CALL    DIGIT
    JR  C,GETT0
    DEC HL
    CALL    NUMBER
    RET C
    LD  C,E         ; MINUTE
    LD  E,0
GETT1:  LD  A,(HL)
    OR  A
    RET Z
    INC HL
    CALL    DIGIT
    JR  C,GETT1
    DEC HL
    CALL    NUMBER          ; SECOND
    RET
;
; CHECK DATE
;
CHKDAT: LD  A,E
    OR  A
    SCF
    RET Z
    CP  12+1
    CCF
    RET C
    PUSH    HL
    LD  D,0
    LD  HL,DM-1
    ADD HL,DE
    LD  B,(HL)          ; DAYS IN MONTH
    POP HL
    CP  2
    JR  NZ,CHKDT0
    CALL    LEAPYR
    JR  NZ,CHKDT0
    INC B
CHKDT0: LD  A,C
    OR  A
    SCF
    RET Z
    CP  B
    RET Z
    CCF
    RET
;
; CHECK TIME
;
CHKTIM: LD  A,B
    CP  24H
    CCF
    RET C
    LD  A,C
    CP  60H
    CCF
    RET C
    LD  A,E
    CP  60H
    CCF
    RET
;
; CALCULATE DAYS
;  ENTRY : HL=YEAR (1978..2157)
;          E =MONTH (1..12)
;          C =DAYS (1..31)
;  EXIT  : BC=DAYS
;             FIRST DAY (00001) : SU 01-01-1978
;             LAST  DAY (65535) : SU 05-06-2157
;
DAYS:   LD  B,0
    PUSH    DE
    LD  DE,1978
DAYS0:  OR  A
    SBC HL,DE
    ADD HL,DE
    JR  Z,DAYS1
    PUSH    HL
    LD  HL,365
    ADD HL,BC
    LD  B,H
    LD  C,L
    POP HL
    EX  DE,HL
    CALL    LEAPYR
    EX  DE,HL
    INC DE
    JR  NZ,DAYS0
    INC BC
    JR  DAYS0
DAYS1:  POP DE
    PUSH    HL
    LD  HL,DM
    LD  D,1
DAYS2:  LD  A,E
    CP  D
    JR  Z,DAYS4
    LD  A,C
    ADD (HL)
    LD  C,A
    JR  NC,DAYS3
    INC B
DAYS3:  INC HL
    LD  A,D
    INC D
    CP  2
    JR  NZ,DAYS2
    EX  (SP),HL
    CALL    LEAPYR
    EX  (SP),HL
    JR  NZ,DAYS2
    INC BC
    JR  DAYS2
DAYS4:  POP HL
    RET
;
; CALCULATE YEAR,MONTH,DAY
;  ENTRY : BC=DAYS
;             FIRST DAY (00001) : SU 01-01-1978
;             LAST  DAY (65535) : SU 05-06-2157
;  EXIT  : HL=YEAR (1978..2157)
;          D =DAYS IN LAST MONTH (1..31)
;          E =MONTH (1..12)
;          B =DAY OF WEEK (1..7, 1=SUNDAY)
;          C =DAYS (1..31)
;
DMJ:    PUSH    BC
    LD  HL,1978
DMJ0:   LD  DE,365
    CALL    LEAPYR
    JR  NZ,DMJ1
    INC DE
DMJ1:   LD  A,E
    SUB C
    LD  A,D
    SBC A,B
    JR  NC,DMJ2
    LD  A,C
    SUB E
    LD  C,A
    LD  A,B
    SBC A,D
    LD  B,A
    INC HL
    JR  DMJ0
DMJ2:   LD  E,1
    PUSH    HL
    LD  HL,DM
DMJ3:   LD  D,(HL)
    LD  A,E
    CP  2
    JR  NZ,DMJ4
    EX  (SP),HL
    CALL    LEAPYR
    EX  (SP),HL
    JR  NZ,DMJ4
    INC D
DMJ4:   LD  A,D
    SUB C
    LD  A,0
    SBC A,B
    JR  NC,DMJ5
    LD  A,C
    SUB D
    LD  C,A
    LD  A,B
    SBC A,0
    LD  B,A
    INC HL
    INC E
    JR  DMJ3
DMJ5:   POP HL
    EX  (SP),HL
    PUSH    BC
    DEC HL
    LD  BC,16*256+7
    XOR A
DMJ6:   RL  L
    RL  H
    RLA
    CP  C
    JR  C,DMJ7
    SUB C
DMJ7:   DJNZ    DMJ6    
    INC A
    POP BC
    LD  B,A
    POP HL
    RET
;
; CALCULATE LEAP YEAR CORRECTION
;  ENTRY : HL = YEAR
;  EXIT  : Z  = 1 CORRECTION NECCESARRY
;          Z  = 0 NO CORRECTION NEEDED
;
LEAPYR: LD  A,L
    AND 3
    RET NZ
    PUSH    HL
    PUSH    DE
    LD  A,-1
    LD  DE,100
    OR  A
LEAPY0: INC A
    SBC HL,DE
    JR  NC,LEAPY0
    ADD HL,DE
    LD  H,A
    LD  A,L
    OR  A
    JR  NZ,LEAPY1
    LD  A,H
    AND 3
    JR  LEAPY2
LEAPY1: XOR A
LEAPY2: POP DE
    POP HL
    RET
;
; DAYS/MONTH
;
DM: DEFB    31,28,31,30,31,30,31,31,30,31,30,31
;
; CONVERT DAYS,MONTH,YEAR TO HEX
;
CONHEX: LD  A,C
    CALL    BCDHEX
    LD  C,A
    LD  A,E
    CALL    BCDHEX
    LD  E,A
    PUSH    DE
    LD  DE,100
    LD  A,L
    CALL    BCDHEX
    LD  L,A
    LD  A,H
    CALL    BCDHEX
    LD  H,0
    OR  A
    JR  Z,CONHX1
CONHX0: ADD HL,DE
    DEC A
    JR  NZ,CONHX0
CONHX1: LD  DE,1978
    OR  A
    SBC HL,DE
    ADD HL,DE
    JR  NC,CONHX2
    LD  DE,100
    ADD HL,DE
CONHX2: POP DE
    RET
;
; PRINT: DAY IN WEEK,DAYS,MONTH,YEAR
;
PRDMJ:  PUSH    HL
    PUSH    BC
    LD  HL,DAYSWK-3
    LD  C,B
    LD  B,0
    ADD HL,BC
    ADD HL,BC
    ADD HL,BC
    LD  A,(HL)
    INC HL
    CALL    OUTCH
    LD  A,(HL)
    INC HL
    CALL    OUTCH
    LD  A,(HL)
    CALL    OUTCH
    POP BC
    POP HL
    LD  A,' '
    CALL    OUTCH
    LD  A,C
    CALL    HEXBCD
    CALL    OUTBCD
    LD  A,'-'
    CALL    OUTCH
    PUSH    HL
    PUSH    DE
    LD  D,0
    LD  HL,MONTHS-3
    ADD HL,DE
    ADD HL,DE
    ADD HL,DE
    LD  A,(HL)
    INC HL
    CALL    OUTCH
    LD  A,(HL)
    INC HL
    CALL    OUTCH
    LD  A,(HL)
    CALL    OUTCH
    POP DE
    POP HL
    LD  A,'-'
    CALL    OUTCH
    LD  A,E
    CALL    HEXBCD
    LD  E,A
    PUSH    DE
    LD  A,-1
    LD  DE,100
    OR  A
PRDMJ0: INC A
    SBC HL,DE
    JR  NC,PRDMJ0
    ADD HL,DE
    CALL    HEXBCD
    LD  H,A
    LD  A,L
    CALL    HEXBCD
    LD  L,A
    POP DE
    LD  A,H
    CALL    OUTBCD
    LD  A,L
    CALL    OUTBCD
    LD  A,' '
    CALL    OUTCH
    LD  A,(HOUR)
    CALL    OUTBCD
    LD  A,':'
    CALL    OUTCH
    LD  A,(MINUTE)
    CALL    OUTBCD
    LD  A,':'
    CALL    OUTCH
    LD  A,(SECOND)
    CALL    OUTBCD
    RET
;
; DAYS IN WEEK
;
MONTHS: DEFM    "JanFebMarAprMayJunJulAugSepOctNovDec"
;
DAYSWK: DEFM    "SunMonTueWedThuFriSat"
;
; CONVERT HEX TO BCD
;
HEXBCD: OR  A
    RET Z
    PUSH    BC
    LD  B,A
    XOR A
HXBCD0: ADD 1
    DAA
    DJNZ    HXBCD0
    POP BC
    RET
;
; CONVERT BCD TO HEX
;
BCDHEX: OR  A
    RET Z
    PUSH    BC
    LD  B,0
BCDHX0: INC B
    SUB 1
    DAA
    JR  NZ,BCDHX0
    LD  A,B
    POP BC
    RET
;
; OUTPUT BCD CHAR
;
OUTBCD: PUSH    AF
    RRCA
    RRCA
    RRCA
    RRCA
    AND 0FH
    ADD '0'
    CALL    OUTCH
    POP AF
    PUSH    AF
    AND 0FH
    ADD '0'
    CALL    OUTCH
    POP AF
    RET
;
; OUTPUT CHAR
;
OUTCH:  PUSH    AF
    PUSH    BC
    PUSH    DE
    PUSH    HL
    LD  E,A
    LD  C,2
    CALL    5
    POP HL
    POP DE
    POP BC
    POP AF
    RET
;
; INPUT NUMBER
;
NUMBER: CALL    BLANK
    LD  A,(HL)
    CALL    DIGIT
    RET C
    AND 0FH
    LD  E,A
    LD  D,0
NUMB0:  INC HL
    LD  A,(HL)
    CALL    DIGIT
    CCF
    RET NC
    EX  DE,HL
    ADD HL,HL
    ADD HL,HL
    ADD HL,HL
    ADD HL,HL
    AND 0FH
    ADD L
    LD  L,A
    EX  DE,HL
    LD  A,D
    OR  A
    SCF
    RET NZ
    JR  NUMB0
;
; BLANK INPUT
;
BLANK:  LD  A,(HL)
    CP  ' '
    JR  Z,BLANK0
    CP  09H
    RET NZ
BLANK0: INC HL
    JR  BLANK
;
; TEST DIGIT
;
DIGIT:  CP  '0'
    RET C
    CP  '9'+1
    CCF
    RET
;
TDFLAG: DEFB    0
;
TIMEDT: DEFW    0
HOUR:   DEFB    0
MINUTE: DEFB    0
SECOND: DEFB    0
;
DATE:   DEFW    0
    DEFS    3
;
SPSAVE: DEFS    2
    DEFS    64
STACK:

;
;   END
