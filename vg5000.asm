  ORG $0000

; Routine at 0
;
; Used by the routines at JPVEC_FDIVC and PROMPT.
L0000:
  JP _STARTUP
  CALL $2E31
  LD SP,$7E00

; Check syntax, 1 byte follows to be compared
;
; Used by the routines at L0C7C, L0D43, L0D45, L0E17, L0E2B, L0E61, L0F61,
; L0F73, L1BA2, L1C74, L1C80, L1CBE, L24BB, L2613, L2667, L2681, L271A, L2770,
; LOPDT2, FRMEQL, OPNPAR, L2910, L2943, L2A34, L2A36, L2A95, L2ABF, L2FAF,
; L302D, L306D, L3075, L3182, L3873, L3878, L38AA, L38D3 and DOCHRT.
SYNCHR:
  LD A,(HL)
  EX (SP),HL
  CP (HL)
  INC HL
  EX (SP),HL
  JP NZ,SN_ERR

; (a.k.a. GETCHR, GETNEXT), pick next char from program
;
; Used by the routines at _ASCTFP, JPVEC_FDIVC, __CLOAD, L1BA4, L1C82, L1CC0,
; PROMPT, FNDWRD, L24BD, _CHRGTB, FC_ERR, ATOH, L2683, __LPRINT, NEXITM,
; SCNVAL, INPBIN, FDTLP, STKTHS, OPRND, EVAL_VARIABLE, __DEF, L2A38, FNDNUM,
; MAKINT, __CLEAR, L2FB1, __NEXT, L302F, SCCPTR, __LIST, __VAL, DIMRET and
; GETVAR.
CHRGTB:
  INC HL
  LD A,(HL)
  CP $3A
  RET NC
  JP _CHRGTB_0
; This entry point is used by the routines at JPVEC_FDIVC, L1BA4, PROMPT,
; L271C, GTVLUS, T_EDIT, __LIST, PRS1, INLPNM, OUTDO_CRLF, SHIFT_STOP, __NEW
; and QINLIN.
CHRGTB_0:
  JP _OUTDO
  JP L0D47_4
  RST $38
  NOP

; compare DE and HL (aka CPDEHL)
;
; Used by the routines at _STARTUP, JPVEC_FDIVC, L1CC0, BAKSTK, ERROR, PROMPT,
; SRCHLP, FNDWRD, __FOR, ATOH, __GOTO, L2615, MOVUP, L2FB1, L3077, __LIST,
; TSTOPL, TESTR, GSTRDE, BAKTMP, GETVAR, FNDARY, BS_ERR, INLPNM and SHIFT_STOP.
DCOMPR:
  LD A,H
  SUB D
  RET NZ
  LD A,L
  SUB E
  RET
  ADD HL,SP
  CALL M,$E93A

; Test sign in number
;
; Used by the routines at __LOG, FMULT, FDIV, RESDIV, __SGN, __ABS, FCOMP,
; NUMASC, POWER, __RND, __SIN, __ATN, JPVEC_FDIVC, L24BD, DEPINT and L2683.
VSIGN:
  LD A,(FPEXP)
  OR A
  JP NZ,SIGN
  RET
; This entry point is used by the routine at JPVEC_FDIVC.
_RST:
  JP RSTHK
_WARM_BT:
  JP WARM_BOOT
  RST $38
  RST $38
; This entry point is used by the routines at CHRGTB, _DISPLAY, JPVEC_FDIVC and
; L1CC0.
_INT:
  CALL INT_SUB
  PUSH AF
  DEC (IX+$00)
  JP NZ,L00FD_8
  LD A,(IX+$02)
  LD (IX+$00),A
  BIT 0,(IX+$01)
  RES 0,(IX+$01)
  JP Z,L00FD_8
  PUSH BC
  PUSH DE
  PUSH HL

; Screen display
_DISPLAY:
  LD A,$28
  OUT ($8F),A
  LD A,$82
  OUT ($CF),A
  LD A,$29
  OUT ($8F),A
  JP DISPLAY
  RST $38
  JP NMIHK

; NMI handler at boot time
NMI_BOOT:
  LD A,$01
  LD (FRGFLG),A
  LD HL,KEYTAB
  LD (KBDTBL),HL
  RETN

; Data block at 118
L0076:
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF

; USR routine return
_USR_RET:
  JP USR_RET

; parameter acquisition on 2 bytes
__CONIS:
  JP CONIS

; parameter acquisition on 1 byte
__GETINT:
  JP GETINT

; acquisition parameter sign on 2 bytes
_GETPARM_2BYTE:
  JP GETPARM_2BYTE

; emitting a sound
_SOUND:
  JP SOUND

; play a melody
_MUSIC:
  JP MUSIC

; character display in X=L, Y=H
_PUTCHAR_XY:
  JP PUTCHAR_XY

; display character at cursor position
_PUTCHAR:
  JP PUTCHAR

; read character in X=L, Y=H
_GETCHAR_XY:
  JP GETCHAR_XY

; read character at cursor position
_GETCHAR:
  JP GETCHAR

; clear the screen and initialize colors
_CLR_SCR:
  JP CLR_SCR

; erase line and initialize colors
_CLR_LINE:
  JP CLR_LINE

; wait EF9345 ready
_VDP_READY:
  JP VDP_READY

; screen address calculation in X, Y
_SCRADR:
  JP SCRADR

; keyboard scanning
_KBDSCAN:
  JP KBDSCAN

; set video processor registers, sequence in (HL), first byte=length
_SET_VDP_REGS:
  JP SET_VDP_REGS

; scan joysticks (up/down)
_JOY_UP_DOWN:
  JP JOY_UP_DOWN

; scan joysticks (right/left)
_JOY_L_R:
  JP JOY_L_R

; scan joysticks (action1/action2)
_JOY_FIRE_F2:
  JP JOY_FIRE_F2

; scans SHIFT / STOP keys
_SHIFT_STOP:
  JP SHIFT_STOP

; Screen display (continued)
;
; Used by the routine at _DISPLAY.
DISPLAY:
  LD A,(IX+$03)
  AND $BF
  OUT ($CF),A
  LD C,$CF
  LD A,$26
  OUT ($8F),A
  XOR A
  OUT (C),A
  LD A,$27
  OUT ($8F),A
  XOR A
  OUT (C),A

; Send the first line of the screen
L00D3:
  LD HL,SCREEN
  LD E,$03
  LD B,$50
  LD A,$20
  OUT ($8F),A
  OUT (C),E
L00D3_0:
  LD A,$22
  OUT ($8F),A
  OUTI
  LD A,$29
  OUT ($8F),A
  OUTI
  JR NZ,L00D3_0
  LD A,$26
  OUT ($8F),A
  LD A,$08
  OUT (C),A
  LD A,$27
  OUT ($8F),A
  XOR A
  OUT (C),A

; Send the following lines
L00FD:
  LD D,$18
L00FD_0:
  LD B,$78
  LD A,$20
  OUT ($8F),A
L00FD_1:
  IN A,($CF)
  OR A
  JP M,L00FD_1
  LD A,$20
  OUT ($8F),A
  OUT (C),E
L00FD_2:
  LD A,$20
  OUT ($8F),A
L00FD_3:
  IN A,($CF)
  OR A
  JP M,L00FD_3
  LD A,$22
  OUT ($8F),A
  OUTI
  LD A,$29
  OUT ($8F),A
  OUTI
  DJNZ L00FD_2
  LD A,$20
  OUT ($8F),A
L00FD_4:
  IN A,($CF)
  OR A
  JP M,L00FD_4
  LD A,$28
  OUT ($8F),A
  LD A,$B0
  OUT (C),A
  DEC D
  JR NZ,L00FD_0
  BIT 6,(IX+$03)
  JR Z,L00FD_7
  LD HL,(CURPOS)
  LD A,H
  OR A
  JR Z,L00FD_5
  ADD A,$07
  LD H,A
L00FD_5:
  LD C,$CF
  LD A,$26
  OUT ($8F),A
  OUT (C),H
  LD A,$27
  OUT ($8F),A
  OUT (C),L
  LD A,$28
  OUT ($8F),A
  LD A,$0A
  OUT (C),A
  LD A,$20
  OUT ($8F),A
L00FD_6:
  IN A,($CF)
  OR A
  JP M,L00FD_6
  LD A,$28
  OUT ($8F),A
  LD A,$82
  OUT ($CF),A
  LD A,$29
  OUT ($8F),A
  SET 6,(IX+$03)
  LD A,(IX+$03)
  OUT ($CF),A
L00FD_7:
  POP HL
  POP DE
  POP BC
; This entry point is used by the routine at VSIGN.
L00FD_8:
  INC (IX+$06)
  POP AF
  EI
  RET

; character display in X=L, Y=H
;
; Used by the routine at _PUTCHAR_XY.
PUTCHAR_XY:
  LD C,$CF
  LD A,$26
  OUT ($8F),A
  OUT (C),H
  LD A,$27
  OUT ($8F),A
  OUT (C),L

; display character at cursor position
;
; Used by the routine at _PUTCHAR.
PUTCHAR:
  LD BC,$03CF
  LD A,$20
  OUT ($8F),A
PUTCHAR_0:
  IN A,($CF)
  OR A
  JP M,PUTCHAR_0
  LD A,$22
  OUT ($8F),A
  OUT (C),D
  LD A,$20
  OUT ($8F),A
PUTCHAR_1:
  IN A,($CF)
  OR A
  JP M,PUTCHAR_1
  LD A,$21
  OUT ($8F),A
  OUT (C),E
  LD A,$28
  OUT ($8F),A
  OUT (C),B
  RET

; read character in X=L, Y=H
;
; Used by the routine at _GETCHAR_XY.
GETCHAR_XY:
  LD C,$CF
  LD A,$26
  OUT ($8F),A
  OUT (C),H
  LD A,$27
  OUT ($8F),A
  OUT (C),L

; read character at cursor position
;
; Used by the routine at _GETCHAR.
GETCHAR:
  LD BC,$0BCF
  LD A,$28
  OUT ($8F),A
  OUT (C),B
  LD A,$20
  OUT ($8F),A
GETCHAR_0:
  IN A,($CF)
  OR A
  JP M,GETCHAR_0
  LD A,$22
  OUT ($8F),A
  IN D,(C)
  LD A,$20
  OUT ($8F),A
GETCHAR_1:
  IN A,($CF)
  OR A
  JP M,GETCHAR_1
  LD A,$21
  OUT ($8F),A
  IN E,(C)
  RET
; This entry point is used by the routines at L0F75, L334F and L33CD.
GETCHAR_2:
  OR (IX+$0A)
  LD HL,(CURPOS)
  PUSH HL
  PUSH AF
  CALL SCRADR
  POP AF
  LD (HL),A
  LD A,(IX+$08)
  INC HL
  LD (HL),A
  INC HL
  EX DE,HL
  LD HL,PICFLG
  RES 2,(HL)
  POP HL
  LD A,L
  CP $27
  JR C,GETCHAR_4
  LD L,$00
  INC H
  LD A,H
  CP $19
  JR C,GETCHAR_3
  DEC H
  LD A,(PICFLG)
  SET 6,A
  LD (PICFLG),A
  BIT 7,A
  JR NZ,GETCHAR_4
  PUSH HL
  CALL L33CD_39
  LD A,$18
  CALL CLR_LINE
  POP HL
  LD (IX+$00),$01
  LD DE,$4780
GETCHAR_3:
  LD A,(PICFLG)
  BIT 0,A
  JR Z,GETCHAR_4
  SET 2,A
  LD (PICFLG),A
  LD A,$84
  LD (DE),A
GETCHAR_4:
  INC L
  LD (CURPOS),HL
  RET

; clear the screen and initialize colors
;
; Used by the routines at _CLR_SCR, T_EDIT and SCR_CO.
CLR_SCR:
  XOR A
  CALL CLR_LINE
  LD HL,SCREEN
  LD DE,$4050
  LD BC,$0780
  LDIR
  RET

; erase line and initialize colors
;
; Used by the routines at _CLR_LINE, GETCHAR, CLR_SCR and L33CD.
CLR_LINE:
  LD L,$00
  LD H,A
  CALL SCRADR
  LD (HL),$80
  LD A,(IX+$09)
  INC HL
  LD (HL),A
  LD C,(IX+$08)
  INC HL
  LD A,$20
  BIT 7,C
  JR Z,CLR_LINE_0
  XOR A
CLR_LINE_0:
  LD B,$27
CLR_LINE_1:
  LD (HL),A
  INC HL
  LD (HL),C
  INC HL
  DJNZ CLR_LINE_1
  RET

; wait EF9345 ready
;
; Used by the routines at _VDP_READY, SET_VDP_REGS and L0D47.
VDP_READY:
  LD A,$20
  OUT ($8F),A
VDP_READY_0:
  IN A,($CF)
  OR A
  JP M,VDP_READY_0
  RET

; screen address calculation in X, Y
;
; Used by the routines at _SCRADR, GETCHAR, CLR_LINE, T_EDIT and L33CD.
SCRADR:
  LD C,L
  LD L,H
  LD H,$00
  ADD HL,HL
  ADD HL,HL
  ADD HL,HL
  LD D,H
  LD E,L
  ADD HL,HL
  ADD HL,HL
  ADD HL,DE
  ADD HL,HL
  LD B,$00
  ADD HL,BC
  ADD HL,BC
  LD BC,SCREEN
  ADD HL,BC
  RET

; keyboard scanning
;
; Used by the routines at _KBDSCAN, __KEY, __LIST, SCR_CO and SHIFT_STOP.
KBDSCAN:
  LD BC,$0787
KBDSCAN_0:
  IN A,(C)
  CPL
  OR A
  JR NZ,KBDSCAN_1
  DEC C
  DJNZ KBDSCAN_0
  IN A,(C)
  CPL
  AND $BB
KBDSCAN_1:
  JR Z,KBDSCAN_6
  LD B,$07
KBDSCAN_2:
  ADD A,A
  JR C,KBDSCAN_3
  DJNZ KBDSCAN_2
KBDSCAN_3:
  LD A,B
  ADD A,A
  ADD A,A
  ADD A,A
  ADD A,C
  SUB $80
  LD C,$80
  IN B,(C)
  LD DE,$0040
  LD HL,(KBDTBL)
  BIT 2,B
  JR NZ,KBDSCAN_4
  ADD HL,DE
  JR KBDSCAN_5
KBDSCAN_4:
  BIT 6,B
  JR NZ,KBDSCAN_5
  ADD HL,DE
  ADD HL,DE
KBDSCAN_5:
  LD E,A
  LD D,$00
  ADD HL,DE
  LD A,(HL)
KBDSCAN_6:
  CP (IX+$05)
  SCF
  JR Z,KBDSCAN_7
  LD (IX+$05),A
  CCF
KBDSCAN_7:
  RET

; set video processor registers, sequence in (HL), first byte=length
;
; Used by the routines at _SET_VDP_REGS and _STARTUP.
SET_VDP_REGS:
  LD B,(HL)
  INC HL
SET_VDP_REGS_0:
  CALL VDP_READY
  LD C,$8F
  OUTI
  LD C,$CF
  OUTI
  JR NZ,SET_VDP_REGS_0
  RET

; BASIC FP mathematics
;
; Used by the routine at NUMASC.
FADDH:
  LD HL,FP_HALF
; This entry point is used by the routines at __RND, __COS and __NEXT.
FADDS:
  CALL LOADFP
  JR FADD

; SUBTRACTION FAC:=ARG-FAC
FSUBS:
  CALL LOADFP
  LD HL,$D1C1

; Formerly SUBCDE, Subtract BCDE from FP reg
;
; Used by the routines at __EXP and __SIN.
FSUB:
  CALL NEG

; a.k.a. FPADD, Add BCDE to FP reg
;
; Used by the routines at FADDH, __LOG, MLSP10, _ASCTFP, POLY and __SIN.
FADD:
  LD A,B
  OR A
  RET Z
  LD A,(FPEXP)
  OR A
  JP Z,FPBCDE
  SUB B
  JR NC,NOSWAP
  CPL
  INC A
  EX DE,HL
  CALL PUSHF
  EX DE,HL
  CALL FPBCDE
  POP BC
  POP DE
NOSWAP:
  CP $19
  RET NC
  PUSH AF
  CALL UNPACK
  LD H,A
  POP AF
  CALL SCALE
  LD A,H
  OR A
  LD HL,FACCU
  JP P,MINCDE
  CALL PLUCDE
  JR NC,RONDUP
  INC HL
  INC (HL)
  JP Z,OV_ERR
  LD L,$01
  CALL SHRITE_1
  JR RONDUP
MINCDE:
  XOR A
  SUB B
  LD B,A
  LD A,(HL)
  SBC A,E
  LD E,A
  INC HL
  LD A,(HL)
  SBC A,D
  LD D,A
  INC HL
  LD A,(HL)
  SBC A,C
  LD C,A
; This entry point is used by the routines at FLOATR and __INT.
CONPOS:
  CALL C,COMPL
; This entry point is used by the routine at __RND.
BNORM:
  LD L,B
  LD H,E
  XOR A
BNRMLP:
  LD B,A
  LD A,C
  OR A
  JR NZ,PNORM
  LD C,D
  LD D,H
  LD H,L
  LD L,A
  LD A,B
  SUB $08
  CP $E0
  JR NZ,BNRMLP
; This entry point is used by the routines at RESDIV, _ASCTFP, __EXP and __VAL.
ZERO:
  XOR A
; This entry point is used by the routine at POWER.
ZERO0:
  LD (FPEXP),A
  RET
NORMAL:
  LD A,H
  OR L
  OR D
  JR NZ,BNORM_1
  LD A,C
BNORM_0:
  DEC B
  RLA
  JR NC,BNORM_0
  INC B
  RRA
  LD C,A
  JR BNORM_2
BNORM_1:
  DEC B
  ADD HL,HL
  LD A,D
  RLA
  LD D,A
  LD A,C
  ADC A,A
  LD C,A
PNORM:
  JP P,NORMAL
BNORM_2:
  LD A,B
  LD E,H
  LD B,L
  OR A
  JR Z,RONDUP
  LD HL,FPEXP
  ADD A,(HL)
  LD (HL),A
  JR NC,ZERO
  JR Z,ZERO
RONDUP:
  LD A,B
; This entry point is used by the routine at RESDIV.
RONDUP_0:
  LD HL,FPEXP
  OR A
  CALL M,FPROND
  LD B,(HL)
  INC HL
  LD A,(HL)
  AND $80
  XOR C
  LD C,A
  JP FPBCDE
; This entry point is used by the routine at QINT.
FPROND:
  INC E
  RET NZ
  INC D
  RET NZ
  INC C
  RET NZ
  LD C,$80
  INC (HL)
  RET NZ
  JP OV_ERR

; Add number pointed by HL to CDE
;
; Used by the routines at FADD and NUMASC.
PLUCDE:
  LD A,(HL)
  ADD A,E
  LD E,A
  INC HL
  LD A,(HL)
  ADC A,D
  LD D,A
  INC HL
  LD A,(HL)
  ADC A,C
  LD C,A
  RET

; Convert a negative number to positive
;
; Used by the routines at FADD and QINT.
COMPL:
  LD HL,SGNRES
  LD A,(HL)
  CPL
  LD (HL),A
  XOR A
  LD L,A
  SUB B
  LD B,A
  LD A,L
  SBC A,E
  LD E,A
  LD A,L
  SBC A,D
  LD D,A
  LD A,L
  SBC A,C
  LD C,A
  RET
; This entry point is used by the routines at FADD and QINT.
SCALE:
  LD B,$00
SCALLP:
  SUB $08
  JR C,SHRITE
  LD B,E
  LD E,D
  LD D,C
  LD C,$00
  JR SCALLP
SHRITE:
  ADD A,$09
  LD L,A
  LD A,D
  OR E
  OR B
  JR NZ,SHRLP
  LD A,C
SHRITE_0:
  DEC L
  RET Z
  RRA
  LD C,A
  JR NC,SHRITE_0
  JR SHRITE_2
SHRLP:
  XOR A
  DEC L
  RET Z
  LD A,C
; This entry point is used by the routine at FADD.
SHRITE_1:
  RRA
  LD C,A
SHRITE_2:
  LD A,D
  RRA
  LD D,A
  LD A,E
  RRA
  LD E,A
  LD A,B
  RRA
  LD B,A
  JR SHRLP

; Data block at 1038
FP_UNITY:
  DEFB $00,$00,$00,$81

; Data block at 1042
FP_LOGTAB_P:
  DEFB $04

; Data block at 1043
L0413:
  DEFB $9A,$F7,$19,$83,$24,$63,$43,$83
  DEFB $75,$CD,$8D,$84,$A9,$7F,$83,$82

; Data block at 1059
FP_LOGTAB_Q:
  DEFB $04

; Data block at 1060
L0424:
  DEFB $00,$00,$00,$81,$E2,$B0,$4D,$83
  DEFB $0A,$72,$11,$83,$F4,$04,$35,$7F

; Routine at 1076
;
; Used by the routine at POWER.
__LOG:
  RST $28
  OR A
  JP PE,FC_ERR
  CALL __LOG_0
  LD BC,$8031
  LD DE,$7218
  JR FMULT
__LOG_0:
  CALL BCDEFP
  LD A,$80
  LD (FPEXP),A
  XOR B
  PUSH AF
  CALL PUSHF
  LD HL,FP_LOGTAB_P
  CALL POLY
  POP BC
  POP HL
  CALL PUSHF
  EX DE,HL
  CALL FPBCDE
  LD HL,FP_LOGTAB_Q
  CALL POLY
  POP BC
  POP DE
  CALL FDIV
  POP AF
  CALL PUSHF
  CALL FLOAT
  POP BC
  POP DE
  JP FADD
  LD HL,$D1C1

; MULTIPLICATION, FAC:=ARG*FAC
;
; Used by the routines at __LOG, EXP, __EXP, SUMSER, POLY, __RND and __SIN.
FMULT:
  RST $28
  RET Z
  LD L,$00
  CALL ADDEXP
  LD A,C
  LD (MULVAL),A
  EX DE,HL
  LD (MULVAL2),HL
  LD BC,L0000
  LD D,B
  LD E,B
  LD HL,BNORM
  PUSH HL
  LD HL,MULT8
  PUSH HL
  PUSH HL
  LD HL,FACCU

; 8 bit multiply
MULT8:
  LD A,(HL)
  INC HL
  OR A
  JR Z,BYTSFT
  PUSH HL
  LD L,$08
; This entry point is used by the routine at NOMADD.
MUL8LP:
  RRA
  LD H,A
  LD A,C
  JR NC,NOMADD
  PUSH HL
  LD HL,(MULVAL2)
  ADD HL,DE
  EX DE,HL
  POP HL
  LD A,(MULVAL)
  ADC A,C

; Routine at 1202
;
; Used by the routine at MULT8.
NOMADD:
  RRA
  LD C,A
  LD A,D
  RRA
  LD D,A
  LD A,E
  RRA
  LD E,A
  LD A,B
  RRA
  LD B,A
  AND $10
  JR Z,NOMADD_0
  LD A,B
  OR $20
  LD B,A
NOMADD_0:
  DEC L
  LD A,H
  JR NZ,MUL8LP
; This entry point is used by the routines at RESDIV, GETVAR and FNDARY.
POPHLRT:
  POP HL
  RET
; This entry point is used by the routine at MULT8.
BYTSFT:
  LD B,E
  LD E,D
  LD D,C
  LD C,A
  RET

; Divide FP by 10
;
; Used by the routines at _ASCTFP and NUMASC.
DIV10:
  CALL PUSHF
  LD BC,$8420
  LD DE,L0000
  CALL FPBCDE

; Divide FP by number on stack
;
; Used by the routine at __TAN.
DIV:
  POP BC
  POP DE

; DIVISION, FAC:=ARG/FAC
;
; Used by the routines at __LOG and __ATN.
FDIV:
  RST $28
  JP Z,O_ERR
  LD L,$FF
  CALL ADDEXP
  INC (HL)
  JP Z,OV_ERR
  INC (HL)
  JP Z,OV_ERR
  DEC HL
  LD A,(HL)
  LD (DIV3),A
  DEC HL
  LD A,(HL)
  LD (DIV2),A
  DEC HL
  LD A,(HL)
  LD (DIV1),A
  LD B,C
  EX DE,HL
  XOR A
  LD C,A
  LD D,A
  LD E,A
  LD (DIV4),A
; This entry point is used by the routine at RESDIV.
FDIV_0:
  PUSH HL
  PUSH BC
  LD A,L
  CALL FDIVC
  SBC A,$00
  CCF
  JR NC,RESDIV
  LD (DIV4),A
  POP AF
  POP AF
  SCF

; ;"JP NC,nn" to skip the next 2 bytes
L0518:
  JP NC,MULVAL2_0

; Routine at 1305
;
; Used by the routine at FDIV.
RESDIV:
  POP BC
  POP HL
  LD A,C
  INC A
  DEC A
  RRA
  JP P,DIV2A
  RLA
  LD A,(DIV4)
  RRA
  AND $C0
  PUSH AF
  LD A,B
  OR H
  OR L
  JR Z,RESDIV_0
  LD A,$20
RESDIV_0:
  POP HL
  OR H
  JP RONDUP_0
DIV2A:
  RLA
  LD A,E
  RLA
  LD E,A
  LD A,D
  RLA
  LD D,A
  LD A,C
  RLA
  LD C,A
  ADD HL,HL
  LD A,B
  RLA
  LD B,A
  LD A,(DIV4)
  RLA
  LD (DIV4),A
  LD A,C
  OR D
  OR E
  JR NZ,FDIV_0
  PUSH HL
  LD HL,FPEXP
  DEC (HL)
  POP HL
  JR NZ,FDIV_0
  JP ZERO
; This entry point is used by the routines at FMULT and FDIV.
ADDEXP:
  LD A,B
  OR A
  JR Z,OVTST3
  LD A,L
  LD HL,FPEXP
  XOR (HL)
  ADD A,B
  LD B,A
  RRA
  XOR B
  LD A,B
  JP P,DIV_OVTST2
  ADD A,$80
  LD (HL),A
  JP Z,POPHLRT
  CALL UNPACK
  LD (HL),A
  DEC HL
  RET
  RST $28
  CPL
  POP HL
DIV_OVTST2:
  OR A
OVTST3:
  POP HL
  JP P,ZERO
  JP OV_ERR

; Multiply number in FPREG by 10
;
; Used by the routine at _ASCTFP.
MLSP10:
  CALL BCDEFP
  LD A,B
  OR A
  RET Z
  ADD A,$02
  JP C,OV_ERR
  LD B,A
  CALL FADD
  LD HL,FPEXP
  INC (HL)
  RET NZ
  JP OV_ERR

; PUT SIGN OF FAC IN A
;
; Used by the routine at VSIGN.
SIGN:
  LD A,(FACCU+2)

; CP 2Fh ..hides the "CPL" instruction      ;"CPI" AROUND NEXT BYTE
L059D:
  DEFB $FE

; Routine at 1438
FCOMPS:
  CPL

; Routine at 1439
ICOMPS:
  RLA
; This entry point is used by the routine at CSLOOP.
SIGNS:
  SBC A,A
  RET NZ
  INC A
  RET

; Routine at 1444
__SGN:
  RST $28

; a.k.a. FLGREL, Float the signed integer in A (CY and A to FP, & normalise)
;
; Used by the routines at __LOG, _ASCTFP and L2945.
FLOAT:
  LD B,$88
  LD DE,L0000

; a.k.a. RETINT
;
; Used by the routines at PRNTHL, ABPASS and L302F.
FLOATR:
  LD HL,FPEXP
  LD C,A
  LD (HL),B
  LD B,$00
  INC HL
  LD (HL),$80
  RLA
  JP CONPOS

; Routine at 1464
__ABS:
  RST $28
  RET P

; Invert number sign
;
; Used by the routines at FSUB, _ASCTFP, NUMASC, POWER, __SIN, __ATN and MINUS.
NEG:
  LD HL,FACCU+2
  LD A,(HL)
  XOR $80
  LD (HL),A
  RET

; a.k.a. STAKFP, Put FP value on stack
;
; Used by the routines at FADD, __LOG, DIV10, _ASCTFP, __SQR, __EXP, SUMSER,
; POLY, __SIN, __TAN and STKTHS.
PUSHF:
  EX DE,HL
  LD HL,(FACCU)
  EX (SP),HL
  PUSH HL
  LD HL,(FACCU+2)
  EX (SP),HL
  PUSH HL
  EX DE,HL
  RET

; a.k.a. PHLTFP, Number at HL to BCDE
;
; Used by the routines at __SQR, POLY, __RND, EVAL_VARIABLE and __NEXT.
MOVFM:
  CALL LOADFP

; Move BCDE to FPREG
;
; Used by the routines at FADD, __LOG, DIV10, NUMASC, GET_UNITY, __TAN and
; L2945.
FPBCDE:
  EX DE,HL
  LD (FACCU),HL
  LD H,B
  LD L,C
  LD (FACCU+2),HL
  EX DE,HL
  RET

; Load FP reg to BCDE
;
; Used by the routines at __LOG, MLSP10, QINT, NUMASC, POWER, SUMSER, __RND and
; L24BD.
BCDEFP:
  LD HL,FACCU

; Load FP value pointed by HL to BCDE
;
; Used by the routines at FADDH, FSUBS, MOVFM, POLY, __RND, __PLAY, __NEXT,
; STRCMP, PRS1 and TESTR.
LOADFP:
  LD E,(HL)
  INC HL
  LD D,(HL)
  INC HL
  LD C,(HL)
  INC HL
  LD B,(HL)
; This entry point is used by the routine at NUMASC.
LOADFP_0:
  INC HL
  RET

; Copy number in FPREG to HL ptr
;
; Used by the routines at __RND, L2615, INPBIN, L2A38 and __NEXT.
FPTHL:
  LD DE,FACCU

; Copy number value from DE to HL
;
; Used by the routines at L2615 and TSTOPL.
VMOVE:
  LD B,$04

; Copy B bytes from DE to HL
MOVE1:
  LD A,(DE)
  LD (HL),A
  INC DE
  INC HL
  DJNZ MOVE1
  RET
; This entry point is used by the routines at FADD, RESDIV and QINT.
UNPACK:
  LD HL,FACCU+2
  LD A,(HL)
  RLCA
  SCF
  RRA
  LD (HL),A
  CCF
  RRA
  INC HL
  INC HL
  LD (HL),A
  LD A,C
  RLCA
  SCF
  RRA
  LD C,A
  RRA
  XOR (HL)
  RET

; Compare FP reg to BCDE
;
; Used by the routines at NUMASC, POWER, __SIN, CONIS, L2945 and __NEXT.
FCOMP:
  LD A,B
  OR A
  JP Z,VSIGN
  LD HL,FCOMPS
  PUSH HL
  RST $28
  LD A,C
  RET Z
  LD HL,FACCU+2
  XOR (HL)
  LD A,C
  RET M
  CALL FCOMP_0
  RRA
  XOR C
  RET
FCOMP_0:
  INC HL
  LD A,B
  CP (HL)
  RET NZ
  DEC HL
  LD A,C
  CP (HL)
  RET NZ
  DEC HL
  LD A,D
  CP (HL)
  RET NZ
  DEC HL
  LD A,E
  SUB (HL)
  RET NZ
  POP HL
  POP HL
  RET

; a.k.a. FPINT, Floating Point to Integer
;
; Used by the routines at __INT, NUMASC and CONIS.
QINT:
  LD B,A
  LD C,A
  LD D,A
  LD E,A
  OR A
  RET Z
  PUSH HL
  CALL BCDEFP
  CALL UNPACK
  XOR (HL)
  LD H,A
  CALL M,DCBCDE
  LD A,$98
  SUB B
  CALL SCALE
  LD A,H
  RLA
  CALL C,FPROND
  LD B,$00
  CALL C,COMPL
  POP HL
  RET

