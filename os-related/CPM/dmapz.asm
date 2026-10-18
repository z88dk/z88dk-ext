; DMAPZ.ASM
; CP/M 2.2 drive map.
; Richard Deane, modified by feilipu for z88dk.
;
; zcc +cpm -clib=8080 -cpu=8080 -create-app dmapz.asm -o dmap
;
; The z88dk CP/M CRT sets the stack and calls _main. _main returns
; to that CRT. ESC or Ctrl-C takes the same return.
;
; Mounted drives are A: through P:. Each search FCB names the drive
; (1 = A: through 16 = P:) and asks for extent 0, so each file is
; counted once for users 0 through 15. Counts 1 through 999 are
; printed. A larger count prints as a star. A zero count is blank.

        SECTION code_user

        PUBLIC  _main

BDOS            EQU     0005H
FN_CONIN        EQU     1
FN_CONOUT       EQU     2
FN_PRINT        EQU     9
FN_CONST        EQU     11
FN_USER         EQU     32
FN_SRCHF        EQU     17
FN_SRCHN        EQU     18

; ------------------------------------------------
; Main program
; ------------------------------------------------

_main:
        LD      E,0FFH
        LD      C,FN_USER
        CALL    BDOS
        LD      (SAVED_USER),A

        LD      DE,HEADING
        CALL    PRINT_STRING

        LD      A,0
        LD      (DSK),A

DETECT_LOOP:
        LD      A,(DSK)
        CP      16
        JP      NC,DETECT_DONE

        LD      C,A
        LD      E,0
        CALL    SELDSK2

        LD      A,H
        OR      L
        JP      NZ,DETECT_NEXT

        LD      A,(DSK)
        LD      E,A
        LD      D,0
        LD      HL,DRIVE_LIST
        ADD     HL,DE
        LD      (HL),'*'

DETECT_NEXT:
        LD      A,(DSK)
        INC     A
        LD      (DSK),A
        JP      DETECT_LOOP

DETECT_DONE:
        LD      DE,DRIVE_HEADING
        CALL    PRINT_STRING

        LD      A,0
        LD      (DSK),A

DRIVE_LIST_LOOP:
        LD      A,(DSK)
        CP      16
        JP      NC,DRIVE_LIST_DONE

        LD      E,A
        LD      D,0
        LD      HL,DRIVE_LIST
        ADD     HL,DE
        LD      A,(HL)
        CALL    OUTCHAR

        LD      A,' '
        CALL    OUTCHAR

        LD      A,(DSK)
        INC     A
        LD      (DSK),A
        JP      DRIVE_LIST_LOOP

DRIVE_LIST_DONE:
        LD      DE,CRLF
        CALL    PRINT_STRING
        LD      DE,USER_HEADING
        CALL    PRINT_STRING

        LD      A,0
        LD      (USER),A

USER_HEADING_LOOP:
        LD      A,(USER)
        CP      16
        JP      NC,USER_HEADING_DONE

        CALL    PRINT_DEC

        LD      A,(USER)
        INC     A
        LD      (USER),A
        JP      USER_HEADING_LOOP

USER_HEADING_DONE:
        LD      DE,CRLF
        CALL    PRINT_STRING
        CALL    PRINT_CRLF

        LD      A,1
        LD      (DSK),A

DRIVE_LOOP:
        LD      A,(DSK)
        CP      17
        JP      NC,ALL_DONE

        LD      A,(DSK)
        DEC     A
        LD      E,A
        LD      D,0
        LD      HL,DRIVE_LIST
        ADD     HL,DE
        LD      A,(HL)
        CP      '*'
        JP      Z,NEXT_DRIVE

        LD      A,0
        LD      (USER),A

USER_LOOP:
        CALL    CHECK_QUIT

        LD      A,(USER)
        CP      16
        JP      NC,PRINT_DRIVE_ROW

        LD      E,A
        LD      C,FN_USER
        CALL    BDOS

        XOR     A
        LD      (NUMBER_LO),A
        LD      (NUMBER_HI),A

        LD      A,(DSK)
        LD      (SEARCH_FCB),A
        XOR     A
        LD      HL,SEARCH_FCB+12
        LD      B,4