; Decrement FP value in BCDE
;
; Used by the routine at QINT.
DCBCDE:
  DEC DE
  LD A,D
  AND E
  INC A
  RET NZ
  DEC BC
  RET

; Routine at 1632
;
; Used by the routines at POWER, __EXP and __SIN.
__INT:
  LD HL,FPEXP
  LD A,(HL)
  CP $98
  LD A,(FACCU)
  RET NC
  LD A,(HL)
  CALL QINT
  LD (HL),$98
  LD A,E
  PUSH AF
  LD A,C
  RLA
  CALL CONPOS
  POP AF
  RET

; Multiply DE by BC
;
; Used by the routines at BS_ERR and INLPNM.
MLDEBC:
  LD HL,L0000
  LD A,B
  OR C
  RET Z
  LD A,$10
MLDEBC_0:
  ADD HL,HL
  JP C,BS_ERR
  EX DE,HL
  ADD HL,HL
  EX DE,HL
  JR NC,MLDEBC_1
  ADD HL,BC
  JP C,BS_ERR
MLDEBC_1:
  DEC A
  JR NZ,MLDEBC_0
  RET

; ASCII to FP number (also '&' prefixes)
;
; Used by the routines at INPBIN, OPRND and __VAL.
H_ASCTFP:
  CP $26
  JP Z,OCTCNS

; ASCII to FP number
_ASCTFP:
  CP $2D
  PUSH AF
  JR Z,_ASCTFP_0
  CP $2B
  JR Z,_ASCTFP_0
  DEC HL
_ASCTFP_0:
  CALL ZERO
  LD B,A
  LD D,A
  LD E,A
  CPL
  LD C,A
_ASCTFP_1:
  RST $10
  JR C,_ASCTFP_DIGITS
  CP $2E
  JR Z,DPOINT
  CP $65
  JR Z,EXPONENTIAL
  CP $45
  JR NZ,_ASCTFP_3
EXPONENTIAL:
  RST $10
  CALL SGNEXP
_ASCTFP_2:
  RST $10
  JR C,_ASCTFP_9
  INC D
  JR NZ,_ASCTFP_3
  XOR A
  SUB E
  LD E,A
  INC C
DPOINT:
  INC C
  JR Z,_ASCTFP_1
_ASCTFP_3:
  PUSH HL
  LD A,E
  SUB B
_ASCTFP_4:
  CALL P,_ASCTFP_6
  JP P,_ASCTFP_5
  PUSH AF
  CALL DIV10
  POP AF
  INC A
_ASCTFP_5:
  JR NZ,_ASCTFP_4
  POP DE
  POP AF
  CALL Z,NEG
  EX DE,HL
  RET
_ASCTFP_6:
  RET Z
; This entry point is used by the routine at NUMASC.
_ASCTFP_7:
  PUSH AF
  CALL MLSP10
  POP AF
  DEC A
  RET
_ASCTFP_DIGITS:
  PUSH DE
  LD D,A
  LD A,B
  ADC A,C
  LD B,A
  PUSH BC
  PUSH HL
  PUSH DE
  CALL MLSP10
  POP AF
  SUB $30
  CALL _ASCTFP_8
  POP HL
  POP BC
  POP DE
  JR _ASCTFP_1
_ASCTFP_8:
  CALL PUSHF
  CALL FLOAT
  POP BC
  POP DE
  JP FADD
_ASCTFP_9:
  LD A,E
  RLCA
  RLCA
  ADD A,E
  RLCA
  ADD A,(HL)
  SUB $30
  LD E,A
  JR _ASCTFP_2
; This entry point is used by the routines at ERROR and SCCPTR.
_ASCTFP_10:
  PUSH HL
  LD HL,IN_MSG
  LD A,(FRGFLG)
  OR A
  JR Z,_ASCTFP_11
  LD HL,IN_MSG_FR
_ASCTFP_11:
  CALL PRS
  POP HL
; This entry point is used by the routines at _STARTUP, T_EDIT, SCCPTR, __LIST
; and SHIFT_STOP.
_ASCTFP_12:
  LD DE,PRNUMS
  PUSH DE

; Print number in HL
PRNTHL:
  EX DE,HL
  XOR A
  LD B,$98
  CALL FLOATR

; Number to ASCII conversion
;
; Used by the routines at __PRINT and __STR_S.
NUMASC:
  LD HL,$49EB
  PUSH HL
  RST $28
  LD (HL),$20
  JP P,NUMASC_0
  LD (HL),$2D
NUMASC_0:
  INC HL
  LD (HL),$30
  JP Z,NUMASC_11
  PUSH HL
  CALL M,NEG
  XOR A
  PUSH AF
  CALL NUMASC_13
NUMASC_1:
  LD BC,$9143
  LD DE,$4FF8
  CALL FCOMP
  OR A
  JP PO,NUMASC_3
  POP AF
  CALL _ASCTFP_7
  PUSH AF
  JR NUMASC_1
NUMASC_2:
  CALL DIV10
  POP AF
  INC A
  PUSH AF
  CALL NUMASC_13
NUMASC_3:
  CALL FADDH
  INC A
  CALL QINT
  CALL FPBCDE
  LD BC,$0306
  POP AF
  ADD A,C
  INC A
  JP M,NUMASC_4
  CP $08
  JR NC,NUMASC_4
  INC A
  LD B,A
  LD A,$02
NUMASC_4:
  DEC A
  DEC A
  POP HL
  PUSH AF
  LD DE,$0806
  DEC B
  JR NZ,NUMASC_5
  LD (HL),$2E
  INC HL
  LD (HL),$30
  INC HL
NUMASC_5:
  DEC B
  LD (HL),$2E
  CALL Z,LOADFP_0
  PUSH BC
  PUSH HL
  PUSH DE
  CALL BCDEFP
  POP HL
  LD B,$2F
NUMASC_6:
  INC B
  LD A,E
  SUB (HL)
  LD E,A
  INC HL
  LD A,D
  SBC A,(HL)
  LD D,A
  INC HL
  LD A,C
  SBC A,(HL)
  LD C,A
  DEC HL
  DEC HL
  JR NC,NUMASC_6
  CALL PLUCDE
  INC HL
  CALL FPBCDE
  EX DE,HL
  POP HL
  LD (HL),B
  INC HL
  POP BC
  DEC C
  JR NZ,NUMASC_5
  DEC B
  JR Z,NUMASC_8
NUMASC_7:
  DEC HL
  LD A,(HL)
  CP $30
  JR Z,NUMASC_7
  CP $2E
  CALL NZ,LOADFP_0
NUMASC_8:
  POP AF
  JR Z,NUMASC_12
  LD (HL),$45
  INC HL
  LD (HL),$2B
  JP P,NUMASC_9
  LD (HL),$2D
  CPL
  INC A
NUMASC_9:
  LD B,$2F
NUMASC_10:
  INC B
  SUB $0A
  JR NC,NUMASC_10
  ADD A,$3A
  INC HL
  LD (HL),B
NUMASC_11:
  INC HL
  LD (HL),A
  INC HL
NUMASC_12:
  LD (HL),C
  POP HL
  RET
NUMASC_13:
  LD BC,$9474
  LD DE,$23F7
  CALL FCOMP
  OR A
  POP HL
  JP PO,NUMASC_2
  JP (HL)

; Constant ptr for 0.5 in FP
FP_HALF:
  DEFB $00,$00,$00,$80

; Data block at 2051
L0803:
  DEFB $40,$42,$0F,$A0,$86,$01,$10,$27
  DEFB $00,$E8,$03,$00,$64,$00,$00,$0A
  DEFB $00,$00,$01,$00,$00

; Negate number
;
; Used by the routines at POWER and __ATN.
NEGAFT:
  LD HL,NEG
  EX (SP),HL
  JP (HL)

; Routine at 2077
__SQR:
  CALL PUSHF
  LD HL,FP_HALF
  CALL MOVFM

; POWER
POWER:
  POP BC
  POP DE
  RST $28
  LD A,B
  JR Z,__EXP
  JP P,POWER_0
  OR A
  JP Z,O_ERR
POWER_0:
  OR A
  JP Z,ZERO0
  PUSH DE
  PUSH BC
  LD A,C
  OR $7F
  CALL BCDEFP
  JP P,POWER_2
  PUSH AF
  LD A,(FPEXP)
  CP $99
  JR C,POWER_1
  POP AF
  JR POWER_2
POWER_1:
  POP AF
  PUSH DE
  PUSH BC
  CALL __INT
  POP BC
  POP DE
  PUSH AF
  CALL FCOMP
  POP HL
  LD A,H
  RRA
POWER_2:
  POP HL
  LD (FACCU+2),HL
  POP HL
  LD (FACCU),HL
  CALL C,NEGAFT
  CALL Z,NEG
  PUSH DE
  PUSH BC
  CALL __LOG
  POP BC
  POP DE

; EXP
EXP:
  CALL FMULT

; Routine at 2164
;
; Used by the routine at POWER.
__EXP:
  LD BC,$8138
  LD DE,$AA3B
  CALL FMULT
  LD A,(FPEXP)
  CP $88
  JR NC,MUL_OVTST1
  CP $68
  JR C,GET_UNITY
  CALL PUSHF
  CALL __INT
  ADD A,$81
  POP BC
  POP DE
  JR Z,MUL_OVTST2
  PUSH AF
  CALL FSUB
  LD HL,FP_EXPTAB
  CALL POLY
  POP BC
  LD DE,L0000
  LD C,D
  JP FMULT
MUL_OVTST1:
  CALL PUSHF
MUL_OVTST2:
  LD A,(FACCU+2)
  OR A
  JP P,RESZER
  POP AF
  POP AF
  JP ZERO

; Routine at 2229
;
; Used by the routine at __EXP.
RESZER:
  JP OV_ERR

; Load '1' to FP accumulator
;
; Used by the routine at __EXP.
GET_UNITY:
  LD BC,$8100
  LD DE,L0000
  JP FPBCDE

; Data block at 2241
FP_EXPTAB:
  DEFB $07

; Data block at 2242
L08C2:
  DEFB $7C,$88,$59,$74

; Data block at 2246
L08C6:
  DEFB $E0,$97,$26,$77,$C4,$1D,$1E,$7A
  DEFB $5E,$50,$63,$7C,$1A,$FE,$75,$7E
  DEFB $18,$72,$31,$80,$00,$00,$00,$81

; Series math sub: POLYNOMIAL EVALUATOR AND THE RANDOM NUMBER GENERATOR
;
; Used by the routines at __SIN and __ATN.
SUMSER:
  CALL PUSHF
  LD DE,$0478
  PUSH DE
  PUSH HL
  CALL BCDEFP
  CALL FMULT
  POP HL

; POLYNOMIAL EVALUATOR, POINTER TO DEGREE+1 IS IN (HL), CONSTANTS SHOULD BE
; STORED IN REVERSE ORDER, FAC HAS X
;
; Used by the routines at __LOG and __EXP.
POLY:
  CALL PUSHF
  LD A,(HL)
  INC HL
  CALL MOVFM
  LD B,$F1
  POP BC
  POP DE
  DEC A
  RET Z
  PUSH DE
  PUSH BC
  PUSH AF
  PUSH HL
  CALL FMULT
  POP HL
  CALL LOADFP
  PUSH HL
  CALL FADD
  POP HL
  JR $08F6

; Routine at 2317
__RND:
  RST $28
  LD HL,SEED+2
  JP M,__RND_2
  LD HL,$4867
  CALL MOVFM
  LD HL,SEED+2
  RET Z
  ADD A,(HL)
  AND $07
  LD B,$00
  LD (HL),A
  INC HL
  ADD A,A
  ADD A,A
  LD C,A
  ADD HL,BC
  CALL LOADFP
  CALL FMULT
  LD A,(RNDCNT)
  INC A
  AND $03
  LD B,$00
  CP $01
  ADC A,B
  LD (RNDCNT),A
  LD HL,$096E
  ADD A,A
  ADD A,A
  LD C,A
  ADD HL,BC
  CALL FADDS
__RND_0:
  CALL BCDEFP
  LD A,E
  LD E,C
  XOR $4F
  LD C,A
  LD (HL),$80
  DEC HL
  LD B,(HL)
  LD (HL),$80
  LD HL,SEED
  INC (HL)
  LD A,(HL)
  SUB $AB
  JR NZ,__RND_1
  LD (HL),A
  INC C
  DEC D
  INC E
__RND_1:
  CALL BNORM
  LD HL,$4867
  JP FPTHL
__RND_2:
  LD (HL),A
  DEC HL
  LD (HL),A
  DEC HL
  LD (HL),A
  JR __RND_0
  LD L,B
  OR C
  LD B,(HL)
  LD L,B
  SBC A,C
  JP (HL)
  SUB D
  LD L,C
  DJNZ $094D
  LD (HL),L
  LD L,B

; Routine at 2430
;
; Used by the routine at __TAN.
__COS:
  LD HL,FP_HALFPI
  CALL FADDS

; Routine at 2436
;
; Used by the routine at __TAN.
__SIN:
  LD A,(FPEXP)
  CP $77
  RET C
  LD A,(FACCU+2)
  OR A
  JP P,__SIN_0
  AND $7F
  LD (FACCU+2),A
  LD DE,NEG
  PUSH DE
__SIN_0:
  LD BC,$7E22
  LD DE,$F983
  CALL FMULT
  CALL PUSHF
  CALL __INT
  POP BC
  POP DE
  CALL FSUB
  LD BC,$7F00
  LD DE,L0000
  CALL FCOMP
  JP M,__SIN_1
  LD BC,$7F80
  LD DE,L0000
  CALL FADD
  LD BC,$8080
  LD DE,L0000
  CALL FADD
  RST $28
  CALL P,NEG
  LD BC,$7F00
  LD DE,L0000
  CALL FADD
  CALL NEG
__SIN_1:
  LD A,(FACCU+2)
  OR A
  PUSH AF
  JP P,__SIN_2
  XOR $80
  LD (FACCU+2),A
__SIN_2:
  LD HL,FP_SINTAB
  CALL SUMSER
  POP AF
  RET P
  LD A,(FACCU+2)
  XOR $80
  LD (FACCU+2),A
  RET

; PI/2 fp CONSTANT
FP_HALFPI:
  DEFB $DB,$0F,$49,$81,$00,$00,$00,$7F
FP_SINTAB:
  DEFB $05

; Data block at 2563
L0A03:
  DEFB $FB

; Data block at 2564
L0A04:
  DEFB $D7,$1E,$86,$65,$26,$99,$87,$58
  DEFB $34,$23,$87,$E1,$5D,$A5,$86,$DB
  DEFB $0F,$49,$83

; Routine at 2583
__TAN:
  CALL PUSHF
  CALL __SIN
  POP BC
  POP HL
  CALL PUSHF
  EX DE,HL
  CALL FPBCDE
  CALL __COS
  JP DIV

; Routine at 2604
__ATN:
  RST $28
  CALL M,NEGAFT
  CALL M,NEG
  LD A,(FPEXP)
  CP $81
  JR C,__ATN_0
  LD BC,$8100
  LD D,C
  LD E,C
  CALL FDIV
  LD HL,FSUBS
  PUSH HL
__ATN_0:
  LD HL,FP_ATNTAB
  CALL SUMSER
  LD HL,FP_HALFPI
  RET

; ATN coefficient table
FP_ATNTAB:
  DEFB $09

; Data block at 2641
L0A51:
  DEFB $4A,$D7,$3B,$78

; Data block at 2645
L0A55:
  DEFB $02,$6E,$84,$7B,$FE,$C1,$2F,$7C
  DEFB $74,$31,$9A,$7D,$84,$3D,$5A,$7D
  DEFB $C8,$7F,$91,$7E,$E4,$BB,$4C,$7E
  DEFB $6C,$AA,$AA,$7F,$00,$00,$00,$81

; Routine at 2677
;
; Used by the routine at NMI_HANDLER.
__PLAY:
  CALL PLYHK
  LD (IX+$01),$00
  CALL EVAL
  PUSH HL
  CALL GSTRCU
  CALL LOADFP
  INC E

; play a melody
;
; Used by the routine at _MUSIC.
MUSIC:
  CALL ISCNTC
  LD A,(BC)
  INC BC
  DEC E
  JP Z,MUSIC_2
  CP $2C
  JR Z,MUSIC
  CP $7C
  JR Z,MUSIC
  CP $20
  JR Z,MUSIC
  CP $41
  JP C,SN_ERR
  RES 5,A
  CP $4F
  JP Z,MUSIC_3
  CP $52
  JP Z,MUSIC_6
  CP $54
  JP Z,MUSIC_4
  CP $48
  JP NC,SN_ERR
  SUB $41
  ADD A,A
  INC A
  LD D,A
  LD A,(BC)
  CP $2B
  JR NZ,MUSIC_0
  INC BC
  INC D
  DEC E
MUSIC_0:
  CP $2D
  JR NZ,MUSIC_1
  INC BC
  DEC D
  DEC E
MUSIC_1:
  LD A,(OCTSAV)
  ADD A,D
  LD D,A
  CALL MUSIC_9
  PUSH HL
  LD A,D
  EXX
  LD E,A
  LD D,$00
  LD HL,$0BBF
  ADD HL,DE
  ADD HL,DE
  ADD HL,DE
  LD C,(HL)
  INC HL
  LD B,(HL)
  LD ($480E),BC
  INC HL
  LD A,(HL)
  POP DE
  CALL SOUND_3
  OR A
  RR D
  RR E
  OR A
  RR D
  RR E
  OR A
  RR D
  RR E
  LD A,(TMPSAV)
  CALL SOUND_3
  OR A
  RR D
  RR E
  OR A
  RR D
  RR E
  CALL SOUND
  JP MUSIC
MUSIC_2:
  POP HL
  LD (IX+$00),$03
  LD (IX+$01),$01
  RET
MUSIC_3:
  LD A,(BC)
  INC BC
  DEC E
  JP Z,OPERAND_ERR
  CP $31
  JP C,SN_ERR
  CP $35
  JP NC,SN_ERR
  SUB $31
  ADD A,A
  LD D,A
  ADD A,A
  ADD A,A
  ADD A,A
  SUB D
  LD (OCTSAV),A
  JP MUSIC
MUSIC_4:
  LD A,(BC)
  SUB $30
  JP C,SN_ERR
  CP $0A
  JP NC,SN_ERR
  LD L,A
  INC BC
  DEC E
  JR Z,MUSIC_5
  LD A,(BC)
  SUB $30
  JR C,MUSIC_5
  CP $0A
  JR NC,MUSIC_5
  LD H,A
  LD A,L
  ADD A,A
  LD L,A
  ADD A,A
  ADD A,A
  ADD A,L
  ADD A,H
  LD L,A
  INC BC
  DEC E
  JR Z,MUSIC_5
  LD A,(BC)
  SUB $30
  JR C,MUSIC_5
  CP $0A
  JR NC,MUSIC_5
  LD H,A
  LD A,L
  ADD A,A
  LD L,A
  ADD A,A
  ADD A,A
  ADD A,L
  ADD A,H
  LD L,A
  INC BC
  DEC E
MUSIC_5:
  LD A,L
  OR A
  RRA
  OR A
  RRA
  INC A
  LD (TMPSAV),A
  JP MUSIC
MUSIC_6:
  CALL MUSIC_9
  PUSH HL
  EXX
  POP DE
  LD A,(TMPSAV)
  CALL SOUND_3
MUSIC_7:
  DEC DE
  LD B,$C0
MUSIC_8:
  NOP
  DJNZ MUSIC_8
  LD A,D
  OR E
  JR NZ,MUSIC_7
  EXX
  JP MUSIC
MUSIC_9:
  LD HL,CHRGTB
  LD A,(BC)
  SUB $30
  RET C
  CP $0A
  RET NC
  LD L,A
  LD H,$00
  INC BC
  DEC E
  RET Z
  LD A,(BC)
  SUB $30
  RET C
  CP $0A
  RET NC
  PUSH DE
  ADD HL,HL
  LD D,H
  LD E,L
  ADD HL,HL
  ADD HL,HL
  ADD HL,DE
  LD E,A
  LD D,$00
  ADD HL,DE
  POP DE
  INC BC
  DEC E
  RET
  CALL Z,$08CC
  JP Z,$09CA
  CP (HL)
  CP (HL)
  ADD HL,BC
  OR E
  OR E
  LD A,(BC)
  OR E
  OR E
  LD A,(BC)
  XOR C
  XOR C
  LD A,(BC)
  SBC A,A
  SBC A,A
  DEC BC
  SUB A
  SUB A
  INC C
  ADC A,(HL)
  ADC A,(HL)
  INC C
  ADD A,(HL)
  ADD A,(HL)
  DEC C
  ADD A,(HL)
  ADD A,(HL)
  DEC C
  LD A,A
  LD A,A
  LD C,$77
  LD (HL),A
  RRCA
  LD (HL),C
  LD (HL),C
  DJNZ MUSIC_12
  LD L,D
  LD DE,$6464
  LD (DE),A
  LD E,(HL)
  LD E,(HL)
  INC DE
  LD E,C
  LD E,C
  INC D
  LD E,C
  LD E,C
  INC D
  LD D,H
  LD D,H
  DEC D
  LD C,A
  LD C,A
  RLA
  LD C,E
  LD C,E
  JR MUSIC_10
  LD B,A
  ADD HL,DE
  LD B,D
  LD B,E
  DEC DE
  LD B,D
  LD B,E
  DEC DE
  LD A,$3F
  DEC E
  LD A,($1E3B)
  SCF
  JR C,$0C33
  INC (HL)
  DEC (HL)
  LD ($3231),HL
  INC H
  LD L,$2F
  DEC H
  INC L
  INC L
  ADD HL,HL
  INC L
  INC L
  ADD HL,HL
  ADD HL,HL
  LD HL,($272B)
  DAA
  LD L,$25
  DEC H
  JR NC,MUSIC_11
  INC HL
  INC SP
  LD HL,$3621
  LD HL,$3621
  RRA
  RRA
  LD A,($1D1D)
  DEC A
  INC E
  DEC DE
  LD B,C
  LD A,(DE)
  LD A,(DE)
  LD B,L
  ADD HL,DE
  JR L0C7E_1
  RLA
  RLA
  LD C,L
  LD D,$15
MUSIC_10:
  LD D,D
  LD D,$15
  LD D,D
  INC D
  INC D
  LD D,A
MUSIC_11:
  INC DE
  INC DE
  LD E,H
  LD (DE),A
  LD (DE),A
MUSIC_12:
  LD H,C
  LD DE,$6711
  DJNZ __SOUND
  LD L,L
  DJNZ __SOUND_0
  LD L,L
  RRCA
  RRCA
  LD (HL),H
  LD C,$0E
  LD D,D
  DEC C
  DEC C
  ADD A,D
  INC C
  INC C
  ADC A,D

; Routine at 3178
;
; Used by the routine at MUSIC.
__SOUND:
  CALL SONHK
; This entry point is used by the routine at MUSIC.
__SOUND_0:
  CALL GETINT
  OR A
  JR NZ,__SOUND_1
  INC A
__SOUND_1:
  EXX
  LD E,A
  LD D,$00
  CPL
  INC A
  LD C,A
  EXX

; Routine at 3196
L0C7C:
  RST $08

; Data block at 3197
L0C7D:
  DEFB $2C

; Routine at 3198
L0C7E:
  CALL GETINT
  EXX
  CALL SOUND_3
  LD B,$04
L0C7E_0:
  OR A
  RR D
  RR E
; This entry point is used by the routine at MUSIC.
L0C7E_1:
  DJNZ L0C7E_0
  INC DE
  EXX
  LD A,(HL)
  CP $2C
  LD A,$00
  JR NZ,L0C7E_2
  INC HL
  CALL GETINT
L0C7E_2:
  EXX
  PUSH AF
  ADD A,C
  LD ($480E),A
  POP HL
  LD A,C
  SUB H
  LD ($480F),A

; emitting a sound
;
; Used by the routines at _SOUND, MUSIC, T_EDIT and L33CD.
SOUND:
  DI
SOUND_0:
  PUSH HL
  POP HL
  LD A,$00
  LD HL,($480E)
SOUND_1:
  CALL SOUND_6
  DEC L
  JP NZ,SOUND_1
  LD A,$08
  OUT ($AF),A
  DEC DE
  CALL SOUND_7
  CALL SOUND_8
SOUND_2:
  CALL SOUND_6
  DEC H
  JP NZ,SOUND_2
  LD A,$00
  OUT ($AF),A
  LD A,D
  OR E
  JP NZ,SOUND_0
  EI
  EXX
  RET
; This entry point is used by the routines at MUSIC and L0C7E.
SOUND_3:
  LD B,$08
  LD HL,L0000
SOUND_4:
  RRCA
  JR NC,SOUND_5
  ADD HL,DE
SOUND_5:
  OR A
  RL E
  RL D
  DJNZ SOUND_4
  EX DE,HL
  RET
SOUND_6:
  EX (SP),HL
  EX (SP),HL
SOUND_7:
  NOP
SOUND_8:
  RET
  DEC B
  RETI

; Routine at 3309
;
; Used by the routine at JPVEC_FDIVC.
__SETE:
  LD A,(HL)
  INC HL
  LD B,$C0
  CP $54
  JR Z,__SETE_0
  LD B,$40
  CP $47
  JP NZ,SN_ERR
__SETE_0:
  PUSH BC
  LD (IX+$01),$00
  CALL GETINT
  CP $20
  JP C,FC_ERR
  SET 7,A
  LD E,A
  LD D,$22
  EX AF,AF'
  CALL L0D47_3
  POP AF
  PUSH AF
  XOR $80
  LD E,A
  LD D,$21
  CALL L0D47_3
  LD D,$20
  LD E,$03
  CALL L0D47_3
  LD D,$24
  EX AF,AF'
  LD E,A
  EX AF,AF'
  LD A,E
  RRA
  RRA
  AND $1F
  LD E,A
  CALL L0D47_3
  LD D,$25
  EX AF,AF'
  AND $03
  POP BC
  OR B
  LD E,A
  CALL L0D47_3
  LD D,$20
  LD E,$34
  CALL L0D47_3

; Routine at 3395
L0D43:
  RST $08

; Data block at 3396
L0D44:
  DEFB $2C

; Routine at 3397
L0D45:
  RST $08

; Data block at 3398
L0D46:
  DEFB $22

; Routine at 3399
L0D47:
  DEC HL
  LD B,$0A
L0D47_0:
  CALL __CALL_0
  PUSH BC
  LD B,$08
L0D47_1:
  RLCA
  RR C
  DJNZ L0D47_1
  LD A,C
  POP BC
  LD E,A
  LD D,$29
  CALL L0D47_3
  CALL VDP_READY
  LD A,$25
  OUT ($8F),A
  IN A,($CF)
  ADD A,$04
  LD E,A
  LD D,$25
  CALL L0D47_3
  DJNZ L0D47_0
  INC HL
  LD A,(HL)
  CP $22
  JR NZ,L0D47_2
  INC HL
L0D47_2:
  LD (IX+$01),$01
  RET
; This entry point is used by the routine at __SETE.
L0D47_3:
  LD C,$8F
  OUT (C),D
  LD C,$CF
  OUT (C),E
  RET
; This entry point is used by the routines at CHRGTB, SCR_CO and SHIFT_STOP.
L0D47_4:
  LD B,$C0
  BIT 7,A
  JR Z,L0D47_5
  LD B,$40
L0D47_5:
  PUSH BC
  LD (IX+$01),$00
  SET 7,A
  LD E,A
  LD D,$22
  EX AF,AF'
  CALL L0D47_3
  POP AF
  PUSH AF
  XOR $80
  LD E,A
  LD D,$21
  CALL L0D47_3
  LD D,$20
  LD E,$03
  CALL L0D47_3
  LD D,$24
  EX AF,AF'
  LD E,A
  EX AF,AF'
  LD A,E
  RRA
  RRA
  AND $1F
  LD E,A
  CALL L0D47_3
  LD D,$25
  EX AF,AF'
  AND $03
  POP BC
  OR B
  LD E,A
  CALL L0D47_3
  LD D,$20
  LD E,$34
  CALL L0D47_3
  LD B,$0A
L0D47_6:
  LD A,(HL)
  INC HL
  PUSH BC
  LD B,$08
L0D47_7:
  RLCA
  RR C
  DJNZ L0D47_7
  LD A,C
  POP BC
  LD E,A
  LD D,$29
  CALL L0D47_3
  CALL VDP_READY
  LD A,$25
  OUT ($8F),A
  IN A,($CF)
  ADD A,$04
  LD E,A
  LD D,$25
  CALL L0D47_3
  DJNZ L0D47_6
  LD (IX+$01),$01
  RET

; Routine at 3576
__TX:
  LD A,$00
  LD (IX+$0A),A
; This entry point is used by the routines at __GR, __ET and __EG.
__TX_0:
  PUSH AF
  OR $07
  LD (IX+$08),A
  POP AF
  RET Z
  CALL GETINT
  PUSH AF
  AND $07
  LD D,A
  LD A,(IX+$08)
  AND $F8
  OR D
  LD (IX+$08),A
  POP AF
  RET Z

; Routine at 3607
L0E17:
  RST $08

; Data block at 3608
L0E18:
  DEFB $2C

; Routine at 3609
L0E19:
  CALL GETINT
  PUSH AF
  AND $07
  ADD A,A
  ADD A,A
  ADD A,A
  ADD A,A
  OR (IX+$08)
  LD (IX+$08),A
  POP AF
  RET Z

; Routine at 3627
L0E2B:
  RST $08

; Data block at 3628
L0E2C:
  DEFB $2C

; Routine at 3629
L0E2D:
  CALL GETINT
  OR A
  RET Z
  SET 3,(IX+$08)
  RET

; Routine at 3639
__GR:
  LD (IX+$0A),$00
  LD A,$80
  JP __TX_0

; Routine at 3648
__ET:
  LD (IX+$0A),$80
  LD A,$00
  JP __TX_0

; Routine at 3657
__EG:
  LD A,$80
  LD (IX+$0A),A
  JP __TX_0

; Routine at 3665
__INIT:
  LD (IX+$08),$00
  PUSH HL
  LD A,$E6
  JR Z,L0E63_1
  POP HL
  CALL GETINT
  JR Z,L0E63_0
  PUSH AF

; Routine at 3681
L0E61:
  RST $08

; Data block at 3682
L0E62:
  DEFB $2C

; Routine at 3683
L0E63:
  CALL GETINT
  AND $07
  LD B,A
  LD A,(IX+$03)
  AND $F8
  OR B
  LD (IX+$03),A
  POP AF
; This entry point is used by the routine at __INIT.
L0E63_0:
  PUSH HL
  AND $07
  LD L,A
  ADD A,A
  ADD A,A
  ADD A,A
  ADD A,A
  ADD A,L
  SET 7,A
; This entry point is used by the routine at __INIT.
L0E63_1:
  CALL SCR_CO_2
  POP HL
  RET

; Routine at 3715
__CURSOR:
  LD A,(HL)
  INC HL
  CP $59
  JR Z,__CURSOR_1
  CP $58
  JP NZ,SN_ERR
  CALL GETINT
  CP $28
  JR C,__CURSOR_0
  LD A,$27
__CURSOR_0:
  LD (CURPOS),A
  RET
__CURSOR_1:
  CALL GETINT
  CP $19
  JR C,__CURSOR_2
  LD A,$18
__CURSOR_2:
  LD (YCURSO),A
  RET

; Routine at 3752
__SCROLL:
  LD A,(PICFLG)
  RES 7,A
  LD (PICFLG),A
  RET

; Routine at 3761
__PAGE:
  LD A,(PICFLG)
  SET 7,A
  LD (PICFLG),A
  RET

; Routine at 3770
__STICKX:
  CALL MAKINT
  CALL JOY_L_R
  JP USR_RET

; scan joysticks (right/left)
;
; Used by the routines at _JOY_L_R and __STICKX.
JOY_L_R:
  LD B,$00
  AND $03
  JR Z,JOY_L_R_1
  DEC A
  JR Z,JOY_L_R_3
  IN A,($80)
  BIT 3,A
  JR NZ,JOY_L_R_0
  DEC B
JOY_L_R_0:
  BIT 4,A
  JR JOY_L_R_5
JOY_L_R_1:
  IN A,($07)
  BIT 3,A
  JR NZ,JOY_L_R_2
  DEC B
JOY_L_R_2:
  BIT 1,A
  JR JOY_L_R_5
JOY_L_R_3:
  IN A,($08)
  BIT 3,A
  JR NZ,JOY_L_R_4
  DEC B
JOY_L_R_4:
  BIT 1,A
; This entry point is used by the routines at JOY_UP_DOWN and JOY_FIRE_F2.
JOY_L_R_5:
  JR NZ,JOY_L_R_6
  INC B
JOY_L_R_6:
  LD A,B
  RET

; Routine at 3824
__STICKY:
  CALL MAKINT
  CALL JOY_UP_DOWN
  JP USR_RET

; scan joysticks (up/down)
;
; Used by the routines at _JOY_UP_DOWN and __STICKY.
JOY_UP_DOWN:
  LD B,$00
  AND $03
  JR Z,JOY_UP_DOWN_1
  DEC A
  JR Z,JOY_UP_DOWN_3
  IN A,($81)
  BIT 6,A
  JR NZ,JOY_UP_DOWN_0
  DEC B
JOY_UP_DOWN_0:
  IN A,($80)
  BIT 5,A
  JR JOY_L_R_5
JOY_UP_DOWN_1:
  IN A,($07)
  BIT 0,A
  JR NZ,JOY_UP_DOWN_2
  DEC B
JOY_UP_DOWN_2:
  BIT 2,A
  JR JOY_L_R_5
JOY_UP_DOWN_3:
  IN A,($08)
  BIT 0,A
  JR NZ,JOY_UP_DOWN_4
  DEC B
JOY_UP_DOWN_4:
  BIT 2,A
  JR JOY_L_R_5

; Routine at 3877
__ACTION:
  CALL MAKINT
  CALL JOY_FIRE_F2
  JP USR_RET

; scan joysticks (action1/action2)
;
; Used by the routines at _JOY_FIRE_F2 and __ACTION.
JOY_FIRE_F2:
  LD B,$00
  AND $03
  JR Z,JOY_FIRE_F2_0
  DEC A
  JP Z,L1CC0_61
  IN A,($81)
  BIT 2,A
  JR JOY_L_R_5
JOY_FIRE_F2_0:
  IN A,($07)
  CPL
  AND $30
  RRCA
  RRCA
  RRCA
  RRCA
  LD B,A
  RET

; Routine at 3913
__KEY:
  PUSH HL
  CALL KBDSCAN
  POP HL
  OR A
  JP USR_RET

; Routine at 3922
__DELIM:
  LD A,(IX+$08)
  PUSH AF
  LD A,$00
  PUSH AF
  CALL GETINT
  AND $07
  POP DE
  OR D
  PUSH AF

; Routine at 3937
L0F61:
  RST $08

; Data block at 3938
L0F62:
  DEFB $2C

; Routine at 3939
L0F63:
  CALL GETINT
  AND $07
  ADD A,A
  ADD A,A
  ADD A,A
  ADD A,A
  POP DE
  OR D
  SET 7,A
  LD (IX+$08),A

; Routine at 3955
L0F73:
  RST $08

; Data block at 3956
L0F74:
  DEFB $2C

; Routine at 3957
L0F75:
  CALL GETINT
  BIT 0,A
  LD A,$80
  JR Z,L0F75_0
  LD A,$84
L0F75_0:
  PUSH HL
  CALL GETCHAR_2
  POP HL
  POP AF
  LD (IX+$08),A
  LD (IX+$01),$01
  RET

; Routine at 3982
;
; Used by the routine at __DISPLAY.
__SCREEN:
  LD (IX+$01),$01
  LD (IX+$00),$01
  HALT
  RET

; Routine at 3992
__DISPLAY:
  LD A,$20
  JR Z,__DISPLAY_0
  CALL GETINT
__DISPLAY_0:
  LD (IX+$02),A
  JR __SCREEN

; Routine at 4004
__STORE:
  LD (IX+$02),$00
  LD (IX+$00),$00
  RET

; Routine at 4013
__CALL:
  CALL GETPARM_2BYTE
  CALL CONIS
  LD A,$C3
  LD (CALHK),A
  LD ($47D4),DE
  JP CALHK
; This entry point is used by the routine at L0D47.
__CALL_0:
  CALL __CALL_1
  JP C,SN_ERR
  ADD A,A
  ADD A,A
  ADD A,A
  ADD A,A
  LD C,A
  CALL __CALL_1
  JP C,SN_ERR
  OR C
  RET
; This entry point is used by the routine at L302F.
__CALL_1:
  INC HL
  LD A,(HL)
  CP $30
  JR C,__CALL_3
  CP $3A
  JR C,__CALL_2
  RES 5,A
  CP $41
  JR C,__CALL_3
  CP $47
  JP NC,__CALL_3
  ADD A,$C9
__CALL_2:
  AND $0F
  RET
__CALL_3:
  SCF
  RET

; Data block at 4078
L0FEE:
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF

; Routine at 4096
;
; Used by the routines at L0000 and ERROR.
_STARTUP:
  DI
  LD HL,L0000
  LD DE,SCREEN
  LD BC,$C000
  LDIR
  LD B,H
  LD C,L
  LD HL,SCREEN
_STARTUP_0:
  LD A,(DE)
  CPI
  INC DE
  JR NZ,$101B
  JP PE,_STARTUP_0
  LD A,$2B
  LD SP,HL
  LD (STKTOP),HL
  LD A,$C9
  LD HL,INT_SUB
  LD B,$1E
_STARTUP_1:
  LD (HL),A
  INC HL
  DJNZ _STARTUP_1
  LD A,$C3
  LD (__LPEN),A
  LD (__DISK),A
  LD (__MODEM),A
  LD HL,$2250
  LD ($47F2),HL
  LD ($47F5),HL
  LD ($47F8),HL
  LD HL,NMI_BOOT
  LD (NMIADDR),HL
  LD (NMIHK),A
  LD IX,INTDIV
  LD HL,VDP_TXT
  CALL SET_VDP_REGS
  LD (IX+$00),$05
  LD (IX+$01),$00
  LD (IX+$04),$01
  IM 1
  EI
  LD HL,$0001
  LD ($4809),HL
  LD (IX+$03),$6E
  CALL SCR_CO
  LD HL,JPVEC_RAMLOW
  LD BC,$0065
  LD DE,RAMLOW
  LDIR
  XOR A
  OUT ($EF),A
  OUT ($AF),A
  LD HL,NMI_HANDLER
  LD (NMIADDR),HL
  CALL L2EC4_3
  CALL OUTDO_CRLF
  LD (ENDBUF),A
  LD ($49FB),A
  LD HL,$FFFE
  ADD HL,SP
  LD DE,$03E8
  RST $20
  JP C,OM_ERR
  LD DE,$FFCE
  LD (MEMSIZ),HL
  ADD HL,DE
  LD (STKTOP),HL
  CALL L2EC4_0
  LD HL,(STKTOP)
  LD DE,$FFEF
  ADD HL,DE
  LD DE,$49FB
  LD A,L
  SUB E
  LD L,A
  LD A,H
  SBC A,D
  LD H,A
  PUSH HL
  LD (IX+$08),$30
  LD HL,STARTUP_MSG
  CALL PRS
  CALL OUTDO_CRLF
  LD HL,STARTUP_MSG
  CALL PRS
  LD (IX+$08),$00
  LD HL,VER_MSG
  CALL PRS
  LD HL,$0004
  CALL PRS
  CALL OUTDO_CRLF
  POP HL
  CALL _ASCTFP_12
  LD HL,AVAIL_MSG_FR
  LD A,(FRGFLG)
  OR A
  JR Z,_STARTUP_2
  LD HL,AVAIL_MSG
_STARTUP_2:
  CALL PRS
  LD DE,$4241
  LD HL,NMI_HANDLER_0
  PUSH HL
  LD HL,$6000
  LD BC,FNCTAB
_STARTUP_3:
  XOR A
  ADC HL,BC
  LD A,H
  OR L
  RET Z
  PUSH HL
  LD A,(HL)
  INC HL
  LD H,(HL)
  LD L,A
  RST $20
  POP HL
  JR NZ,_STARTUP_3
  INC HL
  INC HL
  LD A,(HL)
  INC HL
  LD H,(HL)
  LD L,A
  JP (HL)

; NMI handler at runtime
NMI_HANDLER:
  PUSH AF
  IN A,($80)
  AND $40
  JR Z,WARM_BOOT
  POP AF
  RETN
; This entry point is used by the routine at VSIGN.
WARM_BOOT:
  LD SP,ENDBUF
  XOR A
  OUT ($AF),A
  LD (GETFLG),A
  INC A
  OUT ($10),A
  EI
  LD HL,$0001
  LD ($4809),HL
  LD (IX+$03),$6E
  CALL SCR_CO
  CALL L2EC4_3
; This entry point is used by the routine at JPVEC_RAMLOW.
NMI_HANDLER_0:
  LD HL,STARTUP_SOUND
  CALL __PLAY
  JP READY

; Message at 4424
STARTUP_MSG:
  DEFM "VVGG  55000000  BBAASSIICC"
  DEFB $00

; Message at 4451
VER_MSG:
  DEFM " version "
  DEFB $00

; Data block at 4461
STARTUP_SOUND:
  DEFB $22,$54,$38,$4F,$34,$43,$4F,$33
  DEFB $47,$45,$43,$22

; Data block at 4473
VDP_TXT:
  DEFB $1A

; Data block at 4474
L117A:
  DEFB $28,$81,$29,$00,$20,$82,$29,$6E
  DEFB $20,$83,$29,$F7,$20,$84,$29,$13
  DEFB $20,$87,$29,$08,$26,$08,$27,$00
  DEFB $28,$02

; This ram area will be relocated to RAMLOW
JPVEC_RAMLOW:
  JP NMI_HANDLER_0
JPVEC_USR:
  JP FC_ERR

; Routine at 4506
JPVEC_FDIVC:
  SUB $00
  LD L,A
  LD A,H
  SBC A,$00
  LD H,A
  LD A,B
  SBC A,$00
  LD B,A
  LD A,$00
  RET
  NOP
  NOP
  NOP
  DEC (HL)
  LD C,D
  JP Z,SCPTLP_0
  INC E
  HALT
  SBC A,B
  LD ($B395),HL
  SBC A,B
  LD A,(BC)
  DEFB $DD
  LD B,A
  SBC A,B
  LD D,E
  POP DE
  SBC A,C
  SBC A,C
  LD A,(BC)
  LD A,(DE)
  SBC A,A
  SBC A,B
  LD H,L
  CP H
  CALL $D698
  LD (HL),A
  LD A,$98
  LD D,D
  RST $00
  LD C,A
  ADD A,B
  IN A,($00)
  RET
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  OUT ($11),A
  LD B,L
  JR JPVEC_FDIVC_0
  JR $11F3
  DEC DE
  INC D
  INC DE
  LD A,(DE)
  ADD HL,DE
  LD D,$15
  NOP
  LD A,(BC)
  NOP
  LD A,(BC)
  NOP
  NOP
  JR Z,$120B
  CP $FF
  CALL M,$0049
  LD SP,HL
JPVEC_FDIVC_0:
  LD DE,L1BA4_11
  INC C
  JP P,$7377
  CPL
  NOP
  DEC L
  DEC A
  NOP
  LD (HL),C
  LD A,B
  LD H,L
  LD HL,($2B30)
JPVEC_FDIVC_1:
  LD (BC),A
  NOP
  JR NZ,JPVEC_FDIVC_2
  INC SP
  LD H,(HL)
  LD E,L
  LD (HL),D
  LD L,(HL)
  EX AF,AF'
  LD BC,$3476
  LD H,A
  DJNZ JPVEC_FDIVC_3
  LD L,D
  RLCA
  NOP
  LD H,D
  DEC (HL)
  LD (HL),L
  INC L
  LD A,C
  LD L,B
  LD A,(BC)
  DEC C
  LD SP,$6936
  SCF
  INC A
  LD L,E
  NOP
  ADD HL,BC
  LD A,($6F32)
  JR C,JPVEC_FDIVC_1
  LD L,H
  DEC B
  LD H,C
  LD A,D
  DEC SP
  LD (HL),B
  ADD HL,SP
  LD H,H
  LD L,L
  RRA
  RST $38
  LD D,A
  LD D,E
  LD E,A
  NOP
  CCF
  LD E,(HL)
  NOP
  LD D,C
  LD E,B
  LD B,L
  LD A,H
  ADD HL,HL
  LD L,$04
  NOP
  JR NZ,JPVEC_FDIVC_4
  LD ($5B46),HL
  LD D,D
  LD C,(HL)
  EX AF,AF'
  LD BC,$1C56
  LD B,A
  RST $10
  LD D,H
  LD C,D
  RLCA
  NOP
  LD B,D
  INC H
  LD D,L
  CPL
  LD E,C
  LD C,B
  LD A,(BC)
  DEC C
  INC HL
  DEC H
  LD C,C
  LD H,$3E
  LD C,E
  NOP
  ADD HL,BC
  LD HL,($4F21)
  DAA
JPVEC_FDIVC_2:
  RET C
  LD C,H
  LD B,$41
  LD E,D
  LD B,B
  LD D,B
  JR Z,$12BC
  LD C,L
  RRA
  RST $38
  RST $30
  DI
  LD E,A
  NOP
  CCF
  LD E,(HL)
  NOP
  POP AF
  RET M
  PUSH HL
  DEC SP
  OR B
  LD L,$04
  NOP
  JR NZ,JPVEC_FDIVC_2
JPVEC_FDIVC_3:
  OR E
  AND $DB
JPVEC_FDIVC_4:
  JP P,$08EE
  LD BC,$B4F6
  RST $20
  CALL C,$EAF4
  RLCA
  NOP
  JP PO,$F5B5
  SBC A,$F9
  RET PE
  LD A,(BC)
  DEC C
  OR C
  OR (HL)
  JP (HL)
  OR A
  RST $18
  EX DE,HL
  NOP
  ADD HL,BC
  EXX
  OR D
  RST $28
  CP B
  RET PO
  CALL PE,$E106
  JP M,$F0DA
  CP C
  CALL PO,__SETE

; keyboard table
KEYTAB:
  DEFB $0C,$F2,$7A,$73,$2F,$00,$2D,$3D
  DEFB $00,$61,$78,$65,$2A,$30,$2B,$02
  DEFB $00,$20,$63,$33,$66,$5D,$72,$6E
  DEFB $08,$6D,$76,$34,$67,$10,$74,$6A
  DEFB $07,$00,$62,$35,$75,$2C,$79,$68
  DEFB $0A,$0D,$31,$36,$69,$37,$3C,$6B
  DEFB $00,$09,$3A,$32,$6F,$38,$D8,$6C
  DEFB $05,$71,$77,$3B,$70,$39,$64,$01
  DEFB $1F,$FF,$5A,$53,$5F,$00,$3F,$5E
  DEFB $00,$41,$58,$45,$7C,$29,$2E,$04
  DEFB $00,$20,$43,$22,$46,$5B,$52,$4E
  DEFB $08,$4D,$56,$1C,$47,$D7,$54,$4A
  DEFB $07,$00,$42,$24,$55,$2F,$59,$48
  DEFB $0A,$0D,$23,$25,$49,$26,$3E,$4B
  DEFB $00,$09,$2A,$21,$4F,$27,$D8,$4C
  DEFB $06,$51,$57,$40,$50,$28,$44,$01
  DEFB $1F,$FF,$FA,$F3,$5F,$00,$3F,$5E
  DEFB $00,$E1,$F8,$E5,$3B,$B0,$2E,$04
  DEFB $00,$20,$E3,$B3,$E6,$DB,$F2,$EE
  DEFB $08,$ED,$F6,$B4,$E7,$DC,$F4,$EA
  DEFB $07,$00,$E2,$B5,$F5,$DE,$F9,$E8
  DEFB $0A,$0D,$B1,$B6,$E9,$B7,$DF,$EB
  DEFB $00,$09,$D9,$B2,$EF,$B8,$E0,$EC
  DEFB $06,$F1,$F7,$DA,$F0,$B9,$E4,$01

; Error messages in French
ERRMSG_FR:
  DEFB $00
  DEFM "NEXT sans FOR"
  DEFB $00
  DEFM "Erreur de syntaxe"
  DEFB $00
  DEFM "RETURN sans GOSUB"
  DEFB $00
  DEFM "DATA "
  DEFB $12
  DEFM "puis"
  DEFB $12
  DEFM "es"
  DEFB $00
  DEFM "Appel de fonction incorrecte"
  DEFB $00
  DEFM "D"
  DEFB $12
  DEFM "passement de capacit"
  DEFB $12
  DEFB $00
OV_MSG_FR:
  DEFM "Sortie de m"
  DEFB $12
  DEFM "moire"
  DEFB $00
  DEFM "Ligne non d"
  DEFB $12
  DEFM "finie"
  DEFB $00
  DEFM "Indice hors des limites"
  DEFB $00
  DEFM "Tableau redimensionn"
  DEFB $12
  DEFB $00
  DEFM "Division par z"
  DEFB $12
  DEFM "ro"
  DEFB $00
  DEFM "Incorrect en direct"
  DEFB $00
  DEFM "Op"
  DEFB $12
  DEFM "rande mal adapt"
  DEFB $12
  DEFB $00
  DEFM "Espace-cha"
  DEFB $11
  DEFM "ne "
  DEFB $12
  DEFM "puis"
  DEFB $12
  DEFB $00
  DEFM "Cha"
  DEFB $11
  DEFM "ne trop longue"
  DEFB $00
  DEFM "Formule cha"
  DEFB $11
  DEFM "ne trop complexe"
  DEFB $00
  DEFM "Impossible de continuer"
  DEFB $00
  DEFM "Fonction utilisateur non d"
  DEFB $12
  DEFM "finie"
  DEFB $00
  DEFM "Op"
  DEFB $12
  DEFM "rande manquant"
  DEFB $00
  DEFM "FOR sans NEXT"
  DEFB $00
  DEFM "Peripherique non connect"
  DEFB $12
  DEFB $00
  DEFM "Non reconnu"
  DEFB $00

; Error messages in English
ERRMSG:
  DEFB $00
  DEFM "NEXT without FOR"
  DEFB $00
  DEFM "Syntax error"
  DEFB $00
  DEFM "RETURN without GOSUB"
  DEFB $00
  DEFM "Out of DATA"
  DEFB $00
  DEFM "Illegal function call"
  DEFB $00
  DEFM "Overflow"
  DEFB $00
OV_MSG:
  DEFM "Out of memory"
  DEFB $00
  DEFM "Undefined line number"
  DEFB $00
  DEFM "Subscript out of range"
  DEFB $00
  DEFM "Duplicate definition"
  DEFB $00
  DEFM "Division by zero"
  DEFB $00
  DEFM "Illegal direct"
  DEFB $00
  DEFM "Type mismatch"
  DEFB $00
  DEFM "Out of string space"
  DEFB $00
  DEFM "String too long"
  DEFB $00
  DEFM "String formula too complex"
  DEFB $00
  DEFM "Cannot continue"
  DEFB $00
  DEFM "Undefined user function"
  DEFB $00
  DEFM "Missing operand"
  DEFB $00
  DEFM "FOR without NEXT"
  DEFB $00
  DEFM "Device not supported"
  DEFB $00
  DEFM "Unrecognized"
  DEFB $00
AVAIL_MSG_FR:
  DEFM " octets disponibles"
  DEFB $03
  DEFB $00
AVAIL_MSG:
  DEFM " bytes free"
  DEFB $03
  DEFB $00
LINE_MSG_FR:
  DEFM "Ligne "
  DEFB $00
LINE_MSG:
  DEFM "Line "
  DEFB $00
LINE_ERR_MSG_FR:
  DEFM " non d"
  DEFB $12
  DEFM "finie"
  DEFB $00
LINE_ERR_MSG:
  DEFM " not defined"
  DEFB $00
EXTRA_MSG_FR:
  DEFM "Derni"
  DEFB $12
  DEFM "re information ignor"
  DEFB $12
  DEFM "e"
  DEFB $03
  DEFB $00
EXTRA_MSG:
  DEFM "Extra ignored"
  DEFB $03
  DEFB $00
REDO_MSG_FR:
  DEFM "Recommencez au d"
  DEFB $12
  DEFM "but"
  DEFB $03
  DEFB $00
REDO_MSG:
  DEFM "Redo from start"
  DEFB $03
  DEFB $00
BREAK_MSG_FR:
  DEFB $0F
  DEFM "Arr"
  DEFB $1B
  DEFM "t"
  DEFB $00
BREAK_MSG:
  DEFB $0F
  DEFM "Break"
  DEFB $00
IN_MSG:
  DEFM " en "
  DEFB $00
IN_MSG_FR:
  DEFM " in "
  DEFB $00
FOUND_MSG:
  DEFM "Found:"
  DEFB $00
SKIP_MSG:
  DEFM "Skip:"
  DEFB $00
ABORTED_MSG:
  DEFM "ABORTED - reposition tape"
  DEFB $0E
  DEFB $03
  DEFB $00
BADFILE_MSG:
  DEFM "Bad file"
  DEFB $0E
  DEFB $03
  DEFB $00
MISMATCH_MSG:
  DEFM "File mismatch"
  DEFB $03
  DEFB $00
PRINTER_MSG:
  DEFM "Printer not ready"
  DEFB $03
  DEFB $00
FOUND_MSG_FR:
  DEFM "Trouv"
  DEFB $12
  DEFM ":"
  DEFB $00
SKIP_MSG_FR:
  DEFM "Pass"
  DEFB $12
  DEFM ":"
  DEFB $00
ABORTED_MSG_FR:
  DEFM "ANNULE - repositionner la bande"
  DEFB $0E
  DEFB $03
  DEFB $00
BADFILE_MSG_FR:
  DEFM "Mauvais fichier"
  DEFB $0E
  DEFB $03
  DEFB $00
MISMATCH_MSG_FR:
  DEFM "Fichier non correspondant"
  DEFB $03
  DEFB $00
PRINTER_MSG_FR:
  DEFM "Imprimante pas pr"
  DEFB $1B
  DEFM "te"
  DEFB $03
  DEFB $00
  DEFB $00
  DEFB $01
  DEFB $02
  DEFB $03
  DEFB $04
  DEFB $05
  DEFB $06
  DEFB $07
  DEFB $08
  DEFB $09
  DEFB $0A
  DEFB $0B
  DEFB $0C
  DEFB $0D
  DEFB $0E
  DEFB $0F
  DEFB $10
  DEFB $8C
  DEFB $82
  DEFB $97
  DEFB $8B
  DEFB $87
  DEFB $96
  DEFB $85
  DEFB $83
  DEFB $8A
  DEFB $93
  DEFB $88
  DEFB $9C
  DEFB $AB
  DEFB $1E
  DEFB $1F
  DEFM " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]"
  DEFB $5E
  DEFM "_"
  DEFB $60
  DEFM "abcdefghijklmnopqrstuvwxyz{|}~"
  DEFB $7F
  DEFB $80
  DEFB $81
  DEFB $82
  DEFB $83
  DEFB $84
  DEFB $85
  DEFB $86
  DEFB $87
  DEFB $88
  DEFB $89
  DEFB $8A
  DEFB $8B
  DEFB $8C
  DEFB $8D
  DEFB $8E
  DEFB $8F
  DEFB $90
  DEFB $91
  DEFB $92
  DEFB $93
  DEFB $94
  DEFB $95
  DEFB $96
  DEFB $97
  DEFB $98
  DEFB $99
  DEFB $9A
  DEFB $9B
  DEFB $9C
  DEFB $9D
  DEFB $9E
  DEFB $9F
  DEFB $A0
  DEFB $A1
  DEFB $A2
  DEFB $A3
  DEFB $A4
  DEFB $A5
  DEFB $A6
  DEFB $A7
  DEFB $A8
  DEFB $A9
  DEFB $AA
  DEFB $AB
  DEFB $AC
  DEFB $AD
  DEFB $AE
  DEFB $AF
  DEFB $B0
  DEFB $B1
  DEFB $B2
  DEFB $B3
  DEFB $B4
  DEFB $B5
  DEFB $B6
  DEFB $B7
  DEFB $B8
  DEFB $B9
  DEFB $BA
  DEFB $BB
  DEFB $BC
  DEFB $BD
  DEFB $BE
  DEFB $BF
  DEFB $C0
  DEFB $C1
  DEFB $C2
  DEFB $C3
  DEFB $C4
  DEFB $C5
  DEFB $C6
  DEFB $C7
  DEFB $C8
  DEFB $C9
  DEFB $CA
  DEFB $CB
  DEFB $CC
  DEFB $CD
  DEFB $CE
  DEFB $CF
  DEFB $D0
  DEFB $D1
  DEFB $D2
  DEFB $D3
  DEFB $D4
  DEFB $D5
  DEFB $D6
  DEFB $D7
  DEFB $D8
  DEFB $D9
  DEFB $DA
  DEFB $DB
  DEFB $DC
  DEFB $DD
  DEFB $DE
  DEFB $DF
  DEFB $E0
  DEFB $E1
  DEFB $E2
  DEFB $E3
  DEFB $E4
  DEFB $E5
  DEFB $E6
  DEFB $E7
  DEFB $E8
  DEFB $E9
  DEFB $EA
  DEFB $EB
  DEFB $EC
  DEFB $ED
  DEFB $EE
  DEFB $EF
  DEFB $F0
  DEFB $F1
  DEFB $F2
  DEFB $F3
  DEFB $F4
  DEFB $F5
  DEFB $F6
  DEFB $F7
  DEFB $F8
  DEFB $F9
  DEFB $FA
  DEFB $FB
  DEFB $FC
  DEFB $FD
  DEFB $FE
  DEFB $FF

; Routine at 6469
__SAVE:
  CALL __CLOAD_24
  CALL __LIST_0
  PUSH HL
  PUSH DE
  CALL L1CC0_37
  POP DE
  LD HL,L0000
  LD (RETADR),HL
  LD A,$FF
  LD (PRTFLG),A
  LD A,$82
  LD HL,(VARTAB)
  LD (HL),A
  INC HL
  LD ($4820),HL
  LD ($4824),A
  CALL __LIST_4
  LD A,$83
  CALL L1CC0_19
  LD A,($4824)
  CALL L1CC0_22
  CALL $1A1D
  POP HL
  JP L2EC4_0

; Routine at 6526
__LOAD:
  CALL __LIST_0
  LD A,(CASCOM)
  AND $0F
  LD (CASCOM),A
  CALL L1CC0_59
  LD E,$05
  CALL L1CC0_60
  CALL INLPNM_20
  JR C,__LOAD_0
  CALL L1CC0_57
  JR C,__LOAD_0
  CP $82
  SCF
  JR NZ,__LOAD_0
  LD ($4826),A
  CALL L1CC0_24
__LOAD_0:
  JP C,__CLOAD_10
  PUSH AF
  CALL L1CC0_57
  POP AF
  CALL L1CC0_58
  JR Z,__LOAD_2
  LD HL,OV_MSG_FR
  LD A,(FRGFLG)
  OR A
  JR Z,__LOAD_1
  LD HL,OV_MSG
__LOAD_1:
  CALL PRS
  CALL OUTDO_CRLF
__LOAD_2:
  LD A,(CASCOM)
  AND $F0
  JR Z,__LOAD_3
  CALL __CLOAD_14
  CALL PRS
__LOAD_3:
  LD HL,$19E3
  CALL PRS
  XOR A
  LD (AUTFLG),A
  DEC A
  LD (GETFLG),A
  JP L3184_1
  LD D,B
  LD H,C
  LD (HL),L
  LD (HL),E
  LD H,L
  LD L,$2E
  LD L,$03
  NOP

; Routine at 6637
__CSAVE:
  CP $4D
  JR Z,$1A03
  CP $53
  JR Z,$1A03
  CP $BB
  JR Z,$1A03
  CP $58
  JR Z,$1A03
  CP $4C
  JP Z,L1BA4_18
  LD A,($0123)
  LD A,$20
  LD (FILETAB),A
  CALL __CLOAD_24
  CALL L1BA4_2
  CALL L1BA4_19
  PUSH HL
  CALL L1CC0_39
  JR C,__CSAVE_0
  CALL L1CC0_43
  LD A,$E5
__CSAVE_0:
  CALL INLPNM_3
  JR NC,__CSAVE_2
  LD HL,CASCOM
  SET 7,(HL)
  LD HL,ABORTED_MSG_FR
  LD A,(FRGFLG)
  OR A
  JR Z,__CSAVE_1
  LD HL,ABORTED_MSG
__CSAVE_1:
  CALL PRS
__CSAVE_2:
  POP HL
  RET

; Routine at 6713
__CLOAD:
  CP $BB
  LD ($4817),A
  JR NZ,__CLOAD_0
  INC HL
__CLOAD_0:
  LD (FILETAB),A
  CP $41
  JR Z,$1A4D
  SUB $94
  JR Z,$1A50
  LD A,$23
  XOR A
  LD BC,$232F
  CP $01
  PUSH AF
  LD A,(CASCOM)
  AND $0F
  LD (CASCOM),A
  CALL L1BA4_1
  CALL L1CC0_2
  CALL L1CC0_59
__CLOAD_1:
  LD E,$01
  CALL L1CC0_60
  CALL INLPNM_20
  JR C,$1ADB
  CALL L1BA4_5
  JR C,$1ADB
  LD HL,$4819
  CALL L1BA4_8
  JR Z,__CLOAD_5
  LD HL,SKIP_MSG_FR
  LD A,(FRGFLG)
  OR A
  JR Z,__CLOAD_2
  LD HL,SKIP_MSG
__CLOAD_2:
  CALL L1BA4_10
  LD E,$04
  CALL L1CC0_60
  CALL INLPNM_20
  JR C,$1ADB
__CLOAD_3:
  LD B,$0A
__CLOAD_4:
  CALL L1CC0_57
  JR C,$1ADB
  OR A
  JR NZ,__CLOAD_3
  DJNZ __CLOAD_4
  JR __CLOAD_1
__CLOAD_5:
  LD HL,FOUND_MSG_FR
  LD A,(FRGFLG)
  OR A
  JR Z,__CLOAD_6
  LD HL,FOUND_MSG
__CLOAD_6:
  CALL L1BA4_10
  LD E,$02
  CALL L1CC0_60
  LD A,($4818)
  CP $20
  JR NZ,__CLOAD_7
  LD A,(FILETAB)
  CP $41
  JR Z,__CLOAD_7
  POP AF
  LD (FACCU),A
  CALL C,L2EC4_0
  LD A,(FACCU)
  CP $01
  PUSH AF
__CLOAD_7:
  CALL L1CC0_9
  POP AF
  CALL L1CC0_47
  LD A,$D1
  CALL L1CC0_58
  LD HL,CASCOM
  JR C,__CLOAD_10
  JR NZ,__CLOAD_9
  LD A,C
  OR A
  LD A,(HL)
  JR Z,__CLOAD_15
  AND $F0
  JR NZ,__CLOAD_11