CLEAR_EXT:
        LD      (HL),A
        INC     HL
        DEC     B
        JP      NZ,CLEAR_EXT

        LD      DE,SEARCH_FCB
        LD      C,FN_SRCHF
        CALL    BDOS

SEARCH_LOOP:
        CP      0FFH
        JP      Z,SEARCH_DONE

        LD      HL,NUMBER_LO
        INC     (HL)
        JP      NZ,SEARCH_NEXT
        LD      HL,NUMBER_HI
        INC     (HL)

SEARCH_NEXT:
        CALL    CHECK_QUIT
        LD      C,FN_SRCHN
        CALL    BDOS
        JP      SEARCH_LOOP

SEARCH_DONE:
        LD      A,(USER)
        ADD     A,A
        LD      E,A
        LD      D,0
        LD      HL,USERCOUNT
        ADD     HL,DE
        LD      A,(NUMBER_LO)
        LD      (HL),A
        INC     HL
        LD      A,(NUMBER_HI)
        LD      (HL),A

        LD      A,(USER)
        INC     A
        LD      (USER),A
        JP      USER_LOOP

PRINT_DRIVE_ROW:
        LD      A,' '
        CALL    OUTCHAR
        LD      A,(DSK)
        ADD     A,'A'-1
        CALL    OUTCHAR
        LD      A,':'
        CALL    OUTCHAR
        LD      A,'>'
        CALL    OUTCHAR

        LD      B,0
PRINT_USER_LOOP:
        LD      A,B
        CP      16
        JP      NC,PRINT_ROW_DONE

        ADD     A,A
        LD      E,A
        LD      D,0
        LD      HL,USERCOUNT
        ADD     HL,DE
        CALL    PRINT_COUNT

        INC     B
        JP      PRINT_USER_LOOP

PRINT_ROW_DONE:
        CALL    PRINT_CRLF

NEXT_DRIVE:
        LD      A,(DSK)
        INC     A
        LD      (DSK),A
        JP      DRIVE_LOOP

ALL_DONE:
        CALL    PRINT_CRLF
        LD      A,(SAVED_USER)
        LD      E,A
        LD      C,FN_USER
        CALL    BDOS
        LD      HL,0
        RET

; ------------------------------------------------
; BIOS SELDSK. C is the drive, 0 = A:. E = 0 is the login select.
; HL = 0 when the drive is absent.
; ------------------------------------------------

SELDSK2:
        LD      HL,(0001H)
        LD      DE,0018H
        ADD     HL,DE
        JP      (HL)

; ------------------------------------------------
; ESC or Ctrl-C returns through ALL_DONE.
; A key pressed during printing is held by the BDOS, so function 11
; is the status that sees it. Function 1 then takes the character.
; ------------------------------------------------

CHECK_QUIT:
        LD      C,FN_CONST
        CALL    BDOS
        OR      A
        RET     Z
        LD      C,FN_CONIN
        CALL    BDOS
        CP      03H
        JP      Z,QUIT_NOW
        CP      1BH
        RET     NZ
QUIT_NOW:
        POP     HL
        JP      ALL_DONE

; ------------------------------------------------
; Print a CP/M string ending in '$'.
; ------------------------------------------------

PRINT_STRING:
        PUSH    BC
        LD      C,FN_PRINT
        CALL    BDOS
        POP     BC
        RET

; ------------------------------------------------
; Print one character in A.
; ------------------------------------------------

OUTCHAR:
        PUSH    AF
        PUSH    BC
        PUSH    DE
        LD      E,A
        LD      C,FN_CONOUT
        CALL    BDOS
        POP     DE
        POP     BC
        POP     AF
        RET

; ------------------------------------------------
; Print A as four characters: two spaces, tens or space, ones.
; Used for the user headings 0 through 15.
; ------------------------------------------------

PRINT_DEC:
        LD      C,A
        LD      B,0
PD_TENS:
        LD      A,C
        CP      10
        JP      C,PD_SHOW
        SUB     10
        LD      C,A
        INC     B
        JP      PD_TENS