; This entry point is used by the routine at L1CC0.
__CLOAD_8:
  LD HL,OK_MSG
  CALL PRS
  JP PROMPT_9
__CLOAD_9:
  SET 4,(HL)
  LD E,$0C
  JP ERROR
; This entry point is used by the routine at __LOAD.
__CLOAD_10:
  SET 7,(HL)
  LD HL,ABORTED_MSG_FR
  LD A,(FRGFLG)
  OR A
  JR Z,__CLOAD_13
  LD HL,ABORTED_MSG
  JR __CLOAD_13
__CLOAD_11:
  CP $20
  JR NZ,__CLOAD_12
  LD HL,MISMATCH_MSG_FR
  LD A,(FRGFLG)
  OR A
  JR Z,__CLOAD_13
  LD HL,MISMATCH_MSG
  JR __CLOAD_13
__CLOAD_12:
  CALL __CLOAD_14
__CLOAD_13:
  CALL PRS
  JP RESTART
; This entry point is used by the routine at __LOAD.
__CLOAD_14:
  LD HL,BADFILE_MSG_FR
  LD A,(FRGFLG)
  OR A
  RET Z
  LD HL,BADFILE_MSG
  RET
__CLOAD_15:
  BIT 3,A
  JR NZ,__CLOAD_16
  AND $F0
  JR NZ,__CLOAD_11
__CLOAD_16:
  LD A,($4818)
  CP $4D
  JR Z,__CLOAD_17
  CP $20
  JR NZ,__CLOAD_23
  LD A,(HL)
  AND $F0
  JR NZ,__CLOAD_11
  CALL L1BA4_14
  LD (VARTAB),HL
__CLOAD_17:
  LD A,($4889)
  OR A
  JR NZ,__CLOAD_18
  LD HL,BUFFER
  LD A,(HL)
  OR A
  JR Z,__CLOAD_18
  CP $30
  JR Z,__CLOAD_19
  JR __CLOAD_22
__CLOAD_18:
  LD DE,$4820
  LD A,(DE)
  OR A
  JR NZ,__CLOAD_20
__CLOAD_19:
  LD A,($4818)
  CP $4D
  JR Z,__CLOAD_23
  JP __CLOAD_8
__CLOAD_20:
  PUSH HL
  LD B,$05
__CLOAD_21:
  LD A,(DE)
  LD (HL),A
  INC DE
  INC HL
  DJNZ __CLOAD_21
  POP HL
__CLOAD_22:
  DEC A
  JP __RUN
__CLOAD_23:
  LD HL,(CONTXT)
  RET
; This entry point is used by the routines at __SAVE and __CSAVE.
__CLOAD_24:
  DEC HL
  RST $10
  RET Z
  CP $28
  RET NZ
  RST $10
  JP Z,SN_ERR
  CP $31
  JR Z,__CLOAD_25
  CP $32
  JP NZ,FC_ERR
__CLOAD_25:
  LD D,A
  RST $10
  JP Z,SN_ERR

; Routine at 7074
L1BA2:
  RST $08

; Data block at 7075
L1BA3:
  DEFB $29

; Routine at 7076
L1BA4:
  PUSH HL
  LD HL,CASCOM
  RES 2,(HL)
  LD A,D
  AND $01
  JR NZ,L1BA4_0
  SET 2,(HL)
L1BA4_0:
  POP HL
  RET
; This entry point is used by the routine at __CLOAD.
L1BA4_1:
  XOR A
  LD ($4811),A
  DEC HL
  RST $10
  RET Z
; This entry point is used by the routine at __CSAVE.
L1BA4_2:
  CALL L1CC0_18
  PUSH HL
  LD B,C
  LD C,$06
  LD HL,$4811
L1BA4_3:
  LD A,(DE)
  LD (HL),A
  INC HL
  INC DE
  DEC C
  JR Z,L1BA4_4
  DJNZ L1BA4_3
  LD (HL),$00
L1BA4_4:
  POP HL
  RET
; This entry point is used by the routine at __CLOAD.
L1BA4_5:
  LD B,$0A
L1BA4_6:
  CALL L1CC0_57
  RET C
  SUB $D3
  JR NZ,L1BA4_5
  DJNZ L1BA4_6
  LD HL,$4818
  LD B,$16
L1BA4_7:
  CALL L1CC0_57
  RET C
  LD (HL),A
  INC HL
  DJNZ L1BA4_7
  RET
; This entry point is used by the routine at __CLOAD.
L1BA4_8:
  LD BC,$4811
  LD E,$06
L1BA4_9:
  LD A,(BC)
  OR A
  RET Z
  LD A,(BC)
  CP (HL)
  INC HL
  INC BC
  RET NZ
  DEC E
  JR NZ,L1BA4_9
  RET
; This entry point is used by the routine at __CLOAD.
L1BA4_10:
  PUSH DE
  PUSH AF
  LD A,(CASCOM)
  BIT 0,A
  JR NZ,L1BA4_13
  CALL PRS
  LD HL,$4819
  LD B,$06
L1BA4_11:
  LD A,(HL)
  OR A
  JR Z,L1BA4_12
  INC HL
  RST $18
  DJNZ L1BA4_11
L1BA4_12:
  CALL OUTDO_CRLF
  LD A,$01
  EI
  LD (IX+$00),A
  HALT
  DI
L1BA4_13:
  POP AF
  POP DE
  RET
; This entry point is used by the routine at __CLOAD.
L1BA4_14:
  LD DE,(TXTTAB)
L1BA4_15:
  LD H,D
  LD L,E
  LD A,(HL)
  INC HL
  OR (HL)
  INC HL
  RET Z
L1BA4_16:
  INC HL
  INC HL
L1BA4_17:
  LD A,(HL)
  INC HL
  CP $0E
  JR Z,L1BA4_16
  OR A
  JR NZ,L1BA4_17
  EX DE,HL
  LD (HL),E
  INC HL
  LD (HL),D
  JR L1BA4_15
; This entry point is used by the routine at __CSAVE.
L1BA4_18:
  INC HL
  LD B,$03
  JP INLPNM_0
; This entry point is used by the routine at __CSAVE.
L1BA4_19:
  PUSH HL
  LD HL,$4817
  XOR A
  LD B,$09
L1BA4_20:
  LD (HL),A
  INC HL
  DJNZ L1BA4_20
  LD A,($4889)
  LD ($481D),A
  LD A,(FILETAB)
  CP $20
  JR NZ,L1BA4_22
  LD HL,(TXTTAB)
  LD ($4820),HL
  EX DE,HL
  LD HL,(VARTAB)
  OR A
  SBC HL,DE
; This entry point is used by the routine at L1C82.
L1BA4_21:
  LD ($4822),HL
  JR L1C82_4
L1BA4_22:
  CP $4D
  JR NZ,L1C82_0
  POP HL

; Routine at 7284
L1C74:
  RST $08

; Data block at 7285
L1C75:
  DEFB $2C

; Routine at 7286
L1C76:
  CALL GETPARM_2BYTE
  CALL CONIS
  LD ($4820),DE

; Routine at 7296
L1C80:
  RST $08

; Data block at 7297
L1C81:
  DEFB $2C

; Routine at 7298
L1C82:
  CALL GETPARM_2BYTE
  CALL CONIS
  PUSH HL
  EX DE,HL
  JR L1BA4_21
; This entry point is used by the routine at L1BA4.
L1C82_0:
  CP $53
  JR NZ,L1C82_2
  LD HL,SCREEN
  LD ($4820),HL
  LD HL,NUMASC_8
L1C82_1:
  LD ($4822),HL
  JR L1CC0_1
L1C82_2:
  POP HL
  CP $BB
  JR NZ,L1C82_3
  CALL L1CC0_17
  PUSH HL
  LD ($4820),BC
  EX DE,HL
  JR L1C82_1
L1C82_3:
  CALL L1CC0_18
  LD ($4820),DE
  LD ($4822),BC
  RET
; This entry point is used by the routine at L1BA4.
L1C82_4:
  POP HL
  DEC HL
  RST $10
  RET Z

; Routine at 7358
L1CBE:
  RST $08

; Data block at 7359
L1CBF:
  DEFB $2C

; Routine at 7360
L1CC0:
  LD DE,$4818
  LD B,$05
L1CC0_0:
  PUSH HL
; This entry point is used by the routine at L1C82.
L1CC0_1:
  POP HL
  RET Z
  JP NC,SN_ERR
  LD (DE),A
  INC DE
  RST $10
  DJNZ L1CC0_0
  RET
; This entry point is used by the routine at __CLOAD.
L1CC0_2:
  XOR A
  LD C,A
  DEC HL
  RST $10
  LD DE,BUFFER
  LD (DE),A
  JR Z,L1CC0_4
  CP $2C
  JR NZ,L1CC0_5
  LD B,$05
L1CC0_3:
  RST $10
  JR Z,L1CC0_4
  JP NC,SN_ERR
  LD (DE),A
  INC DE
  DJNZ L1CC0_3
L1CC0_4:
  LD (CONTXT),HL
  LD A,C
  LD ($4817),A
  RET
L1CC0_5:
  PUSH HL
  INC HL
  LD A,(HL)
  CP $24
  JR Z,L1CC0_6
  INC HL
  LD A,(HL)
  CP $24
L1CC0_6:
  POP HL
  PUSH HL
  PUSH BC
  JR NZ,L1CC0_8
  LD A,($4817)
  CP $BB
  JR Z,L1CC0_8
  CALL L1CC0_18
  POP BC
  DEC C
L1CC0_7:
  POP HL
  JR L1CC0_4
L1CC0_8:
  CALL L1CC0_17
  POP BC
  INC C
  JR L1CC0_7
; This entry point is used by the routine at __CLOAD.
L1CC0_9:
  LD A,($4818)
  CP $20
  JR NZ,L1CC0_11
  LD A,(FILETAB)
  CP $41
  JR NZ,L1CC0_10
  LD HL,(VARTAB)
  DEC HL
  DEC HL
  RET
L1CC0_10:
  LD HL,(TXTTAB)
  RET
L1CC0_11:
  CP $4D
  JR NZ,L1CC0_12
  LD HL,($4828)
  RET
L1CC0_12:
  CP $53
  JR NZ,L1CC0_13
  LD HL,SCREEN
  RET
L1CC0_13:
  LD HL,(CONTXT)
  CP $BB
  JR NZ,L1CC0_15
  LD A,($4817)
  DEC A
  JR NZ,L1CC0_16
  CALL L1CC0_17
  PUSH BC
L1CC0_14:
  LD (CONTXT),HL
  LD HL,($482A)
  RST $20
  POP HL
  RET C
  RET Z
  LD HL,(CONTXT)
  CALL L1CC0_58
  JP OV_ERR
L1CC0_15:
  LD A,($4817)
  INC A
  JR NZ,L1CC0_16
  CALL L1CC0_18
  PUSH DE
  LD D,B
  LD E,C
  JR L1CC0_14
L1CC0_16:
  CALL L1CC0_58
  JP TM_ERR
; This entry point is used by the routine at L1C82.
L1CC0_17:
  LD A,$01
  LD (SUBFLG),A
  CALL GETVAR
  JP NZ,FC_ERR
  LD (SUBFLG),A
  RET
; This entry point is used by the routines at L1BA4 and L1C82.
L1CC0_18:
  CALL EVAL
  PUSH HL
  CALL __ASC_0
  DEC HL
  DEC HL
  DEC HL
  LD B,$00
  LD C,(HL)
  POP HL
  RET
; This entry point is used by the routines at __SAVE and INLPNM.
L1CC0_19:
  OR $80
  PUSH HL
  LD HL,($4820)
  LD (HL),A
  INC HL
  CP $83
  JR Z,L1CC0_21
  XOR A
  LD (PRTFLG),A
  CALL L2EC4
  LD A,$FF
  LD (PRTFLG),A
  PUSH DE
  LD DE,(RETADR)
  LD A,D
  OR E
  JR Z,L1CC0_20
  XOR A
  SBC HL,DE
  PUSH HL
  LD HL,(TXTTAB)
  LD (RETADR),HL
  EX DE,HL
  POP BC
  LDIR
  PUSH DE
  SBC HL,DE
  EX DE,HL
  LD HL,(VARTAB)
  SBC HL,DE
  LD (VARTAB),HL
  POP HL
L1CC0_20:
  POP DE
L1CC0_21:
  LD ($4820),HL
  DEC HL
  LD A,(HL)
  LD HL,$4824
  XOR (HL)
  LD (HL),A
  POP HL
  RET
; This entry point is used by the routine at __SAVE.
L1CC0_22:
  LD HL,($4820)
  LD (HL),A
  INC HL
  LD DE,(VARTAB)
  XOR A
  SBC HL,DE
  PUSH HL
  DEC A
  CALL INLPNM_6
  POP BC
  RET C
L1CC0_23:
  LD A,(DE)
  INC DE
  DEC BC
  CALL L1CC0_55
  RET C
  LD A,B
  OR C
  JR NZ,L1CC0_23
  DEC A
  JP INLPNM_8
; This entry point is used by the routine at __LOAD.
L1CC0_24:
  LD HL,(VARTAB)
  LD BC,$0014
  ADD HL,BC
  LD ($4828),HL
  LD D,H
  LD E,L
  INC DE
  LD C,$00
L1CC0_25:
  AND $7F
  LD (HL),A
  CP $03
  JR Z,L1CC0_34
  LD A,$02
  XOR (IX+$03)
  LD (IX+$03),A
  OUT ($CF),A
  CALL L1CC0_53
  JR NC,L1CC0_33
  INC HL
  CALL L1CC0_57
  RET C
  PUSH AF
  CP $8D
  JR NZ,L1CC0_27
  PUSH HL
  PUSH DE
  EX DE,HL
  CALL FC_ERR_0
  LD B,D
  LD C,E
  LD A,$8D
  POP DE
L1CC0_26:
  LD HL,$4826
  XOR (HL)
  LD (HL),A
  POP HL
  POP AF
  JR L1CC0_25
L1CC0_27:
  LD A,B
  OR C
  JR Z,L1CC0_30
  PUSH HL
  XOR A
  LD HL,($480E)
  SBC HL,BC
  JR C,L1CC0_28
  LD HL,(TEMP)
  SBC HL,BC
  JR C,L1CC0_32
  JR Z,L1CC0_32
L1CC0_28:
  POP HL
L1CC0_29:
  LD H,D
  LD L,E
  LD BC,L0000
L1CC0_30:
  POP AF
L1CC0_31:
  PUSH AF
  PUSH HL
  JR L1CC0_26
L1CC0_32:
  POP HL
  EX DE,HL
  JR L1CC0_29
L1CC0_33:
  CALL L1CC0_57
  JR NC,L1CC0_31
  RET
L1CC0_34:
  CALL L1CC0_57
  RET C
  PUSH HL
  LD HL,$4826
  SUB (HL)
  CALL L1CC0_52
  POP HL
  CALL L1CC0_53
  JR NC,L1CC0_35
  XOR A
  RET
L1CC0_35:
  EX DE,HL
  LD A,$03
  LD (HL),A
  RET
; This entry point is used by the routine at SHIFT_STOP.
L1CC0_36:
  LD DE,($4828)
  INC DE
  LD A,(DE)
  LD ($4828),DE
  CP $03
  RET NZ
  XOR A
  LD (GETFLG),A
  JP __CLOAD_8
  LD E,A
  LD H,A
  DEC HL
  INC SP
  RRCA
  DEC HL
  INC SP
  LD DE,$1F19
; This entry point is used by the routine at __SAVE.
L1CC0_37:
  LD HL,CASCOM
  RES 7,(HL)
  LD A,(HL)
  BIT 1,A
  RET NZ
  LD HL,$1E9D
  LD DE,$4826
  LD BC,$0005
  BIT 2,A
  JR Z,L1CC0_38
  ADD HL,BC
L1CC0_38:
  LDIR
  RET
; This entry point is used by the routine at __CSAVE.
L1CC0_39:
  CALL L1CC0_37
  LD DE,($4820)
  LD BC,($4822)
  XOR A
  LD H,A
  LD L,A
L1CC0_40:
  LD A,(DE)
  ADD A,L
  LD L,A
  LD A,$00
  ADC A,H
  LD H,A
  INC DE
  DEC BC
  LD A,B
  OR C
  JR NZ,L1CC0_40
  LD ($4824),HL
  DEC A
  CALL INLPNM_6
  RET C
  LD B,$0A
L1CC0_41:
  LD A,$D3
  CALL L1CC0_55
  RET C
  DJNZ L1CC0_41
  LD B,$16
  LD HL,FILETAB
L1CC0_42:
  LD A,(HL)
  INC HL
  CALL L1CC0_55
  RET C
  DJNZ L1CC0_42
  RET
; This entry point is used by the routine at __CSAVE.
L1CC0_43:
  XOR A
  CALL INLPNM_6
  RET C
  LD B,$0A
L1CC0_44:
  LD A,$D6
  CALL L1CC0_55
  RET C
  DJNZ L1CC0_44
  LD DE,($4820)
  LD BC,($4822)
L1CC0_45:
  LD A,(DE)
  INC DE
  DEC BC
  CALL L1CC0_55
  RET C
  LD A,B
  OR C
  JR NZ,L1CC0_45
  LD B,$0A
L1CC0_46:
  XOR A
  CALL L1CC0_55
  RET C
  DJNZ L1CC0_46
  RET
; This entry point is used by the routine at __CLOAD.
L1CC0_47:
  PUSH HL
  PUSH AF
  CALL INLPNM_20
  JP C,L1CC0_56
  LD A,($4825)
  LD HL,$4889
  OR (HL)
  LD (HL),A
  POP AF
  POP HL
  LD D,A
L1CC0_48:
  LD B,$0A
L1CC0_49:
  CALL L1CC0_57
  RET C
  SUB $D6
  JR NZ,L1CC0_48
  DJNZ L1CC0_49
  LD BC,($482A)
L1CC0_50:
  CALL L1CC0_57
  RET C
  LD E,A
  PUSH HL
  LD HL,($4826)
  OR A
  ADD A,L
  LD L,A
  LD A,$00
  ADC A,H
  LD H,A
  LD ($4826),HL
  POP HL
  LD A,E
  SUB (HL)
  AND D
  JR Z,$1F6D
  LD A,(CASCOM)
  SET 5,A
  LD (CASCOM),A
  LD A,$73
  LD A,$06
  XOR (IX+$03)
  LD (IX+$03),A
  OUT ($CF),A
  LD A,($4818)
  CP $20
  JR NZ,L1CC0_51
  CALL L1CC0_53
  RET NC
L1CC0_51:
  INC HL
  DEC BC
  LD A,B
  OR C
  JR NZ,L1CC0_50
  LD C,D
  PUSH HL
  LD HL,($4826)
  LD DE,($482C)
  RST $20
  POP HL
L1CC0_52:
  RET Z
  LD HL,CASCOM
  SET 6,(HL)
  XOR A
  RET
L1CC0_53:
  PUSH HL
  LD A,$A0
  SUB L
  LD L,A
  LD A,$FF
  SBC A,H
  LD H,A
  JR C,L1CC0_54
  ADD HL,SP
  POP HL
  RET C
  PUSH HL
L1CC0_54:
  POP HL
  XOR A
  DEC A
  RET
L1CC0_55:
  PUSH HL
  PUSH DE
  PUSH BC
  CALL INLPNM_11
  POP BC
L1CC0_56:
  POP DE
  POP HL
  RET
; This entry point is used by the routines at __LOAD, __CLOAD and L1BA4.
L1CC0_57:
  PUSH HL
  PUSH DE
  PUSH BC
  CALL INLPNM_26
  POP BC
  POP DE
  POP HL
  RET
; This entry point is used by the routines at __LOAD and __CLOAD.
L1CC0_58:
  CALL INLPNM_5
  LD A,(MULVAL)
  LD (IX+$03),A
  RET
; This entry point is used by the routines at __LOAD and __CLOAD.
L1CC0_59:
  LD A,(IX+$03)
  LD (MULVAL),A
  RES 6,(IX+$03)
  LD A,$01
  LD (IX+$00),A
  HALT
  RET
; This entry point is used by the routines at __LOAD and __CLOAD.
L1CC0_60:
  LD A,$28
  OUT ($8F),A
  LD A,$82
  OUT ($CF),A
  LD A,$29
  OUT ($8F),A
  LD A,(IX+$03)
  AND $F8
  OR E
  OUT ($CF),A
  RET
; This entry point is used by the routine at JOY_FIRE_F2.
L1CC0_61:
  IN A,($08)
  CPL
  AND $30
  RRCA
  RRCA
  RRCA
  RRCA
  LD B,A
  RET
  RST $38
  RST $38

; Jump table for statements and functions
FNCTAB:
  DEFW __END
  DEFW __FOR
  DEFW __NEXT
  DEFW __DATA
  DEFW __INPUT
  DEFW __DIM
  DEFW __READ
  DEFW __LET
  DEFW __GOTO
  DEFW __RUN
  DEFW __IF
  DEFW __RESTORE
  DEFW __GOSUB
  DEFW __RETURN
  DEFW __REM
  DEFW __STOP
  DEFW __ON
  DEFW __LPRINT
  DEFW __DEF
  DEFW __POKE
  DEFW __PRINT
  DEFW __CONT
  DEFW __LIST
  DEFW __LLIST
  DEFW __CLEAR
  DEFW __RENUM
  DEFW __AUTO
  DEFW __LOAD
  DEFW __SAVE
  DEFW __CLOAD
  DEFW __CSAVE
  DEFW __CALL
  DEFW __INIT
  DEFW __SOUND
  DEFW __PLAY
  DEFW __TX
  DEFW __GR
  DEFW __SCREEN
  DEFW __DISPLAY
  DEFW __STORE
  DEFW __SCROLL
  DEFW __PAGE
  DEFW __DELIM
  DEFW __SETE
  DEFW __ET
  DEFW __EG
  DEFW __CURSOR
  DEFW __DISK
  DEFW __MODEM
  DEFW __NEW

; Extra jump table for functions
FNCTAB_FN:
  DEFW __SGN
  DEFW __INT
  DEFW __ABS
  DEFW __USR
  DEFW __FRE
  DEFW __LPOS
  DEFW __POS
  DEFW __SQR
  DEFW __RND
  DEFW __LOG
  DEFW __EXP
  DEFW __COS
  DEFW __SIN
  DEFW __TAN
  DEFW __ATN
  DEFW __PEEK
  DEFW __LEN
  DEFW __STR_S
  DEFW __VAL
  DEFW __ASC
  DEFW __STICKX
  DEFW __STICKY
  DEFW __ACTION
  DEFW __KEY
  DEFW __LPEN
  DEFW __CHR_S
  DEFW __LEFT_S
  DEFW __RIGHT_S
  DEFW __MID_S

; BASIC keyword list
WORDS:
  DEFM "8"
  DEFB $C5
  DEFM "ND"
  DEFB $C6
  DEFM "OR"
  DEFB $CE
  DEFM "EXT"
  DEFB $C4
  DEFM "ATA"
  DEFB $C9
  DEFM "NPUT"
  DEFB $C4
  DEFM "IM"
  DEFB $D2
  DEFM "EAD"
  DEFB $CC
  DEFM "ET"
  DEFB $C7
  DEFM "OTO"
  DEFB $D2
  DEFM "UN"
  DEFB $C9
  DEFM "F"
  DEFB $D2
  DEFM "ESTORE"
  DEFB $C7
  DEFM "OSUB"
  DEFB $D2
  DEFM "ETURN"
  DEFB $D2
  DEFM "EM"
  DEFB $D3
  DEFM "TOP"
  DEFB $CF
  DEFM "N"
  DEFB $CC
  DEFM "PRINT"
  DEFB $C4
  DEFM "EF"
  DEFB $D0
  DEFM "OKE"
  DEFB $D0
  DEFM "RINT"
  DEFB $C3
  DEFM "ONT"
  DEFB $CC
  DEFM "IST"
  DEFB $CC
  DEFM "LIST"
  DEFB $C3
  DEFM "LEAR"
  DEFB $D2
  DEFM "ENUM"
  DEFB $C1
  DEFM "UTO"
  DEFB $CC
  DEFM "OAD"
  DEFB $D3
  DEFM "AVE"
  DEFB $C3
  DEFM "LOAD"
  DEFB $C3
  DEFM "SAVE"
  DEFB $C3
  DEFM "ALL"
  DEFB $C9
  DEFM "NIT"
  DEFB $D3
  DEFM "OUND"
  DEFB $D0
  DEFM "LAY"
  DEFB $D4
  DEFM "X"
  DEFB $C7
  DEFM "R"
  DEFB $D3
  DEFM "CREEN"
  DEFB $C4
  DEFM "ISPLAY"
  DEFB $D3
  DEFM "TORE"
  DEFB $D3
  DEFM "CROLL"
  DEFB $D0
  DEFM "AGE"
  DEFB $C4
  DEFM "ELIM"
  DEFB $D3
  DEFM "ETE"
  DEFB $C5
  DEFM "T"
  DEFB $C5
  DEFM "G"
  DEFB $C3
  DEFM "URSOR"
  DEFB $C4
  DEFM "ISK"
  DEFB $CD
  DEFM "ODEM"
  DEFB $CE
  DEFM "EW"
  DEFB $D4
  DEFM "AB("
  DEFB $D4
  DEFM "O"
  DEFB $C6
  DEFM "N"
  DEFB $D3
  DEFM "PC("
  DEFB $D4
  DEFM "HEN"
  DEFB $CE
  DEFM "OT"
  DEFB $D3
  DEFM "TEP"
  DEFB $AB
  DEFB $AD
  DEFB $AA
  DEFB $AF
  DEFB $DE
  DEFB $C1
  DEFM "ND"
  DEFB $CF
  DEFM "R"
  DEFB $BE
  DEFB $BD
  DEFB $BC
  DEFB $D3
  DEFM "GN"
  DEFB $C9
  DEFM "NT"
  DEFB $C1
  DEFM "BS"
  DEFB $D5
  DEFM "SR"
  DEFB $C6
  DEFM "RE"
  DEFB $CC
  DEFM "POS"
  DEFB $D0
  DEFM "OS"
  DEFB $D3
  DEFM "QR"
  DEFB $D2
  DEFM "ND"
  DEFB $CC
  DEFM "OG"
  DEFB $C5
  DEFM "XP"
  DEFB $C3
  DEFM "OS"
  DEFB $D3
  DEFM "IN"
  DEFB $D4
  DEFM "AN"
  DEFB $C1
  DEFM "TN"
  DEFB $D0
  DEFM "EEK"
  DEFB $CC
  DEFM "EN"
  DEFB $D3
  DEFM "TR$"
  DEFB $D6
  DEFM "AL"
  DEFB $C1
  DEFM "SC"
  DEFB $D3
  DEFM "TICKX"
  DEFB $D3
  DEFM "TICKY"
  DEFB $C1
  DEFM "CTION"
  DEFB $CB
  DEFM "EY"
  DEFB $CC
  DEFM "PEN"
  DEFB $C3
  DEFM "HR$"
  DEFB $CC
  DEFM "EFT$"
  DEFB $D2
  DEFM "IGHT$"
  DEFB $CD
  DEFM "ID$"
  DEFB $80

; ARITHMETIC PRECEDENCE TABLE
PRITAB:
  DEFB $79,$05,$07,$79,$0B,$03,$7C,$78
  DEFB $04,$7C,$DC,$04,$7F,$26,$08,$50
  DEFB $78,$29,$46,$77,$29,$00

; Message at 8724
OK_MSG:
  DEFM "Ok!"
  DEFB $03
  DEFB $00

; Routine at 8729
;
; Used by the routines at __RETURN and __NEXT.
BAKSTK:
  LD HL,$0004
  ADD HL,SP
; This entry point is used by the routine at __FOR.
LOKFOR:
  LD A,(HL)
  INC HL
  CP $81
  RET NZ
  LD C,(HL)
  INC HL
  LD B,(HL)
  INC HL
  PUSH HL
  LD H,B
  LD L,C
  RST $20
  LD BC,$000D
  POP HL
  RET Z
  ADD HL,BC
  JR LOKFOR

; 'SN err' entry for Input STMT
;
; Used by the routine at NEXITM.
DATSNR:
  LD HL,(DATLIN)
  LD (CURLIN),HL

; entry for '?Syntax ERROR'
;
; Used by the routines at SYNCHR, MUSIC, __SETE, __CURSOR, __CALL, __CLOAD,
; L1CC0, L24BD, STKTHS, L2A38, L2FB1, L302F, L3077, __AUTO, L3184, __LIST and
; GETVAR.
SN_ERR:
  LD E,$02

; "LD BC,nn" to jump over the next word without executing it
L223A:
  DEFB $01

; "?Division by zero ERROR"
;
; Used by the routines at FDIV and POWER.
O_ERR:
  LD E,$14
  LD BC,$001E

; "LD BC,nn" to jump over the next word without executing it
L2240:
  DEFB $01

; '?DD ERROR', "Redimensioned array"
;
; Used by the routine at FNDARY.
DD_ERR:
  LD E,$12
  LD BC,$221E

; "LD BC,nn" to jump over the next word without executing it
L2246:
  DEFB $01

; Overflow error
;
; Used by the routines at FADD, FDIV, RESDIV, MLSP10, RESZER and L1CC0.
OV_ERR:
  LD E,$0A

; "LD BC,nn" to jump over the next word without executing it
L2249:
  DEFB $01

; '?MO ERROR', "Missing Operand"
;
; Used by the routines at MUSIC, FNDWRD and OPRND.
OPERAND_ERR:
  LD E,$24

; "LD BC,nn" to jump over the next word without executing it
L224C:
  DEFB $01

; '?TM ERROR', "Type mismatch"
;
; Used by the routines at L1CC0 and TSTSTR.
TM_ERR:
  LD E,$18
  LD BC,$281E

; Routine at 8786
;
; Used by the routines at __CLOAD, FC_ERR, UL_ERR, __RETURN, FDTLP, L2A38,
; L2EC4, __CONT, TSTOPL, TESTR, CONCAT and BS_ERR.
ERROR:
  CALL L2EC4_3
  CALL CONSOLE_CRLF
  LD HL,ERRMSG_FR
  LD A,(FRGFLG)
  OR A
  JR Z,ERROR_0
  LD HL,ERRMSG
ERROR_0:
  INC E
  INC E
  RES 0,E
  LD A,$2B
  CP E
  JR NC,ERROR_1
  LD E,$2A
ERROR_1:
  LD A,(HL)
  INC HL
  OR A
  JR NZ,ERROR_1
  DEC E
  DEC E
  JR NZ,ERROR_1
; This entry point is used by the routine at INPBRK.
_ERROR_REPORT:
  CALL PRS
  LD HL,(CURLIN)
  LD DE,$FFFE
  RST $20
  JP Z,_STARTUP
  LD A,H
  AND L
  INC A
  CALL NZ,_ASCTFP_10
  LD A,$C1

; Routine at 8844
;
; Used by the routines at __CLOAD and __LIST.
RESTART:
  POP BC

; Routine at 8845
;
; Used by the routines at NMI_HANDLER and INPBRK.
READY:
  CALL FINLPT
  CALL CONSOLE_CRLF
  LD HL,OK_MSG
  CALL PRS

; Routine at 8857
;
; Used by the routine at L3184.
PROMPT:
  LD HL,$FFFF
  LD (CURLIN),HL
  LD A,(AUTFLG)
  OR A
  JR Z,PROMPT_2
  LD HL,(AUTLIN)
  JP SHIFT_STOP_23
; This entry point is used by the routine at SHIFT_STOP.
PROMPT_0:
  LD A,$20
; This entry point is used by the routine at SHIFT_STOP.
PROMPT_1:
  RST $18
PROMPT_2:
  CALL SHIFT_STOP_1
  JR C,PROMPT_2
  RST $10
  INC A
  DEC A
  JR Z,PROMPT
  PUSH AF
  LD A,($4889)
  OR A
  JP NZ,L0000
  CALL FC_ERR_0
  PUSH DE
  CALL TOKENIZE
  LD B,A
  POP DE
  POP AF
  JP NC,EXEC
  PUSH DE
  PUSH BC
  PUSH HL
  LD HL,(AUTINC)
  ADD HL,DE
  JP SHIFT_STOP_24
; This entry point is used by the routine at SHIFT_STOP.
PROMPT_3:
  POP HL
  RST $10
  OR A
  PUSH AF
  CALL SRCHLN
  JR C,PROMPT_4
  POP AF
  PUSH AF
  JP Z,PROMPT
  OR A
PROMPT_4:
  PUSH BC
  JR NC,PROMPT_6
  EX DE,HL
  LD HL,(VARTAB)
PROMPT_5:
  LD A,(DE)
  LD (BC),A
  INC BC
  INC DE
  RST $20
  JR NZ,PROMPT_5
  LD H,B
  LD L,C
  LD (VARTAB),HL
PROMPT_6:
  POP DE
  POP AF
  JR Z,PROMPT_9
  LD HL,(VARTAB)
  EX (SP),HL
  POP BC
  ADD HL,BC
  PUSH HL
  CALL MOVUP
  POP HL
  LD (VARTAB),HL
  EX DE,HL
  LD (HL),H
  POP DE
  INC HL
  INC HL
  LD (HL),E
  INC HL
  LD (HL),D
  INC HL
  EX DE,HL
  LD HL,BUFFER
PROMPT_7:
  LD A,(HL)
  LDI
  CP $0E
  JR NZ,PROMPT_8
  LDI
  LDI
  JR PROMPT_7
PROMPT_8:
  OR A
  JR NZ,PROMPT_7
; This entry point is used by the routine at __CLOAD.
PROMPT_9:
  CALL RUN_FST
  INC HL
  EX DE,HL
PROMPT_10:
  LD H,D
  LD L,E
  LD A,(HL)
  INC HL
  OR (HL)
  JP Z,PROMPT
  INC HL
PROMPT_11:
  INC HL
  INC HL
PROMPT_12:
  LD A,(HL)
  INC HL
  CP $0E
  JR Z,PROMPT_11
  OR A
  JR NZ,PROMPT_12
  EX DE,HL
  LD (HL),E
  INC HL
  LD (HL),D
  JR PROMPT_10
; This entry point is used by the routines at __GOTO, T_EDIT, __RESTORE, L3077,
; SCCPTR, __LIST, SHIFT_STOP and __NEW.
SRCHLN:
  LD BC,L0000
  LD HL,(TXTTAB)

; This entry point is used by the routine at RUNLIN.
;
; Used by the routine at __GOTO.
SRCHLP:
  LD (PRELIN),BC
  LD B,H
  LD C,L
  LD A,(HL)
  INC HL
  OR (HL)
  DEC HL
  RET Z
  INC HL
  INC HL
  LD A,(HL)
  INC HL
  LD H,(HL)
  LD L,A
  RST $20
  LD H,B
  LD L,C
  LD A,(HL)
  INC HL
  LD H,(HL)
  LD L,A
  CCF
  RET Z
  CCF
  RET NC
  JR SRCHLP

; TOKENIZE (CRUNCH) ALL "RESERVED" WORDS ARE TRANSLATED INTO SINGLE ONE OR TWO
; (IF TWO, FIRST IS ALWAYS $FF, 377 OCTAL) BYTES WITH THE MSB ON.
;
; Used by the routine at PROMPT.
TOKENIZE:
  XOR A
  LD (DORES),A
  LD C,$05
  LD DE,BUFFER
; This entry point is used by the routine at FNDWRD.
TOKENIZE_0:
  LD A,(HL)
  CP $20
  JP Z,FNDWRD_13
  LD B,A
  CP $22
  JP Z,FNDWRD_17
  OR A
  JP Z,FNDWRD_18
  LD A,(DORES)
  OR A
  LD A,(HL)
  JP NZ,FNDWRD_13
  CP $3F
  LD A,$94
  JP Z,FNDWRD_13
  LD A,(HL)
  CP $10
  LD A,$AF
  JP Z,FNDWRD_13
  LD A,(HL)
  CP $30
  JR C,FNDWRD
  CP $3C
  JP C,FNDWRD_13

; label
;
; Used by the routine at TOKENIZE.
FNDWRD:
  PUSH DE
  LD DE,WORDS
  PUSH BC
  LD BC,$244B
  PUSH BC
  LD B,$7F
  LD A,(HL)
  CP $61
  JR C,FNDWRD_0
  CP $7B
  JR NC,FNDWRD_0
  AND $5F
  LD (HL),A
FNDWRD_0:
  LD C,(HL)
  EX DE,HL
FNDWRD_1:
  INC HL
  OR (HL)
  JP P,FNDWRD_1
  INC B
  LD A,(HL)
  AND $7F
  RET Z
  CP C
  JR NZ,FNDWRD_1
  EX DE,HL
  PUSH HL
FNDWRD_2:
  INC DE
  LD A,(DE)
  OR A
  JP M,FNDWRD_6
  LD C,A
  LD A,B
  CP $8C
  JR Z,FNDWRD_3
  CP $88
  JR NZ,FNDWRD_4
FNDWRD_3:
  RST $10
  DEC HL
FNDWRD_4:
  INC HL
  LD A,(HL)
  CP $61
  JR C,FNDWRD_5
  AND $5F
FNDWRD_5:
  CP C
  JR Z,FNDWRD_2
  POP HL
  JR FNDWRD_0
FNDWRD_6:
  LD C,B
  POP AF
  POP AF
  LD A,C
  CP $88
  JR Z,FNDWRD_7
  CP $8C
  JR Z,FNDWRD_7
  CP $8B
  JR Z,FNDWRD_7
  CP $B6
  JR NZ,FNDWRD_12
FNDWRD_7:
  PUSH HL
  RST $10
  POP HL
  JR NC,FNDWRD_12
  EX DE,HL
  POP IY
  POP HL
FNDWRD_8:
  LD (HL),C
  INC DE
  PUSH DE
  EXX
  POP HL
  PUSH HL
  CALL FC_ERR_0
  LD A,E
  OR D
  JR Z,FNDWRD_10
  POP BC
  LD A,E
  DEC HL
  PUSH HL
  EXX
  POP DE
  INC HL
  LD (HL),$0E
  INC HL
  LD (HL),A
  INC HL
  EXX
  LD A,D
  EXX
  LD C,A
  INC IY
  INC IY
  INC IY
FNDWRD_9:
  RST $20
  JP NC,OPERAND_ERR
  EX DE,HL
  PUSH HL
  RST $10
  EX DE,HL
  CP $2C
  JR NZ,FNDWRD_11
  LD (HL),C
  INC HL
  INC IY
  LD C,$2C
  POP AF
  JR FNDWRD_8
FNDWRD_10:
  EXX
  POP DE
  DEC DE
  JR FNDWRD_9
FNDWRD_11:
  POP DE
  PUSH HL
  PUSH IY
  EX DE,HL
FNDWRD_12:
  LD A,C
  POP BC
  POP DE
; This entry point is used by the routine at TOKENIZE.
FNDWRD_13:
  INC HL
  LD (DE),A
  INC DE
  INC C
  SUB $3A
  JR Z,FNDWRD_14
  CP $49
  JR NZ,FNDWRD_15
FNDWRD_14:
  LD (DORES),A
FNDWRD_15:
  SUB $54
  JP NZ,TOKENIZE_0
  LD B,A
FNDWRD_16:
  LD A,(HL)
  OR A
  JR Z,FNDWRD_18
  CP B
  JR Z,FNDWRD_13
; This entry point is used by the routine at TOKENIZE.
FNDWRD_17:
  INC HL
  LD (DE),A
  INC C
  INC DE
  JR FNDWRD_16
; This entry point is used by the routine at TOKENIZE.
FNDWRD_18:
  LD HL,BUFMIN
  LD (DE),A
  INC DE
  LD (DE),A
  INC DE
  LD (DE),A
  RET

; Routine at 9338
__FOR:
  LD A,$64
  LD (SUBFLG),A
  CALL __LET
  POP BC
  PUSH HL
  CALL __DATA
  LD (ENDFOR),HL
  LD HL,$0002
  ADD HL,SP
FORSLP:
  CALL LOKFOR
  JR NZ,FORFND
  ADD HL,BC
  PUSH DE
  DEC HL
  LD D,(HL)
  DEC HL
  LD E,(HL)
  INC HL
  INC HL
  PUSH HL
  LD HL,(ENDFOR)
  RST $20
  POP HL
  POP DE
  JR NZ,FORSLP
  POP DE
  LD SP,HL
  INC C
FORFND:
  POP DE
  EX DE,HL
  LD C,$08
  CALL CHKSTK
  PUSH HL
  LD HL,(ENDFOR)
  EX (SP),HL
  PUSH HL
  LD HL,(CURLIN)
  EX (SP),HL
  CALL GETPARM_2BYTE_0

; Routine at 9403
L24BB:
  RST $08

; Data block at 9404
L24BC:
  DEFB $B3

; Routine at 9405
L24BD:
  CALL GETPARM_2BYTE
  PUSH HL
  CALL BCDEFP
  POP HL
  PUSH BC
  PUSH DE
  LD BC,$8100
  LD D,C
  LD E,D
  LD A,(HL)
  CP $B8
  LD A,$01
  JR NZ,SAVSTP
  RST $10
  CALL GETPARM_2BYTE
  PUSH HL
  CALL BCDEFP
  RST $28
  POP HL
SAVSTP:
  PUSH BC
  PUSH DE
  PUSH AF
  INC SP
  PUSH HL
  LD HL,(TEMP)
  EX (SP),HL
; This entry point is used by the routine at __NEXT.
PUTFID:
  LD B,$81
  PUSH BC
  INC SP
; This entry point is used by the routine at __NEXT.
NEWSTT:
  CALL ISCNTC
  LD (TEMP),HL
  LD A,(HL)
  CP $3A
  JR Z,EXEC
  OR A
  JP NZ,SN_ERR
  INC HL
  LD A,(HL)
  INC HL
  OR (HL)
  JP Z,ENDCON
  INC HL
  LD E,(HL)
  INC HL
  LD D,(HL)
  EX DE,HL
  LD (CURLIN),HL
  EX DE,HL
; This entry point is used by the routine at PROMPT.
EXEC:
  RST $10
  LD DE,NEWSTT
  PUSH DE
; This entry point is used by the routine at L2683.
_ONJMP:
  RET Z
; This entry point is used by the routine at L2669.
ONJMP:
  SUB $80
  JP C,__LET
  CP $32
  JP NC,SN_ERR
  RLCA
  LD C,A
  LD B,$00
  EX DE,HL
  LD HL,FNCTAB
  ADD HL,BC
  LD C,(HL)
  INC HL
  LD B,(HL)
  PUSH BC
  EX DE,HL

; Pick next char from program
;
; Used by the routine at DTSTR.
_CHRGTB:
  INC HL
  LD A,(HL)
  CP $3A
  RET NC
; This entry point is used by the routine at CHRGTB.
_CHRGTB_0:
  CP $20
  JR Z,_CHRGTB
  JR NC,NOTLFT
  CP $0F
  JR NC,_CHRGTB_1
  CP $0D
  JR C,_CHRGTB_1
  PUSH HL
  INC HL
  INC HL
  INC HL
  LD (CONTXT),HL
  POP HL
_CHRGTB_1:
  OR A
  RET
NOTLFT:
  CP $30
  CCF
  INC A
  DEC A
  RET
; This entry point is used by the routine at SCPTLP.
_CHRGTB_2:
  RST $10
; This entry point is used by the routine at __CLEAR.
INTIDX_0:
  CALL GETPARM_2BYTE

; Get integer variable to DE, error if negative
;
; Used by the routine at MAKINT.
DEPINT:
  RST $28
  JP M,FC_ERR

; parameter acquisition on 2 bytes
;
; Used by the routines at __CONIS, __CALL, L1C76, L1C82, L2945, NOT, __PEEK,
; GETWORD and L2FB1.
CONIS:
  LD A,(FPEXP)
  CP $90
  JP C,QINT
  LD BC,$9080
  LD DE,L0000
  PUSH HL
  CALL FCOMP
  POP HL
  LD D,C
  RET Z

; Routine at 9575
;
; Used by the routines at __LOG, __SETE, JPVEC_RAMLOW, __CLOAD, L1CC0, DEPINT,
; MAKINT, __CONT, L3077, __ASC, __MID_S and BS_ERR.
FC_ERR:
  LD E,$08
  JP ERROR
; This entry point is used by the routines at L1CC0, PROMPT, FNDWRD, __GOTO,
; __RESTORE, __RENUM, L306F, L3077, __AUTO, L3184, __LIST and __NEW.
FC_ERR_0:
  DEC HL
; This entry point is used by the routine at L2669.
FC_ERR_1:
  RST $10
  CP $0E
  JR NZ,ATOH
  INC HL
  LD E,(HL)
  INC HL
  LD D,(HL)
  RST $10
  RET

; ASCII to Integer, result in DE
;
; Used by the routine at FC_ERR.
ATOH:
  DEC HL
  LD DE,L0000
ATOH_0:
  RST $10
  RET NC
  PUSH HL
  PUSH AF
  LD HL,$1998
  RST $20
  JR C,ATOH_1
  LD H,D
  LD L,E
  ADD HL,DE
  ADD HL,HL
  ADD HL,DE
  ADD HL,HL
  POP AF
  SUB $30
  LD E,A
  LD D,$00
  ADD HL,DE
  EX DE,HL
  POP HL
  JR ATOH_0
ATOH_1:
  POP AF
  POP HL
  RET

; Routine at 9626
;
; Used by the routine at __CLOAD.
__RUN:
  RES 6,(IX+$03)
  JP Z,RUN_FST
  CALL _CLVAR
  LD BC,NEWSTT
  JR RUNLIN

; Routine at 9641
__GOSUB:
  LD C,$03
  CALL CHKSTK
  POP BC
  PUSH HL
  PUSH HL
  LD HL,(CURLIN)
  EX (SP),HL
  LD A,$8C
  PUSH AF
  INC SP
; This entry point is used by the routine at __RUN.
RUNLIN:
  PUSH BC

; Routine at 9658
;
; Used by the routine at L2683.
__GOTO:
  CALL FC_ERR_0
  CALL __REM
  INC HL
  PUSH HL
  LD HL,(CURLIN)
  RST $20
  POP HL
  CALL C,SRCHLP
  CALL NC,SRCHLN
  LD H,B
  LD L,C
  DEC HL
  RET C

; entry for '?UL ERROR'
;
; Used by the routine at __RESTORE.
UL_ERR:
  LD E,$0E
  JP ERROR

; Routine at 9686
__RETURN:
  RET NZ
  LD D,$FF
  CALL BAKSTK
  LD SP,HL
  CP $8C
  LD E,$04
  JP NZ,ERROR
  POP HL
  LD (CURLIN),HL
  INC HL
  LD A,H
  OR L
  JR NZ,__RETURN_0
__RETURN_0:
  LD HL,NEWSTT
  EX (SP),HL

; "LD A,n" to Mask the next byte
L25F1:
  DEFB $3E

; Routine at 9714
;
; Used by the routines at L2772 and GTVLUS.
NXTDTA:
  POP HL

; Routine at 9715
;
; Used by the routines at __FOR and FDTLP.
__DATA:
  LD BC,$0E3A

; Routine at 9717
;
; Used by the routines at __GOTO and L2683.
__REM:
  LD C,$00
  LD B,$00
__REM_0:
  LD A,C
  LD C,B
  LD B,A
__REM_1:
  LD A,(HL)
  CP $0E
  JR NZ,__REM_2
  INC HL
  INC HL
  INC HL
  LD A,(HL)
__REM_2:
  OR A
  RET Z
  CP B
  RET Z
  INC HL
  CP $22
  JR Z,__REM_0
  JR __REM_1

; Routine at 9744
;
; Used by the routines at __FOR and L24BD.
__LET:
  CALL GETVAR

; Routine at 9747
L2613:
  RST $08

; Data block at 9748
L2614:
  DEFB $C1

; Routine at 9749
L2615:
  PUSH DE
  LD A,(VALTYP)
  PUSH AF
  CALL EVAL
  POP AF
  EX (SP),HL
  LD (TEMP),HL
  RRA
  CALL TSTSTR_0
  JR Z,L2615_1
; This entry point is used by the routine at ITMSEP.
__LET_1:
  PUSH HL
  LD HL,(FACCU)
  PUSH HL
  INC HL
  INC HL
  LD E,(HL)
  INC HL
  LD D,(HL)
  LD HL,(TXTTAB)
  RST $20
  JR NC,$2646
  LD HL,(STREND)
  RST $20
  POP DE
  JR NC,L2615_0
  LD HL,DSCTMP
  RST $20
  JR NC,L2615_0
  LD A,$D1
  CALL BAKTMP
  EX DE,HL
  CALL SAVSTR_0
L2615_0:
  CALL BAKTMP
  POP HL
  CALL VMOVE
  POP HL
  RET
L2615_1:
  PUSH HL
  CALL FPTHL
  POP DE
  POP HL
  RET

; Routine at 9822
__ON:
  CALL GETINT
  LD A,(HL)
  LD B,A
  CP $8C
  JR Z,ONGO

; Routine at 9831
L2667:
  RST $08

; Data block at 9832
L2668:
  DEFB $88

; Routine at 9833
L2669:
  DEC HL
; This entry point is used by the routine at __ON.
ONGO:
  LD C,E
ONGOLP:
  DEC C
  LD A,B
  JP Z,ONJMP
  CALL FC_ERR_1
  CP $2C
  RET NZ
  JP ONGOLP

; Routine at 9849
__IF:
  CALL EVAL
  LD A,(HL)
  CP $88
  JR Z,IFGO

; Routine at 9857
L2681:
  RST $08

; Data block at 9858
L2682:
  DEFB $B6

; Routine at 9859
L2683:
  DEC HL
; This entry point is used by the routine at __IF.
IFGO:
  CALL GETPARM_2BYTE_0
  RST $28
  JP Z,__REM
  RST $10
  CP $0E
  JP Z,__GOTO
  JP _ONJMP

; Routine at 9876
__LPRINT:
  LD A,$01
  LD (PRTFLG),A
; This entry point is used by the routine at __PRINT.
MRPRNT:
  DEC HL
  RST $10

; Routine at 9883
__PRINT:
  CALL PRTHK
  CALL Z,OUTDO_CRLF
; This entry point is used by the routine at NEXITM.
PRNTLP:
  JP Z,FINPRT
  CP $B2
  JR Z,__TAB
  CP $B5
  JR Z,__TAB
  PUSH HL
  CP $2C
  JR Z,DOCOM
  CP $3B
  JP Z,NEXITM
  POP BC
  CALL EVAL
  PUSH HL
  LD A,(VALTYP)
  OR A
  JR NZ,PRNTST
  CALL NUMASC
  CALL CRTST
  LD (HL),$20
  LD HL,(FACCU)
  LD A,(PRTFLG)
  OR A
  JR Z,ISTTY
  LD A,(LPTPOS)
  ADD A,(HL)
  CP $84
  JR LINCH2
ISTTY:
  LD A,(LINLEN)
  LD B,A
  INC A
  JR Z,PRNTNB
  LD A,(CURPOS)
  ADD A,(HL)
  DEC A
  CP B
LINCH2:
  CALL NC,OUTDO_CRLF
PRNTNB:
  CALL PRS1
  XOR A
PRNTST:
  CALL NZ,PRS1
  POP HL
  JR MRPRNT
DOCOM:
  LD A,(PRTFLG)
  OR A
  JR Z,ISCTTY
  LD A,(LPTPOS)
  CP $75
  JR CHKCOM
ISCTTY:
  LD A,(CLMLST)
  LD B,A
  LD A,(CURPOS)
  DEC A
  CP B
CHKCOM:
  CALL NC,OUTDO_CRLF
  JR NC,NEXITM
ZONELP:
  SUB $0D
  JR NC,ZONELP
  CPL
  JR ASPCS