PD_SHOW:
        LD      A,' '
        CALL    OUTCHAR
        CALL    OUTCHAR
        LD      A,B
        OR      A
        JP      Z,PD_TENS_SPACE
        ADD     A,'0'
        CALL    OUTCHAR
        JP      PD_ONES
PD_TENS_SPACE:
        LD      A,' '
        CALL    OUTCHAR
PD_ONES:
        LD      A,C
        ADD     A,'0'
        CALL    OUTCHAR
        RET

; ------------------------------------------------
; Print the count at HL in the same four-character column.
; 0 is blank. 1 through 999 are decimal. Above 999 is "   *".
; ------------------------------------------------

PRINT_COUNT:
        PUSH    BC
        LD      E,(HL)
        INC     HL
        LD      D,(HL)

        LD      A,D
        OR      E
        JP      Z,COUNT_BLANK

        LD      A,D
        CP      3
        JP      C,COUNT_FIT
        JP      NZ,COUNT_OVERFLOW
        LD      A,E
        CP      0E8H
        JP      NC,COUNT_OVERFLOW

COUNT_FIT:
        LD      B,0
COUNT_HUND:
        LD      A,D
        OR      A
        JP      NZ,COUNT_HUND_SUB
        LD      A,E
        CP      100
        JP      C,COUNT_TENS
COUNT_HUND_SUB:
        LD      A,E
        SUB     100
        LD      E,A
        LD      A,D
        SBC     A,0
        LD      D,A
        INC     B
        JP      COUNT_HUND

COUNT_TENS:
        LD      C,0
COUNT_TENS_LP:
        LD      A,E
        CP      10
        JP      C,COUNT_SHOW
        SUB     10
        LD      E,A
        INC     C
        JP      COUNT_TENS_LP

COUNT_SHOW:
        LD      A,' '
        CALL    OUTCHAR
        LD      A,B
        OR      A
        JP      Z,COUNT_HUND_SPACE
        ADD     A,'0'
        CALL    OUTCHAR
        JP      COUNT_TENS_DIGIT
COUNT_HUND_SPACE:
        LD      A,' '
        CALL    OUTCHAR
COUNT_TENS_DIGIT:
        LD      A,B
        OR      C
        JP      Z,COUNT_TENS_SPACE
        LD      A,C
        ADD     A,'0'
        CALL    OUTCHAR
        JP      COUNT_ONES
COUNT_TENS_SPACE:
        LD      A,' '
        CALL    OUTCHAR
COUNT_ONES:
        LD      A,E
        ADD     A,'0'
        CALL    OUTCHAR
        JP      COUNT_RET

COUNT_BLANK:
        LD      A,' '
        CALL    OUTCHAR
        CALL    OUTCHAR
        CALL    OUTCHAR
        CALL    OUTCHAR
        JP      COUNT_RET

COUNT_OVERFLOW:
        LD      A,' '
        CALL    OUTCHAR
        CALL    OUTCHAR
        CALL    OUTCHAR
        LD      A,'*'
        CALL    OUTCHAR
COUNT_RET:
        POP     BC
        RET

PRINT_CRLF:
        LD      DE,CRLF
        CALL    PRINT_STRING
        RET

; ------------------------------------------------
; Data
; ------------------------------------------------

HEADING:
        DEFB    "DMAP - CP/M 2.2 Drive Map",13,10
        DEFB    "Press ESC or CTL-C to quit",13,10,13,10,'$'

DRIVE_HEADING:
        DEFB    "Drives present on system =>  $"

USER_HEADING:
        DEFB    "USR>",'$'

CRLF:
        DEFB    13,10,'$'

DRIVE_LIST:
        DEFB    'A','B','C','D','E','F','G','H'
        DEFB    'I','J','K','L','M','N','O','P'

DSK:        DEFB    0
USER:       DEFB    0
SAVED_USER: DEFB    0
NUMBER_LO:  DEFB    0
NUMBER_HI:  DEFB    0

USERCOUNT:  DEFS    32

SEARCH_FCB:
        DEFB    0
        DEFB    "????????"
        DEFB    "???"
        DEFB    0,0,0,0
        DEFS    16
        DEFB    0,0,0,0