; __TAB(   &   __SPC(
;
; Used by the routine at __PRINT.
__TAB:
  PUSH AF
  CALL FNDNUM

; Routine at 10010
L271A:
  RST $08

; Data block at 10011
L271B:
  DEFB $29

; Routine at 10012
L271C:
  DEC HL
  POP AF
  SUB $B5
  PUSH HL
  JR Z,DOSPC
  LD A,(PRTFLG)
  OR A
  JR Z,TTYIST
  LD A,(LPTPOS)
  JR DOSPC
TTYIST:
  LD A,(CURPOS)
DOSPC:
  CPL
  ADD A,E
  JR NC,NEXITM
; This entry point is used by the routine at __PRINT.
ASPCS:
  INC A
  LD B,A
  LD A,$20
SPCLP:
  RST $18
  DJNZ SPCLP

; Routine at 10044
;
; Used by the routines at __PRINT and L271C.
NEXITM:
  POP HL
  RST $10
  JP PRNTLP
; This entry point is used by the routine at __PRINT.
FINPRT:
  XOR A
  LD (PRTFLG),A
  RET
; This entry point is used by the routine at INPBIN.
SCNSTR:
  LD A,(FLGINP)
  OR A
  JP NZ,DATSNR
  POP BC
  LD HL,REDO_MSG_FR
  LD A,(FRGFLG)
  OR A
  JR Z,NEXITM_0
  LD HL,REDO_MSG
NEXITM_0:
  CALL PRS
  JP L2EC4_4

; Routine at 10080
__INPUT:
  CALL L2A38_1
  CALL INPHK
  LD A,(HL)
  CP $22
  LD A,$00
  JR NZ,$2777
  CALL QTSTR

; Routine at 10096
L2770:
  RST $08

; Data block at 10097
L2771:
  DEFB $3B

; Routine at 10098
L2772:
  PUSH HL
  CALL PRS1
  LD A,$E5
  CALL QINLIN
  JR NOTQTI
  NOP
  NOP
  NOP
; This entry point is used by the routine at QINLIN.
L2772_0:
  CALL SHIFT_STOP_0
  RES 6,(IX+$03)
  LD A,(PICFLG)
  RES 5,A
  RES 0,A
  LD (PICFLG),A
  RET
  NOP
NOTQTI:
  POP BC
  JP C,INPBRK
  INC HL
  LD A,(HL)
  OR A
  DEC HL
  PUSH BC
  JP Z,NXTDTA
  LD (HL),$2C
  JR INPCON

; Routine at 10147
__READ:
  PUSH HL
  LD HL,(DATPTR)
  OR $AF

; Routine at 10152
;
; Used by the routine at L2772.
INPCON:
  XOR A
  LD (FLGINP),A
  EX (SP),HL

; "LD BC,nn" to jump over the next word without executing it
L27AD:
  DEFB $01

; CHECK FOR COMMA AND GET ANOTHER VARIABLE TO FILL WITH DATA
;
; Used by the routine at INPBIN.
LOPDT2:
  RST $08

; Data block at 10159
L27AF:
  DEFB $2C

; Routine at 10160
GTVLUS:
  CALL GETVAR
  EX (SP),HL
  PUSH DE
  LD A,(HL)
  CP $2C
  JR Z,SCNVAL
  LD A,(FLGINP)
  OR A
  JP NZ,FDTLP
  LD A,$3F
  RST $18
  CALL QINLIN
  POP DE
  POP BC
  JP C,INPBRK
  INC HL
  LD A,(HL)
  DEC HL
  OR A
  PUSH BC
  JP Z,NXTDTA
  PUSH DE

; Routine at 10197
;
; Used by the routines at GTVLUS and FDTLP.
SCNVAL:
  LD A,(VALTYP)
  OR A
  JR Z,INPBIN
  RST $10
  LD D,A
  LD B,A
  CP $22
  JR Z,STRENT
  LD A,(FLGINP)
  OR A
  LD D,A
  JR Z,ITMSEP
  LD D,$3A

; Item separator - ANSI USES [B]=44 AS A FLAG TO TRIGGER TRAILING SPACE
; SUPPRESSION
;
; Used by the routine at SCNVAL.
ITMSEP:
  LD B,$2C
  DEC HL
; This entry point is used by the routine at SCNVAL.
STRENT:
  CALL DTSTR
DOASIG:
  EX DE,HL
  LD HL,LTSTND
  EX (SP),HL
  PUSH DE
  JP __LET_1

; a.k.a. NUMINS
;
; Used by the routine at SCNVAL.
INPBIN:
  RST $10
  CALL H_ASCTFP
  EX (SP),HL
  CALL FPTHL
  POP HL
LTSTND:
  DEC HL
  RST $10
  JR Z,MORDT
  CP $2C
  JP NZ,SCNSTR
MORDT:
  EX (SP),HL
  DEC HL
  RST $10
  JR NZ,LOPDT2
  POP DE
  LD A,(FLGINP)
  OR A
  EX DE,HL
  JP NZ,__RESTORE_1
  PUSH DE
  OR (HL)
  LD HL,EXTRA_MSG_FR
  PUSH AF
  LD A,(FRGFLG)
  OR A
  JR Z,INPBIN_0
  LD HL,EXTRA_MSG
INPBIN_0:
  POP AF
  CALL NZ,PRS
  POP HL
  RET

; Find next DATA statement
;
; Used by the routine at GTVLUS.
FDTLP:
  CALL __DATA
  OR A
  JR NZ,FDTLP_0
  INC HL
  LD A,(HL)
  INC HL
  OR (HL)
  LD E,$06
  JP Z,ERROR
  INC HL
  LD E,(HL)
  INC HL
  LD D,(HL)
  LD (DATLIN),DE
FDTLP_0:
  RST $10
  CP $83
  JR NZ,FDTLP
  JR SCNVAL

; acquisition parameter sign on 2 bytes
;
; Used by the routines at _GETPARM_2BYTE, __CALL, L1C76, L1C82, L24BD, _CHRGTB,
; L2A38, GETINT and L2FB1.
GETPARM_2BYTE:
  CALL EVAL
; This entry point is used by the routines at __FOR, L2683, EVAL3, RETNUM,
; L2945, NOT, __DEF, L2A38, L2A97 and __STR_S.
GETPARM_2BYTE_0:
  OR $37

; Test a string, 'Type Error' if it is not
;
; Used by the routines at L2945, CONCAT and GETSTR.
TSTSTR:
  SCF
; This entry point is used by the routines at L2615 and L2945.
TSTSTR_0:
  LD A,(VALTYP)
  ADC A,A
  OR A
  RET PE
  JP TM_ERR

; Routine at 10331
FRMEQL:
  RST $08

; Data block at 10332
L285C:
  DEFB $C1

; Routine at 10333
L285D:
  JR EVAL

; Routine at 10335
;
; Used by the routines at EVLPAR and EVAL_VARIABLE.
OPNPAR:
  RST $08

; Data block at 10336
L2860:
  DEFB $28

; Routine at 10337
;
; Used by the routines at __PLAY, L1CC0, L2615, __IF, __PRINT, GETPARM_2BYTE,
; L285D and GETWORD.
EVAL:
  DEC HL
  LD D,$00

; Save precedence and eval until precedence break
;
; Used by the routines at STKTHS, MINUS and NOT.
EVAL1:
  PUSH DE
  LD C,$01
  CALL CHKSTK
  CALL OPRND

; Evaluate expression until precedence break
EVAL2:
  LD (NXTOPR),HL

; Evaluate expression until precedence break
;
; Used by the routine at NOT.
EVAL3:
  LD HL,(NXTOPR)
  POP BC
  LD A,B
  CP $78
  CALL NC,GETPARM_2BYTE_0
  LD A,(HL)
  LD (TEMP3),HL
  CP $B9
  RET C
  CP $C3
  RET NC
  CP $C0
  JR NC,DORELS
  SUB $B9
  LD E,A
  JR NZ,FOPRND
  LD A,(VALTYP)
  DEC A
  LD A,E
  JP Z,CONCAT
FOPRND:
  RLCA
  ADD A,E
  LD E,A
  LD HL,PRITAB
  LD D,$00
  ADD HL,DE
  LD A,B
  LD D,(HL)
  CP D
  RET NC
  INC HL
  CALL GETPARM_2BYTE_0

; Stack expression item and get next one
;
; Used by the routine at L2945.
STKTHS:
  PUSH BC
  LD BC,EVAL3
  PUSH BC
  LD B,E
  LD C,D
  CALL PUSHF
  LD E,B
  LD D,C
  LD C,(HL)
  INC HL
  LD B,(HL)
  INC HL
  PUSH BC
  LD HL,(TEMP3)
  JP EVAL1
; This entry point is used by the routine at EVAL3.
DORELS:
  LD D,$00
LOPREL:
  SUB $C0
  JP C,FINREL
  CP $03
  JP NC,FINREL
  CP $01
  RLA
  XOR D
  CP D
  LD D,A
  JP C,SN_ERR
  LD (TEMP3),HL
  RST $10
  JR LOPREL

; Get next expression value
;
; Used by the routines at EVAL1 and CONCAT.
OPRND:
  XOR A
  LD (VALTYP),A
  RST $10
  JP Z,OPERAND_ERR
  JP C,H_ASCTFP
  CP $26
  JP Z,OCTCNS
  CALL ISLETTER_A
  JR NC,EVAL_VARIABLE
  CP $B9
  JR Z,OPRND
  CP $2E
  JP Z,H_ASCTFP
  CP $BA
  JR Z,MINUS
  CP $22
  JP Z,QTSTR
  CP $B7
  JP Z,NOT
  CP $B4
  JP Z,L2A38_0
  SUB $C3
  JR NC,ISFUN

; End of expression.  Look for ')'.
;
; Used by the routines at L2945 and L2A38.
EVLPAR:
  CALL OPNPAR

; Routine at 10512
L2910:
  RST $08

; Data block at 10513
L2911:
  DEFB $29

; Routine at 10514
L2912:
  RET

; '-', deal with minus sign
;
; Used by the routine at OPRND.
MINUS:
  LD D,$7D
  CALL EVAL1
  LD HL,(NXTOPR)
  PUSH HL
  CALL NEG

; Routine at 10527
RETNUM:
  CALL GETPARM_2BYTE_0
  POP HL
  RET

; Routine at 10532
;
; Used by the routine at OPRND.
EVAL_VARIABLE:
  CALL GETVAR
  PUSH HL
  EX DE,HL
  LD (FACCU),HL
  LD A,(VALTYP)
  OR A
  CALL Z,MOVFM
  POP HL
  RET
; This entry point is used by the routine at OPRND.
ISFUN:
  LD B,$00
  RLCA
  LD C,A
  PUSH BC
  RST $10
  LD A,C
  CP $33
  JR C,OKNORM
  CALL OPNPAR

; Routine at 10563
L2943:
  RST $08

; Data block at 10564
L2944:
  DEFB $2C

; Routine at 10565
L2945:
  CALL TSTSTR
  EX DE,HL
  LD HL,(FACCU)
  EX (SP),HL
  PUSH HL
  EX DE,HL
  CALL GETINT
  EX DE,HL
  EX (SP),HL
  JR GOFUNC
; This entry point is used by the routine at EVAL_VARIABLE.
OKNORM:
  CALL EVLPAR
  EX (SP),HL
  LD DE,RETNUM
  PUSH DE
GOFUNC:
  LD BC,FNCTAB_FN
DISPAT:
  ADD HL,BC
  LD C,(HL)
  INC HL
  LD H,(HL)
  LD L,C
  JP (HL)
; This entry point is used by the routine at _ASCTFP.
SGNEXP:
  DEC D
  CP $BA
  RET Z
  CP $2D
  RET Z
  INC D
  CP $2B
  RET Z
  CP $B9
  RET Z
  DEC HL
  RET
  OR $AF
  PUSH AF
  CALL GETPARM_2BYTE_0
  CALL CONIS
  POP AF
  EX DE,HL
  POP BC
  EX (SP),HL
  EX DE,HL
  CALL FPBCDE
  PUSH AF
  CALL CONIS
  POP AF
  POP BC
  LD A,C
  LD HL,NOT_1
  JR NZ,L2945_0
  AND E
  LD C,A
  LD A,B
  AND D
  JP (HL)
L2945_0:
  OR E
  LD C,A
  LD A,B
  OR D
  JP (HL)
; This entry point is used by the routine at STKTHS.
FINREL:
  LD HL,$29B0
  LD A,(VALTYP)
  RRA
  LD A,D
  RLA
  LD E,A
  LD D,$64
  LD A,B
  CP D
  RET NC
  JP STKTHS
  OR D
  ADD HL,HL
  LD A,C
  OR A
  RRA
  POP BC
  POP DE
  PUSH AF
  CALL TSTSTR_0
  LD HL,DOCMP
  PUSH HL
  JP Z,FCOMP
  XOR A
  LD (VALTYP),A
  JP STRCMP
DOCMP:
  INC A
  ADC A,A
  POP BC
  AND B
  ADD A,$FF
  SBC A,A
  JP FLOAT

; 'NOT' boolean expression
;
; Used by the routine at OPRND.
NOT:
  LD D,$5A
  CALL EVAL1
  CALL GETPARM_2BYTE_0
  CALL CONIS
  LD A,E
  CPL
  LD C,A
  LD A,D
  CPL
  CALL NOT_1
  POP BC
  JP EVAL3
; This entry point is used by the routine at __FRE.
NOT_0:
  LD A,L
  SUB E
  LD C,A
  LD A,H
  SBC A,D
NOT_1:
  LD B,C
; This entry point is used by the routine at USR_RET.
NOT_2:
  LD D,B

; Get back from function passing an INT value in A+B registers
ABPASS:
  LD E,$00
  LD HL,VALTYP
  LD (HL),E
  LD B,$90
  JP FLOATR

; Routine at 10748
__LPOS:
  LD A,(LPTPOS)
  JR USR_RET

; Routine at 10753
__POS:
  CALL MAKINT
  INC A
  JR Z,__POS_0
  LD A,(CURPOS)
  JR USR_RET
__POS_0:
  LD A,(YCURSO)

; USR routine return
;
; Used by the routines at _USR_RET, __STICKX, __STICKY, __ACTION, __KEY,
; __LPOS, __POS and __PEEK.
USR_RET:
  LD B,A
  XOR A
  JR NOT_2

; Routine at 10771
__DEF:
  CALL L2A95
  CALL L2A38_1
  LD BC,__DATA
  PUSH BC
  PUSH DE
  LD DE,L0000
  LD A,(HL)
  CP $28
  JR NZ,L2A36
  RST $10
  CALL GETVAR
  PUSH HL
  EX DE,HL
  DEC HL
  LD D,(HL)
  DEC HL
  LD E,(HL)
  POP HL
  CALL GETPARM_2BYTE_0

; Routine at 10804
L2A34:
  RST $08

; Data block at 10805
L2A35:
  DEFB $29

; Routine at 10806
;
; Used by the routine at __DEF.
L2A36:
  RST $08

; Data block at 10807
L2A37:
  DEFB $C1

; Routine at 10808
L2A38:
  LD B,H
  LD C,L
  EX (SP),HL
  LD (HL),C
  INC HL
  LD (HL),B
  JP CRTMST_0
; This entry point is used by the routine at OPRND.
L2A38_0:
  CALL L2A95
  PUSH DE
  CALL EVLPAR
  CALL GETPARM_2BYTE_0
  EX (SP),HL
  LD E,(HL)
  INC HL
  LD D,(HL)
  INC HL
  LD A,D
  OR E
  JP Z,$2244
  LD A,(HL)
  INC HL
  LD H,(HL)
  LD L,A
  PUSH HL
  LD HL,(PRMNAM)
  EX (SP),HL
  LD (PRMNAM),HL
  LD HL,($49E4)
  PUSH HL
  LD HL,(PRMVAL)
  PUSH HL
  LD HL,PRMVAL
  PUSH DE
  CALL FPTHL
  POP HL
  CALL GETPARM_2BYTE
  DEC HL
  RST $10
  JP NZ,SN_ERR
  POP HL
  LD (PRMVAL),HL
  POP HL
  LD ($49E4),HL
  POP HL
  LD (PRMNAM),HL
  POP HL
  RET
; This entry point is used by the routines at __INPUT and __DEF.
L2A38_1:
  PUSH HL
  LD HL,(CURLIN)
  INC HL
  LD A,H
  OR L
  POP HL
  RET NZ
  LD E,$16
  JP ERROR

; Routine at 10901
;
; Used by the routines at __DEF and L2A38.
L2A95:
  RST $08

; Data block at 10902
L2A96:
  DEFB $B4

; Routine at 10903
L2A97:
  LD A,$80
  LD (SUBFLG),A
  OR (HL)
  LD C,A
  CALL GETVAR_0
  JP GETPARM_2BYTE_0

; Load 'A' with the next number in BASIC program
;
; Used by the routine at __TAB.
FNDNUM:
  RST $10

; Get a number to 'A'
;
; Used by the routines at __GETINT, __SOUND, L0C7E, __SETE, __TX, L0E19, L0E2D,
; __INIT, L0E63, __CURSOR, __DELIM, L0F63, L0F75, __DISPLAY, __ON, L2945, L2AC1
; and L3875.
GETINT:
  CALL GETPARM_2BYTE

; Convert tmp string to int in A register
;
; Used by the routines at __STICKX, __STICKY, __ACTION, __POS and __CHR_S.
MAKINT:
  CALL DEPINT
  LD A,D
  OR A
  JP NZ,FC_ERR
  DEC HL
  RST $10
  LD A,E
  RET

; Routine at 10932
__PEEK:
  CALL CONIS
  LD A,(DE)
  JP USR_RET

; Routine at 10939
__POKE:
  CALL GETWORD
  PUSH DE

; Routine at 10943
L2ABF:
  RST $08

; Data block at 10944
L2AC0:
  DEFB $2C

; Routine at 10945
L2AC1:
  CALL GETINT
  POP DE
  LD (DE),A
  RET

; Data block at 10951
L2AC7:
  DEFB $00,$00,$00,$00

; Get a number to DE (0..65535)
;
; Used by the routine at __POKE.
GETWORD:
  CALL EVAL
  PUSH HL
  CALL CONIS
  POP HL
  RET

; Text editor
;
; Used by the routine at SHIFT_STOP.
T_EDIT:
  PUSH BC
  PUSH DE
  PUSH HL
  LD A,(PICFLG)
  BIT 1,A
  JP NZ,T_EDIT_6
  HALT
  CALL SCR_CO_4
  OR A
  JR Z,T_EDIT_5
  PUSH AF
  EXX
  LD HL,$2020
  LD ($480E),HL
  LD DE,$0001
  CALL SOUND
  LD A,(PICFLG)
  NOP
  NOP
  OR $01
  LD (PICFLG),A
  POP AF
  JP M,T_EDIT_55
  CP $0D
  JP QINLIN_5
; This entry point is used by the routine at QINLIN.
T_EDIT_0:
  CP $09
  JP Z,T_EDIT_35
  CP $0A
  JP Z,T_EDIT_37
  CP $0C
  JP Z,T_EDIT_40
  JP QINLIN_0
  NOP
; This entry point is used by the routine at QINLIN.
T_EDIT_1:
  LD A,(PICFLG)
  BIT 5,A
  JR NZ,T_EDIT_4
  LD A,$1F
; This entry point is used by the routines at L33CD and QINLIN.
T_EDIT_2:
  RST $18
T_EDIT_3:
  LD (IX+$01),$01
  LD (IX+$00),$01
  HALT
; This entry point is used by the routine at L33CD.
T_EDIT_4:
  XOR A
T_EDIT_5:
  POP HL
  POP DE
  POP BC
  RET
T_EDIT_6:
  LD HL,(RETADR)
  LD A,H
  CP $18
  JR NC,T_EDIT_7
  PUSH HL
  INC H
  LD L,$00
  CALL L33CD_34
  POP HL
  JR Z,T_EDIT_8
T_EDIT_7:
  LD A,L
  CP $27
  JR NC,T_EDIT_11
T_EDIT_8:
  PUSH AF
  LD A,L
  CP $27
  JR C,T_EDIT_9
  INC H
  LD L,$00
T_EDIT_9:
  INC L
  LD (RETADR),HL
  PUSH HL
  CALL L33CD_34
  LD B,A
  POP DE
  POP AF
  LD A,B
  JR NC,T_EDIT_16
  CP $20
  JR NZ,T_EDIT_16
T_EDIT_10:
  INC E
  LD A,E
  CP $28
  JR NC,T_EDIT_11
  INC HL
  INC HL
  LD A,(HL)
  CP $20
  JR Z,T_EDIT_10
  LD A,$20
  JR T_EDIT_16
T_EDIT_11:
  LD A,(PICFLG)
  RES 1,A
  LD (PICFLG),A
  LD A,(ENTSTT)
  BIT 7,A
  JR NZ,T_EDIT_15
  CP $20
  JR Z,T_EDIT_12
  CP $30
  JR C,$2BA1
  CP $3A
  JR NC,$2BA1
T_EDIT_12:
  LD A,(CURPOS)
  CP $01
  JR Z,T_EDIT_14
; This entry point is used by the routine at QINLIN.
T_EDIT_13:
  CALL T_EDIT_23
  JR T_EDIT_15
T_EDIT_14:
  JP QINLIN_3
  NOP
  LD B,$CD
  OUT ($2B),A
  LD A,$1E
  RST $18
; This entry point is used by the routine at QINLIN.
T_EDIT_15:
  LD A,$0D
  JP T_EDIT_5
T_EDIT_16:
  CP $FF
  JR NZ,T_EDIT_17
  LD A,$10
T_EDIT_17:
  CP $0D
  JR NZ,T_EDIT_18
  LD A,$11
T_EDIT_18:
  CP $08
  JR NZ,T_EDIT_19
  LD A,$13
T_EDIT_19:
  CP $04
  JR NZ,T_EDIT_20
  LD A,$18
T_EDIT_20:
  CP $1F
  JR NZ,T_EDIT_21
  LD A,$1A
T_EDIT_21:
  CP $03
  JR NZ,T_EDIT_22
  LD A,$1C
T_EDIT_22:
  JP T_EDIT_5
T_EDIT_23:
  LD A,$03
  RST $18
  LD HL,(CURPOS)
  DEC L
  CALL L33CD_34
  JR Z,T_EDIT_23
  RET
; This entry point is used by the routine at QINLIN.
T_EDIT_24:
  LD HL,(CURPOS)
T_EDIT_25:
  DEC L
  PUSH HL
  CALL L33CD_34
  POP HL
  CP $80
  JR Z,T_EDIT_26
  XOR $30
  CP $0A
  JR NC,T_EDIT_27
  JR T_EDIT_25
T_EDIT_26:
  LD A,L
  OR A
  JR NZ,T_EDIT_25
  INC L
  LD A,(AUTFLG)
  OR A
  JR Z,T_EDIT_27
  XOR A
  LD (AUTFLG),A
  LD (CURPOS),HL
  LD A,$04
  RST $18
  JP T_EDIT_15
T_EDIT_27:
  LD L,$01
T_EDIT_28:
  LD (RETADR),HL
  PUSH HL
  DEC L
  CALL L33CD_34
  POP HL
  JR NZ,T_EDIT_29
  DEC H
  JP M,T_EDIT_34
  JR T_EDIT_28
T_EDIT_29:
  LD A,(PICFLG)
  BIT 5,A
  JR Z,T_EDIT_32
  NOP
  NOP
  NOP
  NOP
  NOP
  LD HL,(RETADR)
  PUSH HL
  CALL SCRADR
  POP DE
T_EDIT_30:
  LD A,E
  CP $28
  JR NC,T_EDIT_31
  INC E
  LD A,(HL)
  INC HL
  INC HL
  CP $3F
  JR NZ,T_EDIT_30
  LD (RETADR),DE
  JR T_EDIT_30
T_EDIT_31:
  LD A,$30
  JR T_EDIT_33
T_EDIT_32:
  CALL L33CD_34
T_EDIT_33:
  LD (ENTSTT),A
  LD A,(PICFLG)
  SET 1,A
  LD (PICFLG),A
  LD HL,(RETADR)
  DEC L
  LD (RETADR),HL
T_EDIT_34:
  JP T_EDIT_4
T_EDIT_35:
  LD A,(YCURSO)
  OR A
  JR NZ,T_EDIT_36
  LD A,(PICFLG)
  BIT 5,A
  JP NZ,T_EDIT_4
  LD DE,($4809)
  CALL SRCHLN
  LD BC,(PRELIN)
  LD A,B
  OR C
  JP Z,T_EDIT_4
  JP T_EDIT_41
T_EDIT_36:
  LD A,$09
  JP T_EDIT_2
T_EDIT_37:
  LD A,(YCURSO)
  CP $18
  JP C,T_EDIT_39
  LD A,(PICFLG)
  BIT 5,A
  JP NZ,T_EDIT_4
  LD DE,($4809)
  INC DE
  CALL SRCHLN
  JP C,T_EDIT_38
  JP Z,T_EDIT_4
T_EDIT_38:
  JP T_EDIT_41
T_EDIT_39:
  LD A,$0A
  JP T_EDIT_2
T_EDIT_40:
  LD A,(PICFLG)
  BIT 5,A
  JP NZ,T_EDIT_4
  SCF
  JR T_EDIT_42
T_EDIT_41:
  INC BC
  INC BC
  LD H,B
  LD L,C
  LD E,(HL)
  INC HL
  LD D,(HL)
  LD ($4809),DE
  OR A
  LD HL,(CURPOS)
  PUSH HL
T_EDIT_42:
  PUSH AF
  LD A,$81
  LD (PICFLG),A
  LD (IX+$00),$40
  LD (IX+$08),$00
  LD HL,$0001
  LD (CURPOS),HL
  CALL CLR_SCR
  LD DE,($4809)
  CALL SRCHLN
  LD H,B
  LD L,C
T_EDIT_43:
  CALL T_EDIT_47
  JR C,T_EDIT_44
  LD A,(YCURSO)
  CP $18
  LD A,$03
  CALL C,_CHPUT
  JR C,T_EDIT_43
T_EDIT_44:
  POP AF
  JR C,T_EDIT_45
  POP HL
  JR T_EDIT_46
T_EDIT_45:
  LD HL,$0001
T_EDIT_46:
  LD (CURPOS),HL
  JP T_EDIT_3
T_EDIT_47:
  LD C,(HL)
  INC HL
  LD B,(HL)
  INC HL
  LD A,B
  OR C
  JP Z,T_EDIT_53
  LD E,(HL)
  INC HL
  LD D,(HL)
  INC HL
  PUSH HL
  EX DE,HL
  CALL _ASCTFP_12
  LD A,$20
  POP HL
T_EDIT_48:
  RST $18
  LD A,(PICFLG)
  BIT 6,A
  JP NZ,T_EDIT_53
T_EDIT_49:
  LD A,(HL)
  INC HL
  CP $0E
  JR NZ,T_EDIT_50
  LD A,(HL)
  INC HL
  EX AF,AF'
  LD A,(HL)
  INC HL
  PUSH HL
  LD H,A
  EX AF,AF'
  LD L,A
  CALL _ASCTFP_12
  POP HL
  JR T_EDIT_49
T_EDIT_50:
  OR A
  JP Z,T_EDIT_54
  JP P,T_EDIT_48
  SUB $7F
  LD C,A
  LD DE,$209E
  CP $0F
  JR NZ,T_EDIT_51
  LD (IX+$08),$01
T_EDIT_51:
  LD A,(DE)
  INC DE
  OR A
  JP P,T_EDIT_51
  DEC C
  JP NZ,T_EDIT_51
T_EDIT_52:
  AND $7F
  RST $18
  LD A,(PICFLG)
  BIT 6,A
  JP NZ,T_EDIT_53
  LD A,(DE)
  INC DE
  OR A
  JP P,T_EDIT_52
  JP T_EDIT_49
T_EDIT_53:
  SCF
T_EDIT_54:
  LD (IX+$08),$00
  RET
T_EDIT_55:
  CP $BA
  JP C,T_EDIT_58
  CP $FB
  JP NC,T_EDIT_4
  LD HL,FNKSTR
T_EDIT_56:
  CP (HL)
  INC HL
  JR NZ,T_EDIT_56
T_EDIT_57:
  LD A,(HL)
  BIT 7,A
  JP NZ,T_EDIT_3
  RST $18
  INC HL
  JR T_EDIT_57
T_EDIT_58:
  SUB $B0
  LD E,A
  LD D,$00
  LD HL,$487A
  ADD HL,DE
  LD A,(HL)
  JP T_EDIT_2

; Message at 11678
FNKSTR:
  DEFB $D7
  DEFM "3.14159"
  DEFB $00
  DEFB $D8
  DEFM "PRINT "
  DEFB $00
  DEFB $D9
  DEFM "RENUM "
  DEFB $00
  DEFB $DA
  DEFM "AUTO "
  DEFB $00
  DEFB $DB
  DEFM "FRE(0)"
  DEFB $00
  DEFB $DC
  DEFM "REM "
  DEFB $00
  DEFB $DE
  DEFM "LOAD"
  DEFB $00
  DEFB $DF
  DEFM "SAVE"
  DEFB $00
  DEFB $E0
  DEFM "PLAY "
  DEFB $00
  DEFB $E1
  DEFM "CLOAD"
  DEFB $00
  DEFB $E2
  DEFM "CSAVE"
  DEFB $00
  DEFB $E3
  DEFM "CLEAR "
  DEFB $00
  DEFB $E4
  DEFM "DATA "
  DEFB $00
  DEFB $E5
  DEFM "SETEG "
  DEFB $00
  DEFB $E6
  DEFM "FOR I=1 TO "
  DEFB $00
  DEFB $E7
  DEFM "GOTO "
  DEFB $00
  DEFB $E8
  DEFM "GOSUB "
  DEFB $00
  DEFB $E9
  DEFM "INIT "
  DEFB $00
  DEFB $EA
  DEFM "RETURN"
  DEFB $00
  DEFB $EB
  DEFM "DIM "
  DEFB $00
  DEFB $EC
  DEFM "LIST "
  DEFB $00
  DEFB $ED
  DEFM "INPUT "
  DEFB $00
  DEFB $EE
  DEFM "NEXT I"
  DEFB $00
  DEFB $EF
  DEFM "READ "
  DEFB $00
  DEFB $F0
  DEFM "POKE "
  DEFB $00
  DEFB $F1
  DEFM "RESTORE "
  DEFB $00
  DEFB $F2
  DEFM "RUN "
  DEFB $00
  DEFB $F3
  DEFM "SOUND "
  DEFB $00
  DEFB $F4
  DEFM "CHR$("
  DEFB $00
  DEFB $F5
  DEFM "LEFT$("
  DEFB $00
  DEFB $F6
  DEFM "RIGHT$("
  DEFB $00
  DEFB $F7
  DEFM "MID$("
  DEFB $00
  DEFB $F8
  DEFM "CURSORX "
  DEFB $00
  DEFB $F9
  DEFM "CURSORY "
  DEFB $00
  DEFB $FA
  DEFM "SETET "
  DEFB $00

; BASIC interpreter miscellaneous
;
; Used by the routines at PROMPT and GETVAR.
MOVUP:
  CALL L2EC4
; This entry point is used by the routine at TESTR.
MOVSTR:
  PUSH BC
  EX (SP),HL
  POP BC
MOVLP:
  RST $20
  LD A,(HL)
  LD (BC),A
  RET Z
  DEC BC
  DEC HL
  JR MOVLP

; Check for C levels of stack
;
; Used by the routines at __FOR, __GOSUB, EVAL1 and BS_ERR.
CHKSTK:
  PUSH HL
  LD HL,(STREND)
  LD B,$00
  ADD HL,BC
  ADD HL,BC
  LD A,$E5

; Routine at 11972
;
; Used by the routines at L1CC0, MOVUP and BS_ERR.
L2EC4:
  PUSH HL
  LD A,$A0
  SUB L
  LD L,A
  LD A,$FF
  SBC A,H
  LD H,A
  JR C,OM_ERR
  ADD HL,SP
  POP HL
  RET C
; This entry point is used by the routines at _STARTUP, L2FB1 and BS_ERR.
OM_ERR:
  LD DE,$000C
  JP ERROR
  RET NZ
; This entry point is used by the routines at _STARTUP, __SAVE, __CLOAD and
; __NEW.
L2EC4_0:
  LD HL,(TXTTAB)
; This entry point is used by the routine at __NEW.
L2EC4_1:
  XOR A
  JP SHIFT_STOP_22
; This entry point is used by the routine at SHIFT_STOP.
L2EC4_2:
  INC HL
  LD (VARTAB),HL
; This entry point is used by the routines at PROMPT and __RUN.
RUN_FST:
  LD HL,(TXTTAB)
  DEC HL
; This entry point is used by the routines at __RUN, __CLEAR and L2FB1.
_CLVAR:
  LD (TEMP),HL
  LD HL,(MEMSIZ)
  LD (FRETOP),HL
  XOR A
  CALL __RESTORE
  LD HL,(VARTAB)
  LD (ARYTAB),HL
  LD (STREND),HL
; This entry point is used by the routines at _STARTUP, NMI_HANDLER and ERROR.
L2EC4_3:
  POP BC
  LD HL,(STKTOP)
  LD SP,HL
  LD HL,TEMPST
  LD (TEMPPT),HL
  CALL FINLPT
  XOR A
  LD L,A
  LD H,A
  LD (OLDTXT),HL
  LD (SUBFLG),A
  LD (PRMNAM),HL
  PUSH HL
  PUSH BC
; This entry point is used by the routine at NEXITM.
L2EC4_4:
  LD HL,(TEMP)
  RET

; Routine at 12062
;
; Used by the routine at L2EC4.
__RESTORE:
  EX DE,HL
  LD HL,(TXTTAB)
  JR Z,__RESTORE_0
  EX DE,HL
  CALL FC_ERR_0
  PUSH HL
  CALL SRCHLN
  LD H,B
  LD L,C
  POP DE
  JP NC,UL_ERR
__RESTORE_0:
  DEC HL
; This entry point is used by the routine at INPBIN.
__RESTORE_1:
  LD (DATPTR),HL
  EX DE,HL
  RET

; Routine at 12088
;
; Used by the routine at ISCNTC.
__STOP:
  RET NZ
  OR $C0

; Routine at 12090
__END:
  RET NZ
  LD (TEMP),HL

; "LD HL,nn" to jump over the next word without executing it
L2F3E:
  DEFB $21

; Routine at 12095
;
; Used by the routines at L2772 and GTVLUS.
INPBRK:
  OR $FF
  POP BC
; This entry point is used by the routine at L24BD.
ENDCON:
  LD HL,(CURLIN)
  PUSH AF
  LD A,L
  AND H
  INC A
  JR Z,NOLIN
  LD (OLDLIN),HL
  LD HL,(TEMP)
  LD (OLDTXT),HL
NOLIN:
  CALL FINLPT
  CALL CONSOLE_CRLF
  POP AF
  LD HL,BREAK_MSG_FR
  PUSH AF
  LD A,(FRGFLG)
  OR A
  JR Z,INPBRK_0
  LD HL,BREAK_MSG
INPBRK_0:
  POP AF
  JP NZ,_ERROR_REPORT
  JP READY

; Routine at 12143
__CONT:
  LD HL,(OLDTXT)
  LD A,H
  OR L
  LD DE,DCOMPR
  JP Z,ERROR
  LD DE,(OLDLIN)
  LD (CURLIN),DE
  RET
_FC_ERR_A:
  JP FC_ERR
__ERASE:
  LD A,$01
  LD (SUBFLG),A
  CALL GETVAR
  PUSH HL
  LD (SUBFLG),A
  LD H,B
  LD L,C
  DEC BC
  DEC BC
  DEC BC
  DEC BC
; This entry point is used by the routine at GETVAR.
__CONT_0:
  LD A,(HL)

; Check char in 'A' being in the 'A'..'Z' range
;
; Used by the routines at OPRND and GETVAR.
ISLETTER_A:
  CP $41
  RET C
  CP $5B
  CCF
  RET

; Routine at 12192
__CLEAR:
  JP Z,_CLVAR
  CALL INTIDX_0
  DEC HL
  RST $10
  PUSH HL
  LD HL,(MEMSIZ)
  JR Z,STORED
  POP HL

; Routine at 12207
L2FAF:
  RST $08

; Data block at 12208
L2FB0:
  DEFB $2C

; Routine at 12209
L2FB1:
  PUSH DE
  CALL GETPARM_2BYTE
  CALL CONIS
  DEC HL
  RST $10
  JP NZ,SN_ERR
  EX (SP),HL
  EX DE,HL
; This entry point is used by the routine at __CLEAR.
STORED:
  CALL L2FB1_0
  JP C,OM_ERR
  PUSH HL
  LD HL,(VARTAB)
  LD BC,VSIGN
  ADD HL,BC
  RST $20
  JP NC,OM_ERR
  EX DE,HL
  LD (STKTOP),HL
  POP HL
  LD (MEMSIZ),HL
  POP HL
  JP _CLVAR
L2FB1_0:
  LD A,L
  SUB E
  LD E,A
  LD A,H
  SBC A,D
  LD D,A
  RET

; Routine at 12260
__NEXT:
  LD DE,L0000
__NEXT_0:
  CALL NZ,GETVAR
  LD (TEMP),HL
  CALL BAKSTK
  JP NZ,$223E
  LD SP,HL
  PUSH DE
  LD A,(HL)
  PUSH AF
  INC HL
  PUSH DE
  CALL MOVFM
  EX (SP),HL
  PUSH HL
  CALL FADDS
  POP HL
  CALL FPTHL
  POP HL
  CALL LOADFP
  PUSH HL
  CALL FCOMP
  POP HL
  POP BC
  SUB B
  CALL LOADFP
  JR Z,__NEXT_1
  EX DE,HL
  LD (CURLIN),HL
  LD L,C
  LD H,B
  JP PUTFID
__NEXT_1:
  LD SP,HL
  LD HL,(TEMP)
  LD A,(HL)
  CP $2C
  JP NZ,NEWSTT
  RST $10
  CALL __NEXT_0
; This entry point is used by the routines at H_ASCTFP and OPRND.
OCTCNS:
  INC HL

; Routine at 12333
L302D:
  RST $08

; Data block at 12334
L302E:
  DEFB $22

; Routine at 12335
L302F:
  LD DE,L0000
  LD B,$05
  DEC HL
L302F_0:
  CALL __CALL_1
  JR C,L302F_1
  EX DE,HL
  ADD HL,HL
  ADD HL,HL
  ADD HL,HL
  ADD HL,HL
  OR L
  LD L,A
  EX DE,HL
  DJNZ L302F_0
L302F_1:
  CP $22
  JP NZ,SN_ERR
  RST $10
  LD B,$98
  PUSH HL
  XOR A
  CALL FLOATR
  POP HL
  RET
  NOP
  NOP
  NOP
  NOP
  NOP

; Routine at 12376
__RENUM:
  LD BC,$000A
  PUSH BC
  LD D,B
  LD E,B
  JR Z,L3077_0
  CP $2C
  JR Z,L306D
  PUSH DE
  CALL FC_ERR_0
  LD B,D
  LD C,E
  POP DE
  JR Z,L3077_0

; Routine at 12397
;
; Used by the routine at __RENUM.
L306D:
  RST $08

; Data block at 12398
L306E:
  DEFB $2C

; Routine at 12399
L306F:
  CALL FC_ERR_0
  JR Z,L3077_0
  POP AF

; Routine at 12405
L3075:
  RST $08

; Data block at 12406
L3076:
  DEFB $2C

; Routine at 12407
L3077:
  PUSH DE
  CALL FC_ERR_0
  JP NZ,SN_ERR
  LD A,D
  OR E
  JP Z,FC_ERR
  EX DE,HL
  EX (SP),HL
  EX DE,HL
; This entry point is used by the routines at __RENUM and L306F.
L3077_0:
  PUSH BC
  CALL SRCHLN
  POP DE
  PUSH DE
  PUSH BC
  CALL SRCHLN
  LD H,B
  LD L,C
  POP DE
  RST $20
  EX DE,HL
  JP C,FC_ERR
  POP DE
  POP BC
  POP AF
  PUSH HL
  PUSH DE
  JR L3077_2
L3077_1:
  ADD HL,BC
  JP C,FC_ERR
  EX DE,HL
  PUSH HL
  LD HL,$FFF9
  RST $20
  POP HL
  JP C,FC_ERR
L3077_2:
  PUSH DE
  LD E,(HL)
  LD A,E
  INC HL
  LD D,(HL)
  OR D
  EX DE,HL
  POP DE
  JR Z,__RENUM_FIN
  LD A,(HL)
  INC HL
  OR (HL)
  DEC HL
  EX DE,HL
  JR NZ,L3077_1
__RENUM_FIN:
  PUSH BC
  CALL SCCLIN
  POP BC
  POP DE
  POP HL
__RENUM_LP:
  PUSH DE
  LD E,(HL)
  LD A,E
  INC HL
  LD D,(HL)
  OR D
  JR Z,LINE2PTR
  EX DE,HL
  EX (SP),HL
  EX DE,HL
  INC HL
  LD (HL),E
  INC HL
  LD (HL),D
  EX DE,HL
  ADD HL,BC
  EX DE,HL
  POP HL
  JR __RENUM_LP
LINE2PTR:
  LD BC,RESTART
  PUSH BC

; 'CP $F6'  masking the next byte/instr.
L30DE:
  DEFB $FE

; 'OR $AF'  masking the next instruction
SCCLIN:
  DEFB $F6

; THE SUBROUTINES SCCLIN AND SCCPTR CONVERT ALL LINE #'S TO POINTERS AND
; VICE-VERSA.
SCCPTR:
  XOR A
  LD (PTRFLG),A
  LD HL,(TXTTAB)
  DEC HL
SCCPTR_0:
  INC HL
  LD A,(HL)
  INC HL
  OR (HL)
  RET Z
  INC HL
  LD E,(HL)
  INC HL
  LD D,(HL)
SCNEXT:
  RST $10
  OR A
  JR Z,SCCPTR_0
  LD C,A
  LD A,(PTRFLG)
  OR A
  LD A,C
  JR Z,SCCPTR_4
  CP $0E
  JR NZ,SCNEXT
  PUSH DE
  INC HL
  LD E,(HL)
  INC HL
  LD D,(HL)
  INC HL
  PUSH HL
  CALL SRCHLN
  DEC BC
  LD A,$0D
  JR C,SCCPTR_6
  CALL CONSOLE_CRLF
  LD HL,LINE_MSG_FR
  LD A,(FRGFLG)
  OR A
  JR Z,SCCPTR_1
  LD HL,LINE_MSG
SCCPTR_1:
  PUSH DE
  CALL PRS
  POP HL
  CALL _ASCTFP_12
  LD HL,LINE_ERR_MSG_FR
  LD A,(FRGFLG)
  OR A
  JR Z,SCCPTR_2
  LD HL,LINE_ERR_MSG
SCCPTR_2:
  CALL PRS
  POP BC
  POP HL
  PUSH HL
  PUSH BC
  CALL _ASCTFP_10
SCNPOP:
  POP HL
SCNEX3:
  POP DE
  DEC HL
SCCPTR_3:
  JR SCNEXT
SCCPTR_4:
  CP $0E
  JR NZ,SCCPTR_5
  INC HL
  INC HL
  JR SCCPTR_3
SCCPTR_5:
  CP $0D
  JR NZ,SCCPTR_3
  PUSH DE
  INC HL
  LD E,(HL)
  INC HL
  LD D,(HL)
  INC HL
  PUSH HL
  EX DE,HL
  INC HL
  INC HL
  INC HL
  LD C,(HL)
  INC HL
  LD B,(HL)
  LD A,$0E
SCCPTR_6:
  LD HL,SCNPOP
  PUSH HL
  LD HL,(CONTXT)
  PUSH HL
  DEC HL
  LD (HL),B
  DEC HL
  LD (HL),C
  DEC HL
  LD (HL),A
  POP HL
  RET

; Routine at 12654
__AUTO:
  LD DE,$000A
  PUSH DE
  JR Z,L3184_0
  CALL FC_ERR_0
  EX AF,AF'
  LD A,D
  OR E
  JP Z,SN_ERR
  POP BC
  PUSH DE
  EX AF,AF'
  JR Z,L3184_0

; Routine at 12674
L3182:
  RST $08

; Data block at 12675
L3183:
  DEFB $2C

; Routine at 12676
L3184:
  CALL FC_ERR_0
  EX AF,AF'
  LD A,D
  OR E
  JP Z,SN_ERR
  EX AF,AF'
  JP NZ,SN_ERR
  LD (AUTINC),DE
; This entry point is used by the routine at __AUTO.
L3184_0:
  POP DE
  LD (AUTLIN),DE
  LD A,$01
  LD (AUTFLG),A
; This entry point is used by the routine at __LOAD.
L3184_1:
  POP BC
  JP PROMPT

; Routine at 12707
__LLIST:
  LD A,$01
  LD (PRTFLG),A

; Routine at 12712
__LIST:
  CALL __LIST_0
  CALL __LIST_4
  JP RESTART
; This entry point is used by the routines at __SAVE and __LOAD.
__LIST_0:
  LD DE,L0000
  DEC HL
  RST $10
  JR Z,__LIST_1
  CP $2C
  CALL NZ,FC_ERR_0
__LIST_1:
  LD (TEMP),DE
  PUSH HL
  CALL SRCHLN
  POP HL
  PUSH BC
  DEC HL
  RST $10
  JR Z,__LIST_2
  CP $2C
  JP NZ,SN_ERR
  INC HL
  CALL FC_ERR_0
  JP NZ,SN_ERR
  LD A,D
  OR E
  JR NZ,__LIST_3
__LIST_2:
  LD DE,$FFFF
__LIST_3:
  LD ($480E),DE
  POP DE
  RET
; This entry point is used by the routine at __SAVE.
__LIST_4:
  LD A,$01
  LD (PICFLG),A
  EX DE,HL
__LIST_5:
  LD C,(HL)
  INC HL
  LD B,(HL)
  INC HL
  LD A,B
  OR C
  RET Z
  CALL ISCNTC
  CP $04
  JR NZ,__LIST_8
  PUSH HL
__LIST_6:
  CALL KBDSCAN
  OR A
  JR NZ,__LIST_6
__LIST_7:
  CALL ISCNTC
  CALL KBDSCAN
  OR A
  JR Z,__LIST_7
  POP HL
__LIST_8:
  LD E,(HL)
  INC HL
  LD D,(HL)
  PUSH HL
  LD HL,($480E)
  RST $20
  POP HL
  CCF
  RET NC
  LD ($4809),DE
  INC HL
  PUSH HL
  EX DE,HL
  CALL _ASCTFP_12
  LD A,$20
  POP HL
  LD (RETADR),HL
  RST $18
  LD HL,(RETADR)
  LD (RETADR),BC
  LD A,$DF
__LIST_9:
  LD A,(HL)
  INC HL
  CP $0E
  JR NZ,__LIST_10
  LD A,$20
  RST $18
  LD A,(HL)
  INC HL
  EX AF,AF'
  LD A,(HL)
  INC HL
  PUSH HL
  LD H,A
  EX AF,AF'
  LD L,A
  CALL _ASCTFP_12
  POP HL
  JR __LIST_9
__LIST_10:
  OR A
  JR Z,__LIST_13
  JP P,$322D
  SUB $7F
  LD C,A
  LD DE,$209E
  CP $0F
  JR NZ,__LIST_11
  LD (IX+$08),$01
__LIST_11:
  LD A,(DE)
  INC DE
  OR A
  JP P,__LIST_11
  DEC C
  JR NZ,__LIST_11
__LIST_12:
  AND $7F
  RST $18
  LD A,(DE)
  INC DE
  OR A
  JP P,__LIST_12
  JR __LIST_9
__LIST_13:
  LD (IX+$08),$00
  CALL OUTDO_CRLF
  JP __LIST_5

; Routine at 12920
;
; Used by the routines at _STARTUP and NMI_HANDLER.
SCR_CO:
  SET 6,(IX+$03)
  BIT 0,(IX+$04)
  SET 4,(IX+$03)
  JR Z,SCR_CO_0
  RES 4,(IX+$03)
SCR_CO_0:
  CALL L33CD_41
  LD A,$7F
  JP SHIFT_STOP_27
; This entry point is used by the routine at SHIFT_STOP.
SCR_CO_1:
  CALL L0D47_4
  LD A,$E6
; This entry point is used by the routines at L0E63 and L3362.
SCR_CO_2:
  LD (IX+$09),A
  CALL CLR_SCR
; This entry point is used by the routine at L3362.
SCR_CO_3:
  LD HL,$0001
  LD (CURPOS),HL
  LD (IX+$01),$01
  LD (IX+$00),$01
  HALT
  RET
  EXX
  PUSH AF
  CALL KBDSCAN
  OR A
  POP BC
  LD A,B
  EXX
  RET
; This entry point is used by the routine at T_EDIT.
SCR_CO_4:
  EXX
  CALL KBDSCAN
  JR NC,SCR_CO_7
  LD L,A
  BIT 7,(IX+$07)
  LD A,(IX+$06)
  JP NZ,SCR_CO_5
  CP $80
  JP SCR_CO_6
SCR_CO_5:
  CP $08
SCR_CO_6:
  JP C,SCR_CO_12
  LD (IX+$07),$80
  LD A,L
  JP SCR_CO_8
SCR_CO_7:
  LD (IX+$07),$00
SCR_CO_8:
  LD (IX+$06),$00
  CP $01
  JR NZ,SCR_CO_11
  BIT 0,(IX+$04)
  JR Z,SCR_CO_9
  RES 0,(IX+$04)
  SET 4,(IX+$03)
  JR SCR_CO_10
SCR_CO_9:
  SET 0,(IX+$04)
  RES 4,(IX+$03)
SCR_CO_10:
  LD (IX+$01),$01
  JR SCR_CO_12
SCR_CO_11:
  BIT 0,(IX+$04)
  JR Z,SCR_CO_13
  CP $61
  JR C,SCR_CO_13
  CP $7B
  JR NC,SCR_CO_13
  RES 5,A
  JR SCR_CO_13
SCR_CO_12:
  XOR A
SCR_CO_13:
  OR A
  EXX
  RET

; Routine at 13082
;
; Used by the routines at T_EDIT, L33CD and FINLPT.
_CHPUT:
  EXX
  PUSH AF
  LD HL,_CHPUT_1
  PUSH HL
  CP $80
  JR C,_CHPUT_0
  CP $A0
  JR NC,L334F_0
  AND $7F
  JR _CHPUT_1
_CHPUT_0:
  CP $20
  JR NC,L334F_0
  BIT 7,(IX+$0A)
  JR NZ,_CHPUT_1
  BIT 7,(IX+$08)
  JR NZ,L334F_0
_CHPUT_1:
  LD D,$00
  PUSH AF
  LD E,A
  ADD A,A
  ADD A,E
  LD E,A
  LD HL,L3362
  ADD HL,DE
  POP AF
  JP (HL)
; This entry point is used by the routine at L3362.
_CHPUT_2:
  LD A,$FF
  LD HL,$0D3E

; Routine at 13132
;
; Used by the routine at L3362.
L334C:
  LD A,$0D
  LD HL,$083E

; Routine at 13135
;
; Used by the routine at L3362.
L334F:
  LD A,$08
  LD HL,$043E
  LD HL,$1F3E
  LD HL,$033E
; This entry point is used by the routines at _CHPUT and L3362.
L334F_0:
  POP HL
  LD HL,L33CD
  PUSH HL
  JP GETCHAR_2

; Routine at 13154
L3362:
  JP L33CD_43
  JP L33CD_43
  JP L33CD_14
  JP L33CD_0
  JP L33CD_21
  JP L33CD_25
  JP L33CD_35
  JP L33CD_11
  JP L33CD_8
  JP L33CD_5
  JP L33CD_2
  JP L33CD_43
  JP SCR_CO_3
  JP L33CD_0
  JP L33CD_42
  JP L33CD_41
  JP _CHPUT_2
  JP L334C
  JP L334F_0
  JP L334F
  JP L334F_0
  JP L334F_0
  JP L334F_0
  JP L334F_0
  JP $3352
  JP L334F_0
  JP $3355
  JP L334F_0
  JP $3358
  JP L334F_0
  JP L33CD_37
  LD A,(IX+$09)
  JP SCR_CO_2
_CHPUT_1:
  LD A,(PICFLG)
  RES 2,A
  LD (PICFLG),A

; Routine at 13261
L33CD:
  LD (IX+$01),$01
  POP AF
  EXX
  RET
; This entry point is used by the routine at L3362.
L33CD_0:
  LD HL,(CURPOS)
  LD A,$01
  CP L
  JR NZ,L33CD_1
  LD A,(PICFLG)
  BIT 2,A
  JR Z,L33CD_1
  CALL SCRADR
  DEC HL
  DEC HL
  LD (HL),$80
  RET
L33CD_1:
  LD L,$01
  LD (CURPOS),HL
; This entry point is used by the routine at L3362.
L33CD_2:
  LD HL,(CURPOS)
  LD A,H
  CP $18
  JR C,L33CD_3
  LD A,(PICFLG)
  BIT 7,A
  JR NZ,L33CD_4
  CALL L33CD_39
  LD A,$18
  CALL CLR_LINE
  JR L33CD_4
L33CD_3:
  INC H
  LD (CURPOS),HL
L33CD_4:
  RET
; This entry point is used by the routine at L3362.
L33CD_5:
  LD HL,(CURPOS)
  LD A,H
  OR A
  JR NZ,L33CD_6
  LD A,(PICFLG)
  BIT 7,A
  JR NZ,L33CD_7
  CALL L33CD_40
  LD A,$00
  CALL CLR_LINE
  JR L33CD_7
L33CD_6:
  DEC H
  LD (CURPOS),HL
L33CD_7:
  RET
; This entry point is used by the routine at L3362.
L33CD_8:
  LD HL,(CURPOS)
  LD A,L
  CP $02
  JR C,L33CD_10
  DEC L
L33CD_9:
  LD (CURPOS),HL
  RET
L33CD_10:
  LD A,H
  OR A
  RET Z
  CALL L33CD_5
  LD L,$27
  JR L33CD_9
; This entry point is used by the routine at L3362.
L33CD_11:
  LD HL,(CURPOS)
  LD A,L
  CP $27
  JR NC,L33CD_13
  INC L
L33CD_12:
  LD (CURPOS),HL
  RET
L33CD_13:
  LD A,H
  CP $18
  RET NC
  CALL L33CD_2
  LD L,$01
  JR L33CD_12
; This entry point is used by the routine at L3362.
L33CD_14:
  LD HL,(CURPOS)
  LD A,$01
  CP L
  JP NC,L33CD_17
  PUSH HL
  CALL SCRADR
  POP BC
  LD D,H
  LD E,L
  DEC DE
  DEC DE
  LD A,$28
  SUB C
  ADD A,A
  LD C,A
  LD B,$00
L33CD_15:
  LDIR
  LD A,(HL)
  CP $84
  DEC HL
  DEC HL
  JR NZ,L33CD_19
  INC HL
  INC HL
L33CD_16:
  INC HL
  INC HL
  LD A,(HL)
  DEC HL
  DEC HL
  DEC HL
  DEC HL
  LD (HL),A
  INC HL
  INC HL
  INC HL
  INC HL
  LD D,H
  LD E,L
  INC HL
  INC HL
  LD BC,$004C
  JR L33CD_15
L33CD_17:
  LD A,H
  OR A
  RET Z
  DEC HL
  CALL L33CD_34
  RET NZ
  JR L33CD_16
; This entry point is used by the routine at QINLIN.
L33CD_18:
  JP NZ,T_EDIT_4
  LD A,$06
  JP T_EDIT_2
L33CD_19:
  LD A,$20
  BIT 7,(IX+$08)
  JR Z,L33CD_20
  XOR A
L33CD_20:
  LD (HL),A
  CALL L33CD_8
  RET
; This entry point is used by the routine at L3362.
L33CD_21:
  LD HL,(CURPOS)
  PUSH HL
  LD A,$28
  SUB L
  PUSH AF
  CALL SCRADR
  POP BC
  LD E,(IX+$08)
  LD A,$20
  BIT 7,E
  JR Z,L33CD_22
  XOR A
L33CD_22:
  LD (HL),A
  INC HL
  LD (HL),E
  INC HL
  DJNZ L33CD_22
L33CD_23:
  POP HL
  LD L,$00
  INC H
  PUSH HL
  CALL L33CD_34
  POP HL
  JR NZ,L33CD_24
  LD A,H
  PUSH HL
  CALL CLR_LINE
  JR L33CD_23
L33CD_24:
  RET
; This entry point is used by the routine at L3362.
L33CD_25:
  LD HL,(CURPOS)
  LD L,$00
L33CD_26:
  INC H
  PUSH HL
  CALL L33CD_34
  EX DE,HL
  POP HL
  JP Z,L33CD_26
  EX DE,HL
  DEC HL
  DEC HL
  LD A,(HL)
  DEC D
  PUSH DE
  BIT 7,(IX+$08)
  JR Z,L33CD_27
  OR A
  JP Z,L33CD_30
  JR L33CD_28
L33CD_27:
  CP $20
  JP Z,L33CD_30
L33CD_28:
  LD HL,(CURPOS)
  DEC H
  JP QINLIN_4
; This entry point is used by the routine at QINLIN.
L33CD_29:
  PUSH HL
  LD (CURPOS),DE
  LD A,$06
  CALL _CHPUT
  LD HL,(CURPOS)
  LD L,$00
  CALL SCRADR
  LD A,$84
  LD (HL),A
  POP HL
  LD (CURPOS),HL
; This entry point is used by the routine at QINLIN.
L33CD_30:
  LD HL,(CURPOS)
  POP DE
  LD A,D
  CP H
  JP Z,L33CD_31
  LD H,D
  LD L,E
  DEC D
  PUSH DE
  CALL SCRADR
  LD BC,$004C
  ADD HL,BC
  INC HL
  LD D,H
  LD E,L
  INC DE
  INC DE
  LDDR
  DEC HL
  DEC HL
  DEC HL
  LD A,(HL)
  INC HL
  INC HL
  INC HL
  INC HL
  LD (HL),A
  JP L33CD_30
L33CD_31:
  LD HL,(CURPOS)
  PUSH HL
  LD L,$26
  CALL SCRADR
  POP BC
  LD D,H
  LD E,L
  INC DE
  INC DE
  LD A,$27
  SUB C
  JR Z,L33CD_32
  ADD A,A
  LD C,A
  LD B,$00
  LDDR
L33CD_32:
  LD A,$20
  BIT 7,(IX+$08)
  JR Z,L33CD_33
  XOR A
L33CD_33:
  CALL GETCHAR_2
  CALL L33CD_8
  RET
; This entry point is used by the routine at T_EDIT.
L33CD_34:
  CALL SCRADR
  LD A,(HL)
  CP $84
  RET
; This entry point is used by the routine at L3362.
L33CD_35:
  LD HL,(CURPOS)
  LD A,H
  OR A
  JR Z,L33CD_36
  LD L,$00
  CALL SCRADR
  LD DE,SCREEN
  OR A
  SBC HL,DE
  LD C,L
  LD B,H
  LD HL,$4050
  LDIR
L33CD_36:
  LD A,(YCURSO)
  CALL CLR_LINE
  RET
; This entry point is used by the routine at L3362.
L33CD_37:
  LD A,(YCURSO)
L33CD_38:
  PUSH AF
  CALL CLR_LINE
  POP AF
  INC A
  CP $19
  JR C,L33CD_38
  RET
; This entry point is used by the routine at GETCHAR.
L33CD_39:
  LD HL,$4050
  LD DE,SCREEN
  LD BC,$0780
  LDIR
  RET
L33CD_40:
  LD HL,$4780
  LD DE,INT_SUB
  LD BC,$0780
  LDDR
  RET
; This entry point is used by the routines at SCR_CO, L3362 and FINLPT.
L33CD_41:
  LD A,(PICFLG)
  AND $7D
  LD (PICFLG),A
  LD (IX+$0A),$00
  LD (IX+$08),$00
  LD (IX+$01),$01
  LD (IX+$02),$0A
  RES 7,(IX+$03)
  SET 6,(IX+$03)
  RET
; This entry point is used by the routine at L3362.
L33CD_42:
  LD DE,$007D
  LD HL,$2844
  LD ($480E),HL
  CALL SOUND
  EXX
; This entry point is used by the routine at L3362.
L33CD_43:
  RET

; Data block at 13812
L35F4:
  DEFB $00,$10,$38,$7C,$FE,$7C,$38,$10
  DEFB $00,$00,$00,$00,$00,$00,$00,$24
  DEFB $00,$00,$00,$00,$00

; BASIC string handler
;
; Used by the routine at L2945.
STRCMP:
  PUSH DE
  CALL GSTRCU
  LD A,(HL)
  INC HL
  INC HL
  LD C,(HL)
  INC HL
  LD B,(HL)
  POP DE
  PUSH BC
  PUSH AF
  CALL GSTRDE
  CALL LOADFP
  POP AF
  LD D,A
  POP HL

; Routine at 13855
CSLOOP:
  LD A,E
  OR D
  RET Z
  LD A,D
  SUB $01
  RET C
  XOR A
  CP E
  INC A
  RET NC
  DEC D
  DEC E
  LD A,(BC)
  INC BC
  CP (HL)
  INC HL
  JR Z,CSLOOP
  CCF
  JP SIGNS

; Routine at 13878
__STR_S:
  CALL GETPARM_2BYTE_0
  CALL NUMASC
  CALL CRTST
  CALL GSTRCU

; Save string in string area
SAVSTR:
  LD BC,TOPOOL
  PUSH BC
; This entry point is used by the routine at L2615.
SAVSTR_0:
  LD A,(HL)
  INC HL
  INC HL
  PUSH HL
  CALL TESTR
  POP HL
  LD C,(HL)
  INC HL
  LD B,(HL)
  CALL CRTMST
  PUSH HL
  LD L,A
  CALL TOSTRA
  POP DE
  RET
; This entry point is used by the routine at __CHR_S.
SAVSTR_1:
  LD A,$01

; Make temporary string
;
; Used by the routine at CONCAT.
MKTMST:
  CALL TESTR

; Create temporary string entry
;
; Used by the routines at SAVSTR, DTSTR and __LEFT_S.
CRTMST:
  LD HL,DSCTMP
  PUSH HL
  LD (HL),A
  INC HL
; This entry point is used by the routine at L2A38.
CRTMST_0:
  INC HL
  LD (HL),E
  INC HL
  LD (HL),D
  POP HL
  RET

; Create String
;
; Used by the routines at __PRINT, __STR_S and PRS.
CRTST:
  DEC HL

; Create quote terminated String
;
; Used by the routines at __INPUT and OPRND.
QTSTR:
  LD B,$22
  LD D,B

; Create String, termination char in D
;
; Used by the routine at ITMSEP.
DTSTR:
  PUSH HL
  LD C,$FF
STRGET:
  INC HL
  LD A,(HL)
  INC C
  OR A
  JR Z,STRFIN
  CP D
  JR Z,STRFIN
  CP B
  JR NZ,STRGET
STRFIN:
  CP $22
  CALL Z,_CHRGTB
  EX (SP),HL
  INC HL
  EX DE,HL
  LD A,C
  CALL CRTMST

; Temporary string to pool
;
; Used by the routines at CONCAT, TOPOOL and __LEFT_S.
TSTOPL:
  LD DE,DSCTMP
  LD HL,(TEMPPT)
  LD (FACCU),HL
  LD A,$01
  LD (VALTYP),A
  CALL VMOVE
  RST $20
  LD (TEMPPT),HL
  POP HL
  LD A,(HL)
  RET NZ
  LD DE,$001E
  JP ERROR
PRNUMS:
  INC HL

; Create string entry and print it
;
; Used by the routines at _ASCTFP, _STARTUP, __LOAD, __CSAVE, __CLOAD, L1BA4,
; ERROR, READY, NEXITM, INPBIN, SCCPTR and SHIFT_STOP.
PRS:
  CALL CRTST

; Print string at HL
;
; Used by the routines at __PRINT and L2772.
PRS1:
  CALL GSTRCU
  CALL LOADFP
  INC E
PRS1_0:
  DEC E
  RET Z
  LD A,(BC)
  RST $18
  INC BC
  JR PRS1_0

; Test if enough room for string
;
; Used by the routines at SAVSTR, MKTMST and __LEFT_S.
TESTR:
  OR A
  LD C,$F1
  PUSH AF
  LD HL,(STKTOP)
  EX DE,HL
  LD HL,(FRETOP)
  CPL
  LD C,A
  LD B,$FF
  ADD HL,BC
  INC HL
  RST $20
  JR C,TESTR_0
  LD (FRETOP),HL
  INC HL
  EX DE,HL
  POP AF
  RET
TESTR_0:
  POP AF
  LD DE,$001A
  JP Z,ERROR
  CP A
  PUSH AF
  LD BC,$36BD
  PUSH BC
; This entry point is used by the routine at __FRE.
TESTR_1:
  LD HL,(MEMSIZ)
TESTR_2:
  LD (FRETOP),HL
  LD HL,L0000
  PUSH HL
  LD HL,(STREND)
  PUSH HL
  LD HL,TEMPST
  LD DE,(TEMPPT)
  RST $20
  LD BC,$36F4
  JP NZ,TESTR_6
  LD HL,(VARTAB)
TESTR_3:
  LD DE,(ARYTAB)
  RST $20
  JR Z,TESTR_5
  INC HL
  LD A,(HL)
  INC HL
  OR A
  CALL TESTR_7
  JR TESTR_3
TESTR_4:
  POP BC
TESTR_5:
  LD DE,(STREND)
  RST $20
  JP Z,TESTR_8
  CALL LOADFP
  LD A,D
  PUSH HL
  ADD HL,BC
  OR A
  JP P,TESTR_4
  LD (TEMP8),HL
  POP HL
  LD C,(HL)
  LD B,$00
  ADD HL,BC
  ADD HL,BC
  INC HL
  EX DE,HL
  LD HL,(TEMP8)
  EX DE,HL
  RST $20
  JR Z,TESTR_5
  LD BC,$372F
TESTR_6:
  PUSH BC
  OR $80
TESTR_7:
  LD A,(HL)
  INC HL
  INC HL
  LD E,(HL)
  INC HL
  LD D,(HL)
  INC HL
  RET P
  OR A
  RET Z
  LD B,H
  LD C,L
  LD HL,(FRETOP)
  RST $20
  LD H,B
  LD L,C
  RET C
  POP HL
  EX (SP),HL
  RST $20
  EX (SP),HL
  PUSH HL
  LD H,B
  LD L,C
  RET NC
  POP BC
  POP AF
  POP AF
  PUSH HL
  PUSH DE
  PUSH BC
  RET
TESTR_8:
  POP DE
  POP HL
  LD A,H
  OR L
  RET Z
  DEC HL
  LD B,(HL)
  DEC HL
  LD C,(HL)
  PUSH HL
  DEC HL
  DEC HL
  LD L,(HL)
  LD H,$00
  ADD HL,BC
  LD D,B
  LD E,C
  DEC HL
  LD B,H
  LD C,L
  LD HL,(FRETOP)
  CALL MOVSTR
  POP HL
  LD (HL),C
  INC HL
  LD (HL),B
  LD H,B
  LD L,C
  DEC HL
  JP TESTR_2

; String concatenation
;
; Used by the routine at EVAL3.
CONCAT:
  PUSH BC
  PUSH HL
  LD HL,(FACCU)
  EX (SP),HL
  CALL OPRND
  EX (SP),HL
  CALL TSTSTR
  LD A,(HL)
  PUSH HL
  LD HL,(FACCU)
  PUSH HL
  ADD A,(HL)
  LD DE,$001C
  JP C,ERROR
  CALL MKTMST
  POP DE
  CALL GSTRDE
  EX (SP),HL
  CALL GSTRHL
  PUSH HL
  LD HL,($499D)
  EX DE,HL
  CALL SSTSA
  CALL SSTSA
  LD HL,EVAL2
  EX (SP),HL
  PUSH HL
  JP TSTOPL

; Move string on stack to string area
;
; Used by the routine at CONCAT.
SSTSA:
  POP HL
  EX (SP),HL
  LD A,(HL)
  INC HL
  INC HL
  LD C,(HL)
  INC HL
  LD B,(HL)
  LD L,A

; Move string in BC, (len in L) to string area
;
; Used by the routines at SAVSTR and __LEFT_S.
TOSTRA:
  INC L

; TOSTRA loop
TSALP:
  DEC L
  RET Z
  LD A,(BC)
  LD (DE),A
  INC BC
  INC DE
  JR TSALP

; Get string pointed by FPREG 'Type Error' if it is not
;
; Used by the routine at GETLEN.
GETSTR:
  CALL TSTSTR

; Get string pointed by FPREG
;
; Used by the routines at __PLAY, STRCMP, __STR_S, PRS1 and __FRE.
GSTRCU:
  LD HL,(FACCU)

; Get string pointed by HL
;
; Used by the routine at CONCAT.
GSTRHL:
  EX DE,HL

; Get string pointed by DE
;
; Used by the routines at STRCMP, CONCAT and __LEFT_S.
GSTRDE:
  CALL BAKTMP
  EX DE,HL
  RET NZ
  PUSH DE
  LD D,B
  LD E,C
  DEC DE
  LD C,(HL)
  LD HL,(FRETOP)
  RST $20
  JR NZ,GSTRDE_0
  LD B,A
  ADD HL,BC
  LD (FRETOP),HL
GSTRDE_0:
  POP HL
  RET

; Back to last tmp-str entry
;
; Used by the routines at L2615 and GSTRDE.
BAKTMP:
  LD HL,(TEMPPT)
  DEC HL
  LD B,(HL)
  DEC HL
  LD C,(HL)
  DEC HL
  DEC HL
  RST $20
  RET NZ
  LD (TEMPPT),HL
  RET

; Routine at 14331
__LEN:
  LD BC,USR_RET
  PUSH BC

; Routine at 14335
;
; Used by the routines at __ASC and __VAL.
GETLEN:
  CALL GETSTR
  XOR A
  LD D,A
  LD (VALTYP),A
  LD A,(HL)
  OR A
  RET

; Routine at 14346
__ASC:
  LD BC,USR_RET
  PUSH BC
; This entry point is used by the routine at L1CC0.
__ASC_0:
  CALL GETLEN
  JP Z,FC_ERR
  INC HL
  INC HL
  LD E,(HL)
  INC HL
  LD D,(HL)
  LD A,(DE)
  RET

; Routine at 14363
__CHR_S:
  CALL SAVSTR_1
  CALL MAKINT
  LD HL,($499D)
  LD (HL),E

; Save in string pool
TOPOOL:
  POP BC
  JP TSTOPL

; Routine at 14377
__LEFT_S:
  CALL LFRGNM
  XOR A
; This entry point is used by the routine at __RIGHT_S.
__LEFT_S_0:
  EX (SP),HL
  LD C,A
  PUSH HL
  LD A,(HL)
  CP B
  JR C,$3836
  LD A,B
  LD DE,$000E
  PUSH BC
  CALL TESTR
  POP BC
  POP HL
  PUSH HL
  INC HL
  INC HL
  LD B,(HL)
  INC HL
  LD H,(HL)
  LD L,B
  LD B,$00
  ADD HL,BC
  LD B,H
  LD C,L
  CALL CRTMST
  LD L,A
  CALL TOSTRA
  POP DE
  CALL GSTRDE
  JP TSTOPL

; Routine at 14424
__RIGHT_S:
  CALL LFRGNM
  POP DE
  PUSH DE
  LD A,(DE)
  SUB B
  JR __LEFT_S_0

; Routine at 14433
__MID_S:
  EX DE,HL
  LD A,(HL)
  CALL MIDNUM
  INC B
  DEC B
  JP Z,FC_ERR
  PUSH BC
  LD E,$FF
  CP $29
  JP Z,L3878

; Routine at 14451
L3873:
  RST $08

; Data block at 14452
L3874:
  DEFB $2C

; Routine at 14453
L3875:
  CALL GETINT

; Routine at 14456
;
; Used by the routine at __MID_S.
L3878:
  RST $08

; Data block at 14457
L3879:
  DEFB $29

; Routine at 14458
L387A:
  POP AF
  EX (SP),HL
  LD BC,$382F
  PUSH BC
  DEC A
  CP (HL)
  LD B,$00
  RET NC
  LD C,A
  LD A,(HL)
  SUB C
  CP E
  LD B,A
  RET C
  LD B,E
  RET

; Routine at 14477
__VAL:
  CALL GETLEN
  JP Z,ZERO
  LD E,A
  INC HL
  INC HL
  LD A,(HL)
  INC HL
  LD H,(HL)
  LD L,A
  PUSH HL
  ADD HL,DE
  LD B,(HL)
  LD (HL),D
  EX (SP),HL
  PUSH BC
  DEC HL
  RST $10
  CALL H_ASCTFP
  POP BC
  POP HL
  LD (HL),B
  RET

; number in program listing and check for ending ')'
;
; Used by the routines at __LEFT_S and __RIGHT_S.
LFRGNM:
  EX DE,HL

; Routine at 14506
L38AA:
  RST $08

; Data block at 14507
L38AB:
  DEFB $29

; Routine at 14508
;
; Used by the routine at __MID_S.
MIDNUM:
  POP BC
  POP DE
  PUSH BC
  LD B,E
  RET

; Routine at 14513
__FRE:
  LD HL,(STREND)
  EX DE,HL
  LD HL,L0000
  ADD HL,SP
  LD A,(VALTYP)
  OR A
  JP Z,NOT_0
  CALL GSTRCU
  CALL TESTR_1
  LD DE,(STKTOP)
  LD HL,(FRETOP)
  JP NOT_0

; a.k.a. DIMCON, Return from 'DIM' command
DIMRET:
  DEC HL
  RST $10
  RET Z

; Routine at 14547
L38D3:
  RST $08

; Data block at 14548
L38D4:
  DEFB $2C

; Routine at 14549
__DIM:
  LD BC,DIMRET
  PUSH BC
  OR $AF

; Get variable address to DE
;
; Used by the routines at L1CC0, __LET, GTVLUS, EVAL_VARIABLE, __DEF, __CONT
; and __NEXT.
GETVAR:
  XOR A
  LD (DIMFLG),A
  LD C,(HL)
; This entry point is used by the routine at L2A97.
GETVAR_0:
  CALL __CONT_0
  JP C,SN_ERR
  XOR A
  LD B,A
  LD (VALTYP),A
  RST $10
  JR C,GETVAR_1
  CALL ISLETTER_A
  JR C,GETVAR_3
GETVAR_1:
  LD B,A
GETVAR_2:
  RST $10
  JR C,GETVAR_2
  CALL ISLETTER_A
  JR NC,GETVAR_2
GETVAR_3:
  SUB $24
  JR NZ,GETVAR_4
  INC A
  LD (VALTYP),A
  RRCA
  ADD A,B
  LD B,A
  RST $10
GETVAR_4:
  LD A,(SUBFLG)
  DEC A
  JP Z,ARLDSV
  JP P,GETVAR_5
  LD A,(HL)
  SUB $28
  JP Z,SBSCPT
GETVAR_5:
  XOR A
  LD (SUBFLG),A
  PUSH HL
  LD D,B
  LD E,C
  LD HL,(PRMNAM)
  RST $20
  LD DE,PRMVAL
  JP Z,POPHLRT
  LD HL,(ARYTAB)
  EX DE,HL
  LD HL,(VARTAB)
GETVAR_6:
  RST $20
  JP Z,GETVAR_8
  LD A,C
  SUB (HL)
  INC HL
  JP NZ,GETVAR_7
  LD A,B
  SUB (HL)
GETVAR_7:
  INC HL
  JP Z,GETVAR_10
  INC HL
  INC HL
  INC HL
  INC HL
  JP GETVAR_6
GETVAR_8:
  POP HL
  EX (SP),HL
  PUSH DE
  LD DE,$2927
  RST $20
  POP DE
  JP Z,GETVAR_11
  EX (SP),HL
  PUSH HL
  PUSH BC
  LD BC,$0006
  LD HL,(STREND)
  PUSH HL
  ADD HL,BC
  POP BC
  PUSH HL
  CALL MOVUP
  POP HL
  LD (STREND),HL
  LD H,B
  LD L,C
  LD (ARYTAB),HL
GETVAR_9:
  DEC HL
  LD (HL),$00
  RST $20
  JR NZ,GETVAR_9
  POP DE
  LD (HL),E
  INC HL
  LD (HL),D
  INC HL
GETVAR_10:
  EX DE,HL
  POP HL
  RET
GETVAR_11:
  LD (FPEXP),A
  LD HL,$2213
  LD (FACCU),HL
  POP HL
  RET

; Sort out subscript
;
; Used by the routine at GETVAR.
SBSCPT:
  PUSH HL
  LD HL,(DIMFLG)
  EX (SP),HL
  LD D,A

; SBSCPT loop
SCPTLP:
  PUSH DE
  PUSH BC
  CALL _CHRGTB_2
  POP BC
  POP AF
  EX DE,HL
  EX (SP),HL
  PUSH HL
  EX DE,HL
  INC A
  LD D,A
  LD A,(HL)
  CP $2C
; This entry point is used by the routine at JPVEC_FDIVC.
SCPTLP_0:
  JP Z,SCPTLP

; Routine at 14748
DOCHRT:
  RST $08

; Data block at 14749
L399D:
  DEFB $29

; Routine at 14750
SUBSOK:
  LD (NXTOPR),HL
  POP HL
  LD (DIMFLG),HL
  LD E,$00
  PUSH DE

; "LD DE,nn", OVER THE NEXT TWO BYTES
L39A8:
  DEFB $11

; a.k.a. ERSFIN
;
; Used by the routine at GETVAR.
ARLDSV:
  PUSH HL
  PUSH AF
  LD HL,(ARYTAB)

; "LD A,n" AROUND THE NEXT BYTE
L39AE:
  DEFB $3E

; Routine at 14767
FNDARY:
  ADD HL,DE
  LD DE,(STREND)
  RST $20
  JR Z,CREARY
  LD A,(HL)
  INC HL
  CP C
  JR NZ,NXTARY
  LD A,(HL)
  CP B
NXTARY:
  INC HL
  LD E,(HL)
  INC HL
  LD D,(HL)
  INC HL
  JR NZ,FNDARY
  LD A,(DIMFLG)
  OR A
  JP NZ,DD_ERR
  POP AF
  LD B,H
  LD C,L
  JP Z,POPHLRT
  SUB (HL)
  JP Z,FINDEL

; entry for '?BS ERROR'
;
; Used by the routines at MLDEBC and INLPNM.
BS_ERR:
  LD DE,CHRGTB
  JP ERROR
; This entry point is used by the routine at FNDARY.
CREARY:
  LD DE,$0004
  POP AF
  JP Z,FC_ERR
  LD (HL),C
  INC HL
  LD (HL),B
  INC HL
  LD C,A
  CALL CHKSTK
  INC HL
  INC HL
  LD (TEMP3),HL
  LD (HL),C
  INC HL
  LD A,(DIMFLG)
  RLA
  LD A,C
GETSIZ:
  LD BC,$000B
  JR NC,DEFSIZ
  POP BC
  INC BC
DEFSIZ:
  LD (HL),C
  PUSH AF
  INC HL
  LD (HL),B
  INC HL
  PUSH HL
  CALL MLDEBC
  EX DE,HL
  POP HL
  POP AF
  DEC A
  JR NZ,GETSIZ
  PUSH AF
  LD B,D
  LD C,E
  EX DE,HL
  ADD HL,DE
  JP C,OM_ERR
  CALL L2EC4
  LD (STREND),HL
ZERARY:
  DEC HL
  LD (HL),$00
  RST $20
  JR NZ,ZERARY
  INC BC
  LD D,A
  LD HL,(TEMP3)
  LD E,(HL)
  EX DE,HL
  ADD HL,HL
  ADD HL,BC
  EX DE,HL
  DEC HL
  DEC HL
  LD (HL),E
  INC HL
  LD (HL),D
  INC HL
  POP AF
  JR C,ENDDIM
; This entry point is used by the routine at FNDARY.
FINDEL:
  LD B,A
  LD C,A
  LD A,(HL)
  INC HL

; "LD D,n" to skip "POP HL"
L3A38:
  DEFB $16

; Routine at 14905
INLPNM:
  POP HL
  LD E,(HL)
  INC HL
  LD D,(HL)
  INC HL
  EX (SP),HL
  PUSH AF
  RST $20
  JP NC,BS_ERR
  PUSH HL
  CALL MLDEBC
  POP DE
  ADD HL,DE
  POP AF
  DEC A
  LD B,H
  LD C,L
  JR NZ,INLPNM
  ADD HL,HL
  ADD HL,HL
  POP BC
  ADD HL,BC
  EX DE,HL
; This entry point is used by the routine at BS_ERR.
ENDDIM:
  LD HL,(NXTOPR)
  RET
; This entry point is used by the routine at L1BA4.
INLPNM_0:
  LD A,$02
  OUT ($AF),A
  DI
INLPNM_1:
  PUSH BC
  LD BC,L0000
INLPNM_2:
  EX (SP),HL
  EX (SP),HL
  NOP
  DEC C
  JR NZ,INLPNM_2
  DJNZ INLPNM_2
  POP BC
  DJNZ INLPNM_1
; This entry point is used by the routine at __CSAVE.
INLPNM_3:
  PUSH BC
  PUSH AF
  LD BC,L0000
INLPNM_4:
  DEC BC
  LD A,B
  OR C
  JR NZ,INLPNM_4
  POP AF
  POP BC
; This entry point is used by the routine at L1CC0.
INLPNM_5:
  PUSH AF
  LD A,$00
  OUT ($AF),A
  POP AF
  EI
  RET
; This entry point is used by the routine at L1CC0.
INLPNM_6:
  OR A
  PUSH AF
  LD A,$02
  OUT ($AF),A
  LD HL,L0000
INLPNM_7:
  DEC HL
  LD A,H
  OR L
  JR NZ,INLPNM_7
  POP AF
; This entry point is used by the routine at L1CC0.
INLPNM_8:
  LD A,($482A)
  JR Z,INLPNM_9
  ADD A,A
  ADD A,A
INLPNM_9:
  LD B,A
  LD C,$00
  DI
INLPNM_10:
  CALL INLPNM_16
  CALL INLPNM_14
  DEC BC
  LD A,B
  OR C
  JR NZ,INLPNM_10
  JP ISCNTC_0
; This entry point is used by the routine at L1CC0.
INLPNM_11:
  LD HL,($4826)
  PUSH AF
  LD A,L
  SUB $0E
  LD L,A
  CALL INLPNM_17
  POP AF
  LD B,$08
INLPNM_12:
  RRCA
  CALL C,INLPNM_15
  CALL NC,INLPNM_13
  DJNZ INLPNM_12
  CALL INLPNM_15
  CALL INLPNM_15
  JP ISCNTC_0
INLPNM_13:
  LD HL,($4826)
  CALL INLPNM_17
INLPNM_14:
  RET
INLPNM_15:
  CALL INLPNM_16
  EX (SP),HL
  EX (SP),HL
  NOP
  NOP
  NOP
  NOP
  CALL INLPNM_16
  RET
INLPNM_16:
  LD HL,($4828)
INLPNM_17:
  PUSH AF
INLPNM_18:
  DEC L
  JP NZ,INLPNM_18
  LD A,$03
  OUT ($AF),A
INLPNM_19:
  DEC H
  JP NZ,INLPNM_19
  LD A,$02
  OUT ($AF),A
  POP AF
  RET
; This entry point is used by the routines at __LOAD, __CLOAD and L1CC0.
INLPNM_20:
  LD A,$02
  OUT ($AF),A
  DI
INLPNM_21:
  LD HL,$0457
INLPNM_22:
  LD D,C
  CALL INLPNM_38
  RET C
  LD A,C
  CP $DE
  JR NC,INLPNM_21
  CP $05
  JR C,INLPNM_21
  SUB D
  JR NC,INLPNM_23
  CPL
  INC A
INLPNM_23:
  CP $04
  JR NC,INLPNM_21
  DEC HL
  LD A,H
  OR L
  JR NZ,INLPNM_22
  LD HL,L0000
  LD B,L
  LD D,L
INLPNM_24:
  CALL INLPNM_38
  RET C
  ADD HL,BC
  DEC D
  JP NZ,INLPNM_24
  LD BC,$05FA
  ADD HL,BC
  LD A,H
  RRA
  AND $7F
  LD D,A
  ADD HL,HL
  LD A,H
  SUB D
  LD D,A
  SUB $05
  LD (LOWLIM),A
  LD A,D
  ADD A,A
  LD B,$00
INLPNM_25:
  SUB $03
  INC B
  JR NC,INLPNM_25
  LD A,B
  SUB $03
  LD (WINWID),A
  OR A
  RET
; This entry point is used by the routine at L1CC0.
INLPNM_26:
  LD A,(LOWLIM)
  LD D,A
INLPNM_27:
  CALL ISCNTC_0
  RET C
  IN A,($AF)
  RLCA
  JR NC,INLPNM_27
INLPNM_28:
  CALL ISCNTC_0
  RET C
  IN A,($AF)
  RLCA
  JR C,INLPNM_28
  LD E,$00
  CALL INLPNM_34
INLPNM_29:
  LD B,C
  CALL INLPNM_34
  RET C
  LD A,B
  ADD A,C
  JR C,INLPNM_29
  CP D
  JR C,INLPNM_29
  LD L,$08
INLPNM_30:
  CALL INLPNM_31
  CP $04
  CCF
  RET C
  CP $02
  CCF
  RR D
  LD A,C
  RRCA
  CALL NC,INLPNM_35
  CALL INLPNM_34
  DEC L
  JP NZ,INLPNM_30
  CALL ISCNTC_0
  LD A,D
  RET
INLPNM_31:
  LD A,(WINWID)
  LD B,A
  LD C,$00
INLPNM_32:
  IN A,($AF)
  XOR E
  JP P,INLPNM_33
  LD A,E
  CPL
  LD E,A
  INC C
  DJNZ INLPNM_32
  LD A,C
  RET
INLPNM_33:
  NOP
  NOP
  NOP
  NOP
  DJNZ INLPNM_32
  LD A,C
  RET
INLPNM_34:
  CALL ISCNTC_0
  RET C
INLPNM_35:
  LD C,$00
INLPNM_36:
  INC C
  JR Z,INLPNM_37
  IN A,($AF)
  XOR E
  JP P,INLPNM_36
  LD A,E
  CPL
  LD E,A
  RET
INLPNM_37:
  DEC C
  RET
INLPNM_38:
  CALL ISCNTC_0
  RET C
  IN A,($AF)
  RLCA
  JR C,INLPNM_38
  LD E,$00
  CALL INLPNM_35
  JP INLPNM_36
; This entry point is used by the routine at CHRGTB.
_OUTDO:
  CALL OUTHK
  PUSH AF
  LD A,(PRTFLG)
  OR A
  JR Z,_OUTCON
  DEC A
  JR Z,OUTC_TABEXP
  POP AF
  JP L1CC0_19
OUTC_TABEXP:
  LD A,(RAWPRT)
  CP $FF
  JR Z,NO_TAB_1
  POP AF
  PUSH AF
  CP $09
  JR NZ,NO_TAB
TABEXP_LOOP:
  LD A,$20
  RST $18
  LD A,(LPTPOS)
  AND $07
  JR NZ,TABEXP_LOOP
  POP AF
  RET

; Routine at 15354
;
; Used by the routine at INLPNM.
NO_TAB:
  SUB $0D
  JR Z,NO_TAB_0
  JR C,NO_TAB_1
  CP $13
  JR C,NO_TAB_1
  LD A,(LPTPOS)
  CP $84
  CALL Z,OUTDO_CRLF_0
  INC A
NO_TAB_0:
  LD (LPTPOS),A
; This entry point is used by the routine at INLPNM.
NO_TAB_1:
  POP AF
  JP OUTPRT_CHR

; Disable printer echo if enabled
;
; Used by the routines at READY, L2EC4 and INPBRK.
FINLPT:
  LD A,$01
  LD (PRTFLG),A
  LD A,(PRTSTT)
  BIT 7,A
  RES 7,A
  LD (PRTSTT),A
  JR NZ,FINLPT_0
  LD A,(LPTPOS)
  OR A
FINLPT_0:
  CALL NZ,OUTDO_CRLF_0
  LD A,(PICFLG)
  AND $40
  LD (PICFLG),A
  XOR A
  LD (PRTFLG),A
  LD (PRTSTT),A
  RET
; This entry point is used by the routine at INLPNM.
_OUTCON:
  POP AF
  JP _CHPUT
; This entry point is used by the routines at ERROR, READY, INPBRK and SCCPTR.
CONSOLE_CRLF:
  CALL L33CD_41
  LD A,(CURPOS)
  DEC A
  RET Z
  JR OUTDO_CRLF_2
; This entry point is used by the routine at SHIFT_STOP.
FINLPT_1:
  DEC HL
  LD A,(HL)
  CP $20
  JR Z,FINLPT_1
  INC HL
; This entry point is used by the routine at SHIFT_STOP.
FINLPT_2:
  XOR A
  LD (HL),A
  LD HL,BUFMIN
  RET

; a.k.a. CRDO
;
; Used by the routines at _STARTUP, __LOAD, L1BA4, __PRINT and __LIST.
OUTDO_CRLF:
  CALL CRDHK
  LD A,(PRTFLG)
  OR A
  JR Z,OUTDO_CRLF_2
  DEC A
  JR NZ,OUTDO_CRLF_1
; This entry point is used by the routines at NO_TAB and FINLPT.
OUTDO_CRLF_0:
  LD A,$0D
  RST $18
  LD A,$0A
  RST $18
  XOR A
  LD (LPTPOS),A
  RET
OUTDO_CRLF_1:
  LD A,$0D
  RST $18
  RET
; This entry point is used by the routine at FINLPT.
OUTDO_CRLF_2:
  LD A,$03
  RST $18
  XOR A
  RET

; Check STOP key status
;
; Used by the routines at MUSIC, L24BD and __LIST.
ISCNTC:
  CALL SHIFT_STOP
  JP __STOP
; This entry point is used by the routine at INLPNM.
ISCNTC_0:
  EX (SP),HL
  EX (SP),HL

; scans SHIFT / STOP keys
;
; Used by the routines at _SHIFT_STOP and ISCNTC.
SHIFT_STOP:
  IN A,($81)
  AND $01
  RET NZ
  IN A,($80)
  AND $04
  RET NZ
  LD A,($4889)
  OR A
  RET NZ
  LD (IX+$05),$F2
  LD (IX+$07),$00
  LD (IX+$06),$00
  SCF
  RET
; This entry point is used by the routine at L2772.
SHIFT_STOP_0:
  LD A,$3F
  RST $18
  LD A,$20
  RST $18
; This entry point is used by the routine at PROMPT.
SHIFT_STOP_1:
  CALL PINLIN
  LD HL,BUFFER
; This entry point is used by the routine at __NEW.
SHIFT_STOP_2:
  LD A,(GETFLG)
  OR A
  JR Z,SHIFT_STOP_3
  CALL L1CC0_36
  JR NZ,SHIFT_STOP_4
  JR FINLPT_2
SHIFT_STOP_3:
  CALL T_EDIT
  OR A
  JR NZ,SHIFT_STOP_4
  CALL SHIFT_STOP
  JR NC,SHIFT_STOP_3
  RET
SHIFT_STOP_4:
  CP $0D
  JR Z,FINLPT_1
  RES 7,A
  LD (HL),A
  LD DE,ENDBUF-1
  RST $20
  JP __NEW_0
; This entry point is used by the routine at __NEW.
SHIFT_STOP_5:
  INC HL
  JR SHIFT_STOP_2
; This entry point is used by the routine at NO_TAB.
OUTPRT_CHR:
  PUSH BC
  PUSH DE
  PUSH HL
  PUSH AF
  CALL KBDSCAN
  CP $D8
  CALL Z,SHIFT_STOP_18
  LD A,(PRTSTT)
  BIT 6,A
  JR NZ,SHIFT_STOP_6
  CALL SHIFT_STOP_8
SHIFT_STOP_6:
  CALL SHIFT_STOP_10
  JR C,SHIFT_STOP_7
  POP AF
  PUSH AF
  CALL SHIFT_STOP_19
  DI
  CALL SHIFT_STOP_21
  EI
SHIFT_STOP_7:
  POP AF
  POP HL
  POP DE
  POP BC
  RET
SHIFT_STOP_8:
  SET 6,A
  LD (PRTSTT),A
  PUSH AF
  LD HL,(PRTINT)
SHIFT_STOP_9:
  LD A,(HL)
  OR A
  CALL NZ,OUTPRT_CHR
  INC HL
  JR NZ,SHIFT_STOP_9
  POP AF
  RET
SHIFT_STOP_10:
  RLA
  RET C
  RRA
  AND $F0
  LD (PRTSTT),A
SHIFT_STOP_11:
  LD HL,$1B58
SHIFT_STOP_12:
  IN A,($10)
  AND $02
  RET Z
  DEC HL
  XOR A
SHIFT_STOP_13:
  DEC A
  OR A
  JR NZ,SHIFT_STOP_13
  LD A,H
  OR L
  JR NZ,SHIFT_STOP_12
  XOR A
  LD (PRTFLG),A
  LD B,$05
SHIFT_STOP_14:
  LD A,$0E
  RST $18
  DJNZ SHIFT_STOP_14
  LD HL,PRTSTT
  SET 0,(HL)
  LD A,(PRTCOM)
  RRA
  JR C,SHIFT_STOP_16
  LD HL,PRINTER_MSG_FR
  LD A,(FRGFLG)
  OR A
  JR Z,SHIFT_STOP_15
  LD HL,PRINTER_MSG
SHIFT_STOP_15:
  CALL PRS
SHIFT_STOP_16:
  LD A,$01
  LD (PRTFLG),A
SHIFT_STOP_17:
  CALL KBDSCAN
  OR A
  JR Z,SHIFT_STOP_17
  CP $D8
  JR NZ,SHIFT_STOP_11
SHIFT_STOP_18:
  CALL KBDSCAN
  CP $D8
  JR Z,SHIFT_STOP_18
  LD HL,PRTSTT
  LD A,$80
  XOR (HL)
  LD (HL),A
  RLA
  RET
SHIFT_STOP_19:
  PUSH AF
  LD A,(RAWPRT)
  OR A
  JR NZ,SHIFT_STOP_20
  LD HL,(PRTXLT)
  LD A,H
  OR L
  JR Z,SHIFT_STOP_20
  POP AF
  ADD A,L
  LD L,A
  LD A,$00
  ADC A,H
  LD H,A
  LD A,(HL)
  PUSH AF
SHIFT_STOP_20:
  POP AF
  RET
SHIFT_STOP_21:
  OUT ($11),A
  LD A,$00
  OUT ($10),A
  LD A,$01
  OUT ($10),A
  RET
; This entry point is used by the routine at L2EC4.
SHIFT_STOP_22:
  LD (AUTFLG),A
  LD (HL),A
  INC HL
  LD (HL),A
  JP L2EC4_2
; This entry point is used by the routine at PROMPT.
SHIFT_STOP_23:
  PUSH HL
  CALL _ASCTFP_12
  POP DE
  CALL SRCHLN
  JP NC,PROMPT_0
  LD A,$A0
  JP PROMPT_1
; This entry point is used by the routine at PROMPT.
SHIFT_STOP_24:
  JR C,SHIFT_STOP_25
  PUSH DE
  LD DE,$FFF9
  RST $20
  POP DE
  JR NC,SHIFT_STOP_25
  LD (AUTLIN),HL
  JR SHIFT_STOP_26
SHIFT_STOP_25:
  XOR A
  LD (AUTFLG),A
SHIFT_STOP_26:
  JP PROMPT_3
; This entry point is used by the routine at SCR_CO.
SHIFT_STOP_27:
  LD HL,$35FE
  CALL L0D47_4
  LD A,$20
  LD HL,L35F4
  JP SCR_CO_1

; Routine at 15822
__NEW:
  JP Z,L2EC4_0
  CALL FC_ERR_0
  RET NZ
  LD BC,READY
  PUSH BC
  CALL SRCHLN
  LD H,B
  LD L,C
  JP L2EC4_1
; This entry point is used by the routine at SHIFT_STOP.
__NEW_0:
  JR C,__NEW_1
  JP NZ,SHIFT_STOP_2
  LD A,$0E
  RST $18
__NEW_1:
  JP SHIFT_STOP_5

; Get INPUT with prompt, HL = resulting text
;
; Used by the routines at L2772 and GTVLUS.
QINLIN:
  LD A,(PICFLG)
  SET 5,A
  LD (PICFLG),A
  SET 6,(IX+$03)
  JP L2772_0
; This entry point is used by the routine at T_EDIT.
QINLIN_0:
  CP $06
  JR NZ,QINLIN_2
  LD A,(PICFLG)
  BIT 5,A
  JP L33CD_18
QINLIN_1:
  JP T_EDIT_2
QINLIN_2:
  CP $1F
  JR NZ,QINLIN_1
  JP T_EDIT_1
; This entry point is used by the routine at T_EDIT.
QINLIN_3:
  LD A,(PICFLG)
  BIT 5,A
  JP NZ,T_EDIT_13
  LD A,$06
  RST $18
  JP T_EDIT_15
; This entry point is used by the routine at L33CD.
QINLIN_4:
  JP M,L33CD_30
  LD A,(PICFLG)
  BIT 5,A
  JP NZ,L33CD_30
  JP L33CD_29
; This entry point is used by the routine at T_EDIT.
QINLIN_5:
  JP Z,T_EDIT_24
  PUSH AF
  LD A,($4889)
  OR A
  JR NZ,QINLIN_6
  POP AF
  JP T_EDIT_0
QINLIN_6:
  POP AF
  JP QINLIN_0

; Data block at 15935
L3E3F:
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
  DEFB $FF

; Video screen map of 25*80 bytes
SCREEN:
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00

; Maskable interrupt hook
;
; Used by the routine at VSIGN.
INT_SUB:
  NOP
  NOP
  NOP

; Call routine vector hook
;
; Used by the routine at __CALL.
CALHK:
  NOP
  NOP
  NOP

; Sound generator hook
;
; Used by the routine at __SOUND.
SONHK:
  NOP
  NOP
  NOP

; Play routine hook
;
; Used by the routine at __PLAY.
PLYHK:
  NOP
  NOP
  NOP

; Programmable restart "&30" hook
;
; Used by the routine at VSIGN.
RSTHK:
  NOP
  NOP
  NOP

; PRINT command hook
;
; Used by the routine at __PRINT.
PRTHK:
  NOP
  NOP
  NOP

; OUTDO command hook
;
; Used by the routine at INLPNM.
OUTHK:
  NOP
  NOP
  NOP

; CRDO command hook
;
; Used by the routine at OUTDO_CRLF.
CRDHK:
  NOP
  NOP
  NOP

; a.k.a. RINPUT, Line input
;
; Used by the routine at SHIFT_STOP.
PINLIN:
  NOP
  NOP
  NOP

; INPUT command hook
;
; Used by the routine at __INPUT.
INPHK:
  NOP
  NOP
  NOP

; Non maskable interrupt hook
;
; Used by the routine at _DISPLAY.
NMIHK:
  NOP
NMIADDR:
  NOP
  NOP

; Routine at 18417
__LPEN:
  NOP
  NOP
  NOP

; Routine at 18420
__DISK:
  NOP
  NOP
  NOP

; Routine at 18423
__MODEM:
  NOP
  NOP
  NOP

; interrupt divider counter for screen update
INTDIV:
  DEFB $00

; flag to indicate if screen update is required
INTACT:
  DEFB $00

; interrupt divider reload value (i.e. rate)
INTRAT:
  DEFB $00

; cursor params and border color (MAT reg 9345)
CURSOR:
  DEFB $00

; keyboard lock key status
FKLOCK:
  DEFB $00

; most recently scanned keyboard value
CRCHAR:
  DEFB $00

; key auto repeat timer (every 20ms)
REPTIM:
  DEFB $00

; key auto repeat flags
REPENA:
  DEFB $00

; foreground attribute for character to be printed: bit 0-2 Fore, 3 Flash, 4
; Dheight, 5 Dwidth, 6 revrs, 4-6 back(gfx), 7 gfx mode
ATTRCAR:
  DEFB $00

; background attribute used by INIT command: bit 0-2 Fore, 3=0, 4-6
; back(alphanumeric), 7=1
ATTBAK:
  DEFB $00

; flag to indicate extension characted print mode: bit7 = 0 for ROM, 1 for RAM
EXTENF:
  DEFB $00
CURPOS:
  DEFB $00

; vertical pos of cursor
YCURSO:
  DEFB $00

; previous line number to one just identified
PRELIN:
  DEFB $00,$00,$00,$00

; address of line currently being sent to BASIC
RETADR:
  DEFB $00,$00

; flag to indicate line being sent to BASIC
ENTSTT:
  DEFB $00,$00,$00

; file table for cassette files
FILETAB:
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00

; cassette calibration
LOWLIM:
  DEFB $00

; cassette calibration
WINWID:
  DEFB $00

; jump vector to warm start address
RAMLOW:
  DEFB $00,$00,$00

; Routine at 18483
__USR:
  NOP
  NOP
  NOP

; FP divisio subroutine work area
FDIVC:
  DEFB $00

; Data block at 18487
DIV1:
  DEFB $00,$00,$00,$00

; Data block at 18491
DIV2:
  DEFB $00,$00,$00,$00

; Data block at 18495
DIV3:
  DEFB $00,$00,$00

; Data block at 18498
DIV4:
  DEFB $00,$00

; Data block at 18500
;
; Seed for RND numbers
SEED:
  DEFB $00

; BASIC random number generator tables
RNDCNT:
  DEFB $00

; Data block at 18502
SEED+2:
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00
LSTRND2:
  DEFB $00,$00,$00,$00,$00

; current line printer column number
LPTPOS:
  DEFB $00

; character output stream flag: 0=terminal, 1=printer, $FF=cassette
PRTFLG:
  DEFB $00

; character input stream flag:  0=terminal, 1=printer
GETFLG:
  DEFB $00

; picture display control flags: 7=scroll off, 6=page end, 5=INPUT mode,
; 2=CONT, 1=BASIC editor, 0=CONT char required
PICFLG:
  DEFB $00

; cassette communication flag status bits
CASCOM:
  DEFB $00

; printer description flag; 0=MSX, 1=user, $ff=no controls
RAWPRT:
  DEFB $00

; printer status flag: 7= user off (PRT key toggle), 7=initialized, 0=busy
PRTSTT:
  DEFB $00

; printer communications flag (set by user)
PRTCOM:
  DEFB $00

; address of printer init string vector
PRTINT:
  DEFB $00,$00

; address of printer translation table
PRTXLT:
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00

; AUTO mode flag
AUTFLG:
  DEFB $00

; Current line number for auto
AUTLIN:
  DEFW L0000

; Increment for auto
AUTINC:
  DEFW L0000
  DEFW L0000

; line length
LINLEN:
  DEFB $00

; position of last comma column
CLMLST:
  DEFB $00

; current executing BASIC line number
CURLIN:
  DEFB $00,$00
TXTTAB:
  DEFB $00,$00

; 0=French messages, 1=international
FRGFLG:
  DEFB $00

; address of keyboard lookup table
KBDTBL:
  DEFB $00,$00

; tempo save in PLAY statement
TMPSAV:
  DEFB $00

; octave save in PLAY statement
OCTSAV:
  DEFB $00

; address of top location used by stack
STKTOP:
  DEFB $00,$00
BUFMIN:
  DEFB $00
BUFFER:
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
ENDBUF-1:
  DEFB $00

; end of crunch buffer
ENDBUF:
  DEFB $00
DIMFLG:
  DEFB $00

; the type indicator, 0=num, 1=string
VALTYP:
  DEFB $00

; flag to disable crunching
DORES:
  DEFB $00

; save the text pointer after constant
CONTXT:
  DEFB $00,$00

; highest location in memory
MEMSIZ:
  DEFB $00,$00

; pointer at first 3 temp descriptor
TEMPPT:
  DEFB $00,$00

; storage for numtmp temp descriptor
TEMPST:
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00

; storage for numtmp temp descriptor
DSCTMP:
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00,$00,$00

; address of top string free space
FRETOP:
  DEFB $00,$00

; address of end of string erase
TEMP3:
  DEFB $00,$00

; used by garbage collection
TEMP8:
  DEFB $00,$00

; saved text pointer of FOR statement
ENDFOR:
  DEFB $00,$00

; data line number
DATLIN:
  DEFB $00,$00

; used in FOR statement
SUBFLG:
  DEFB $00

; was it READ or INPUT?
FLGINP:
  DEFB $00

; temporary for statement code
TEMP:
  DEFB $00,$00

; (aka PTRFRG) used by RENUM statement
PTRFLG:
  DEFB $00

; temporary(2) for statement code
;
; (word) temp. storage used by EVAL, a.k.a. TEMP2
NXTOPR:
  DEFB $00,$00

; old line number
OLDLIN:
  DEFB $00,$00

; old text pointer
OLDTXT:
  DEFB $00,$00

; pointer to start of simple variable space
;
; ; BASIC program end ptr (a.k.a. PROGND, Simple Variables)
VARTAB:
  DEFB $00,$00
ARYTAB:
  DEFB $00,$00
STREND:
  DEFB $00,$00

; pointer to data in DATA statement
DATPTR:
  DEFB $00,$00

; name of parameter that is active
PRMNAM:
  DEFB $00,$00

; value of active parameter
PRMVAL:
  DEFB $00,$00,$00,$00
FACCU:
  DEFB $00,$00
FACCU+2:
  DEFB $00
FPEXP:
  DEFB $00
SGNRES:
  DEFB $00,$00,$00,$00,$00,$00,$00,$00
  DEFB $00,$00,$00,$00,$00,$00

; temp for fmult (a.k.a. FMLTT)
MULVAL:
  DEFB $00

; temp (2) for fmult (a.k.a. FMLTT2)
MULVAL2:
  DEFB $00,$00
