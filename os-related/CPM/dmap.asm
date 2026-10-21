; DMAP.ASM
; Drive map for CP/M 2.2 and CP/M 3.
; Richard Deane, modified by feilipu for z88dk.
;
; zcc +cpm -vn dmap.asm -o dmap -create-app
;
; The z88dk CP/M CRT sets the stack and calls _main. _main returns
; to that CRT. ESC, Ctrl-C, and a finished map take the same return.
; A bad argument returns HL = 1 so CP/M 3 can record an error.
;
; dmap           - map every drive A: through P:
; dmap X         - map drive X only (optional trailing colon)
;
; Each count is extent 0 for one user, so each file is counted once.
; Users are 0 through 15. A zero count is blank. 1000 or more is ***.
;
        SECTION code_user

        PUBLIC  _main

; ------------------------------------------------
; BDOS function numbers
; ------------------------------------------------

BDOS            EQU     0005H
FN_CONOUT       EQU     2
FN_DIRIO        EQU     6
FN_PRINT        EQU     9
FN_CONST        EQU     11
FN_GETUSR       EQU     32
FN_SETUSR       EQU     32
FN_SRCHF        EQU     17
FN_SRCHN        EQU     18
FN_CURDSK       EQU     25
FN_SETDMA       EQU     26

NO_DRIVE        EQU     0FFH            ; no drive on the command line

; ------------------------------------------------
; Main program
; ------------------------------------------------

_main:
        LD      HL,0
        ADD     HL,SP
        LD      (SAVED_SP),HL           ; CRT return address sits here

        LD      E,0FFH
        LD      C,FN_GETUSR
        CALL    BDOS
        LD      (SAVED_USER),A

        LD      C,FN_CURDSK             ; logged-in drive, restored with E = 1
        CALL    BDOS
        LD      (ORIG_DRIVE),A

        CALL    PARSE_CMDLINE           ; read the tail before any disk I/O
        CALL    SELECT_SELDSK

        LD      DE,DMABUF               ; DMA buffer for directory searches
        LD      C,FN_SETDMA
        CALL    BDOS

        LD      A,(ARG_DRIVE)
        CP      NO_DRIVE
        JR      NZ,SINGLE_DRIVE

; ------------------------------------------------
; All drives
; ------------------------------------------------

        LD      DE,HEADING
        CALL    PRINT_STRING

        CALL    DETECT_DRIVES

        LD      DE,DRIVE_HEADING
        CALL    PRINT_STRING
        LD      DE,DRIVE_LIST
        LD      B,16
LIST_LOOP:
        LD      A,(DE)
        CALL    OUTCHAR
        LD      A,' '
        CALL    OUTCHAR
        INC     DE
        DJNZ    LIST_LOOP
        CALL    PRINT_CRLF

        CALL    SHOW_BAR
        CALL    SHOW_USER_HEADER

        XOR     A
        LD      (NEXT_DRV),A
ALL_LOOP:
        LD      A,(NEXT_DRV)
        LD      E,A
        LD      D,0
        LD      HL,DRIVE_LIST
        ADD     HL,DE
        LD      A,(HL)
        CP      '*'                     ; '*' = drive not present
        LD      A,(NEXT_DRV)            ; (LD does not touch the flags)
        CALL    NZ,DRIVE_MAP
        LD      A,(NEXT_DRV)
        INC     A
        LD      (NEXT_DRV),A
        CP      16
        JR      C,ALL_LOOP
        JP      QUIT_PROGRAM

; ------------------------------------------------
; Single drive given on the command line
; ------------------------------------------------

SINGLE_DRIVE:
        LD      A,(ARG_DRIVE)
        CALL    DRIVE_PRESENT           ; NZ = present
        PUSH    AF
        CALL    RESTORE_DRIVE           ; no media access
        POP     AF
        JR      NZ,SINGLE_FOUND

        LD      DE,MSG_DRIVE
        CALL    PRINT_STRING
        LD      A,(ARG_DRIVE)
        CALL    PRINT_DRIVE_LETTER
        LD      DE,MSG_ABSENT
        CALL    PRINT_STRING
        JP      QUIT_PROGRAM

SINGLE_FOUND:
        LD      DE,MSG_DRIVE
        CALL    PRINT_STRING
        LD      A,(ARG_DRIVE)
        CALL    PRINT_DRIVE_LETTER
        CALL    PRINT_CRLF
        CALL    SHOW_BAR
        CALL    SHOW_USER_HEADER
        LD      A,(ARG_DRIVE)
        CALL    DRIVE_MAP
        ; fall through

QUIT_PROGRAM:
        LD      HL,0
        JR      EXIT_TO_CRT

; Bad argument. The message is already printed.
USAGE_EXIT:
        LD      HL,1

; Restore the entry stack, then return to the CRT from any depth.
EXIT_TO_CRT:
        LD      (EXIT_CODE),HL
        LD      A,(SAVED_USER)
        LD      E,A
        LD      C,FN_SETUSR
        CALL    BDOS
        LD      HL,(SAVED_SP)
        LD      SP,HL
        LD      HL,(EXIT_CODE)
        RET

; ------------------------------------------------
; DRIVE_MAP
; Entry: A = drive number (0 = A: ... 15 = P:)
; Prints one row: "  X:>" followed by the extent-0 file
; count for users 0..15, then CR/LF.
; ------------------------------------------------

DRIVE_MAP:
        LD      (DSK),A
        LD      A,' '
        CALL    OUTCHAR
        CALL    OUTCHAR
        LD      A,(DSK)
        CALL    PRINT_DRIVE_LETTER
        LD      A,':'
        CALL    OUTCHAR
        LD      A,'>'
        CALL    OUTCHAR

        XOR     A
        LD      (USER),A

MAP_USER_LOOP:
        CALL    EXIT_CHECK

        LD      A,(USER)
        LD      E,A
        LD      C,FN_SETUSR
        CALL    BDOS

        LD      A,(DSK)                 ; FCB drive code: 1 = A:
        INC     A
        LD      (SEARCH_FCB),A

; Rebuild the name and extent. Search rewrites the FCB, and a later
; user would otherwise match with leftover attribute or extent bytes.
        LD      HL,SEARCH_FCB+1
        LD      B,11
        LD      A,'?'
SET_WILD:
        LD      (HL),A
        INC     HL
        DJNZ    SET_WILD
        LD      B,4
CLR_EXT:
        LD      (HL),0
        INC     HL
        DJNZ    CLR_EXT

        LD      HL,0
        LD      (COUNT),HL

        LD      DE,SEARCH_FCB
        LD      C,FN_SRCHF
        CALL    BDOS
MAP_SEARCH:
        CP      0FFH
        JR      Z,MAP_COUNTED
        LD      HL,(COUNT)
        INC     HL
        LD      A,H
        OR      L
        JR      NZ,COUNT_STORE
        DEC     HL                      ; stay at 65535, which prints as ***
COUNT_STORE:
        LD      (COUNT),HL
        LD      DE,SEARCH_FCB
        LD      C,FN_SRCHN
        CALL    BDOS
        JR      MAP_SEARCH

MAP_COUNTED:
        LD      HL,(COUNT)
        CALL    PRINT_COUNT
        LD      A,' '
        CALL    OUTCHAR

        LD      A,(USER)
        INC     A
        LD      (USER),A
        CP      16
        JR      C,MAP_USER_LOOP

        JP      PRINT_CRLF

; ------------------------------------------------
; Parse the command tail at 0080H.
; Stores the drive (0..15) in ARG_DRIVE, or NO_DRIVE
; if the tail is empty. A bad parameter prints usage
; and returns to the CRT with HL = 1.
;
; The z88dk CRT turns some spaces in the tail into NULs
; before _main. A NUL is treated as a space.
;
; Accepted: nothing, or one letter A-P, an optional colon,
; then only spaces or NULs. Anything else is a usage error.
; ------------------------------------------------

PARSE_CMDLINE:
        LD      A,NO_DRIVE
        LD      (ARG_DRIVE),A
        LD      HL,0080H
        LD      A,(HL)                  ; tail length
        OR      A
        RET     Z
        LD      B,A
PC_SKIP:
        INC     HL
        LD      A,(HL)
        CP      ' '
        JR      Z,PC_BLANK
        OR      A                       ; NUL from the CRT parser
        JR      NZ,PC_GOT
PC_BLANK:
        DJNZ    PC_SKIP
        RET                             ; only spaces or NULs

PC_GOT:
        CP      'a'
        JR      C,PC_UPPER
        CP      'z'+1
        JR      NC,PC_UPPER
        SUB     20H                     ; to uppercase
PC_UPPER:
        SUB     'A'
        CP      16                      ; also rejects anything below 'A'
        JR      NC,PC_BAD
        LD      (ARG_DRIVE),A
        DJNZ    PC_MORE                 ; letter was the last character
        RET
PC_MORE:
        INC     HL
        LD      A,(HL)
        CP      ':'
        JR      NZ,PC_TRAIL
        DJNZ    PC_AFTER_COLON          ; colon was the last character
        RET
PC_AFTER_COLON:
        INC     HL
        LD      A,(HL)
PC_TRAIL:
        CP      ' '
        JR      Z,PC_TRAIL_OK
        OR      A
        JR      NZ,PC_BAD
PC_TRAIL_OK:
        DJNZ    PC_TRAIL_NEXT
        RET
PC_TRAIL_NEXT:
        INC     HL
        LD      A,(HL)
        JR      PC_TRAIL

PC_BAD:
        LD      DE,MSG_USAGE
        CALL    PRINT_STRING
        JP      USAGE_EXIT

; ------------------------------------------------
; Detect drives with SELDSK; marks absent ones '*'
; in DRIVE_LIST, then selects the logged-in drive again
; without reading it.
; ------------------------------------------------

DETECT_DRIVES:
        XOR     A
        LD      (DSK),A
DET_LOOP:
        CALL    EXIT_CHECK
        LD      A,(DSK)
        CALL    DRIVE_PRESENT
        JR      NZ,DET_NEXT
        LD      A,(DSK)
        LD      E,A
        LD      D,0
        LD      HL,DRIVE_LIST
        ADD     HL,DE
        LD      (HL),'*'
DET_NEXT:
        LD      A,(DSK)
        INC     A
        LD      (DSK),A
        CP      16
        JR      C,DET_LOOP
        JP      RESTORE_DRIVE

; ------------------------------------------------
; DRIVE_PRESENT
; Entry: A = drive number
; Exit : NZ = drive exists, Z = no such drive
; (leaves the drive selected; caller reselects)
; ------------------------------------------------

DRIVE_PRESENT:
        CALL    SELDSK
        LD      A,H
        OR      L
        RET

; Select the drive that was logged in at entry.
; E = 1 tells the BIOS this disk is already logged, so it
; returns the DPH and does not touch the media.
RESTORE_DRIVE:
        LD      A,1
        LD      (SEL_FLAG),A
        LD      A,(ORIG_DRIVE)
        CALL    SELDSK
        XOR     A
        LD      (SEL_FLAG),A
        RET

; ------------------------------------------------
; Select the appropriate SELDSK routine
; Call this once during program initialisation.
; ------------------------------------------------

SELECT_SELDSK:
        LD      C,12                    ; Get BDOS version
        CALL    BDOS                    ; H = system type, L = version
        LD      A,H
        AND     01H                     ; bit 0 set means MP/M
        JR      NZ,USE_DIRECT           ; MP/M has no function 50
        LD      A,L
        CP      30H                     ; CP/M Plus is 30h or 31h
        JR      C,USE_DIRECT
        LD      HL,SELDSK3
        JR      SET_SELDSK
USE_DIRECT:
        LD      HL,SELDSK2
SET_SELDSK:
        LD      (SELDSK_CALL+1),HL
        RET

; ------------------------------------------------
; Common entry point.
; Call this from anywhere: CALL SELDSK
; Entry: A = drive number (0=A:, 1=B:, etc.)
; Exit : HL = DPH address, or 0 if no such drive
; ------------------------------------------------

SELDSK:
SELDSK_CALL:
        JP      SELDSK2                 ; address replaced at initialisation

; ------------------------------------------------
; CP/M 2.2 BIOS SELDSK
; ------------------------------------------------

SELDSK2:
        LD      C,A                     ; Drive number
        LD      HL,(0001H)              ; address of WBOOT, which is BIOS+3
        LD      DE,0018H                ; SELDSK is WBOOT+24
        ADD     HL,DE
        LD      A,(SEL_FLAG)            ; 0 = login, 1 = already logged
        LD      E,A                     ; load E after DE has been used
        JP      (HL)                    ; BIOS SELDSK returns to our caller

; ------------------------------------------------
; CP/M 3 BIOS SELDSK via BDOS function 50
; Entry: A = drive number
; ------------------------------------------------

SELDSK3:
        LD      C,A
        LD      B,0
        LD      (BCREG),BC              ; C = drive, B = 0

; Function 50 on a banked system copies the DPH onto this block,
; so every field has to be written again for the next drive.
; E = 0 logs the drive in. HL = 0 when it is absent or cannot be logged.
; E = 1 returns the DPH of a drive that is already logged.
        LD      A,(SEL_FLAG)
        LD      (DEREG),A
        XOR     A
        LD      (DEREG+1),A
        LD      H,A
        LD      L,A
        LD      (HLREG),HL
        LD      (AREG),A

        LD      A,9                     ; BIOS function SELDSK
        LD      (FUNC),A

        LD      DE,BIOSPB
        LD      C,50                    ; BDOS function 50
        JP      BDOS                    ; DPH address, or 0, comes back in HL

; ------------------------------------------------
; Console polling: quit on ESC or CTRL-C, never waits
; ------------------------------------------------

EXIT_CHECK:
        LD      C,FN_CONST
        CALL    BDOS                    ; A = 0 if no key waiting
        OR      A
        RET     Z
        LD      E,0FFH
        LD      C,FN_DIRIO              ; direct input, no echo
        CALL    BDOS
        AND     7FH                     ; ignore a parity bit
        CP      1BH                     ; ESC
        JP      Z,QUIT_PROGRAM
        CP      03H                     ; CTRL-C
        JP      Z,QUIT_PROGRAM
        RET

; ------------------------------------------------
; Print routines
; ------------------------------------------------

PRINT_STRING:
        PUSH    BC
        LD      C,FN_PRINT
        CALL    BDOS
        POP     BC
        RET

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

PRINT_CRLF:
        LD      DE,CRLF
        JR      PRINT_STRING

SHOW_BAR:
        LD      DE,DIVIDER
        CALL    PRINT_STRING
        JR      PRINT_CRLF

; Entry: A = drive number 0..15
PRINT_DRIVE_LETTER:
        ADD     A,'A'
        JR      OUTCHAR

; "USR>  0  1 ... 15" followed by a blank line
SHOW_USER_HEADER:
        LD      DE,USER_HEADING
        CALL    PRINT_STRING
        LD      B,0
SUH_LOOP:
        LD      A,B
        CALL    PRINT_DEC
        INC     B
        LD      A,B
        CP      16
        JR      C,SUH_LOOP
        CALL    PRINT_CRLF
        JR      PRINT_CRLF

; Print 0..15 as a four character right-aligned field
; Entry: A = value
PRINT_DEC:
        LD      D,A
        LD      A,' '
        CALL    OUTCHAR
        CALL    OUTCHAR
        LD      A,D
        CP      10
        JR      C,PD_ONE_DIGIT
        SUB     10
        LD      D,A
        LD      A,'1'                   ; 10..15 -> tens digit is always 1
        JR      PD_TENS
PD_ONE_DIGIT:
        LD      A,' '
PD_TENS:
        CALL    OUTCHAR
        LD      A,D
        ADD     A,'0'
        JR      OUTCHAR

; Print a 16-bit count as a three-character field.
; Entry: HL = count
; 0       -> three spaces
; 1-999   -> decimal, right-aligned, no leading zeroes
; 1000+   -> ***
PRINT_COUNT:
        LD      A,H
        OR      L
        JR      Z,COUNT_BLANK

        PUSH    HL
        LD      DE,1000
        OR      A
        SBC     HL,DE
        POP     HL
        JR      NC,COUNT_OVERFLOW

        LD      B,0                     ; hundreds
        LD      DE,100
COUNT_HUND:
        OR      A
        SBC     HL,DE
        JR      C,COUNT_HUND_DONE
        INC     B
        JR      COUNT_HUND
COUNT_HUND_DONE:
        ADD     HL,DE                   ; undo last subtraction
        LD      A,L                     ; 0..99
        LD      C,0                     ; tens
COUNT_TENS:
        CP      10
        JR      C,COUNT_TENS_DONE
        SUB     10
        INC     C
        JR      COUNT_TENS
COUNT_TENS_DONE:
        LD      D,A                     ; ones

        LD      A,B                     ; hundreds digit or space
        OR      A
        JR      Z,COUNT_NO_HUND
        ADD     A,'0'
        JR      COUNT_PUT_HUND
COUNT_NO_HUND:
        LD      A,' '
COUNT_PUT_HUND:
        CALL    OUTCHAR

        LD      A,B                     ; tens digit or space
        OR      C
        JR      Z,COUNT_NO_TENS
        LD      A,C
        ADD     A,'0'
        JR      COUNT_PUT_TENS
COUNT_NO_TENS:
        LD      A,' '
COUNT_PUT_TENS:
        CALL    OUTCHAR

        LD      A,D                     ; ones digit
        ADD     A,'0'
        JP      OUTCHAR

COUNT_BLANK:
        LD      DE,STR_BLANK
        JP      PRINT_STRING

COUNT_OVERFLOW:
        LD      DE,STR_OVERFLOW
        JP      PRINT_STRING

; ------------------------------------------------
; Data
; ------------------------------------------------

HEADING:
        DB      "DMAP - Drive map for CP/M 2.2 and CP/M 3",13,10
        DB      "Usage: DMAP [drive]",13,10
        DB      "Press ESC or CTL-C to quit",13,10,13,10,'$'

DRIVE_HEADING:
        DB      "Drives present on system =>  $"

USER_HEADING:
        DB      "USR>$"

MSG_DRIVE:
        DB      "Drive $"

MSG_ABSENT:
        DB      " not present",13,10,'$'

MSG_USAGE:
        DB      "Usage: DMAP [drive A..P]",13,10,'$'

CRLF:
        DB      13,10,'$'

STR_BLANK:
        DB      "   $"

STR_OVERFLOW:
        DB      "***$"

DIVIDER:
        DB      "====================================================================  $"

DRIVE_LIST:
        DB      'A','B','C','D','E','F','G','H'
        DB      'I','J','K','L','M','N','O','P'

ARG_DRIVE:  DB  NO_DRIVE
DSK:        DB  0
USER:       DB  0
NEXT_DRV:   DB  0
SAVED_USER: DB  0
ORIG_DRIVE: DB  0
SEL_FLAG:   DB  0                       ; 0 = login select, 1 = already logged
COUNT:      DW  0
SAVED_SP:   DW  0
EXIT_CODE:  DW  0
DMABUF:     DS  128

SEARCH_FCB:
        DB      0
        DB      "????????"
        DB      "???"
        DB      0,0,0,0
        DS      16
        DB      0,0,0,0

; CP/M 3 BIOS-call parameter block

; Banked CP/M 3 function 50 writes a 25-byte DPH copy at BIOSPB+1.
; The block is 26 bytes so that copy stays inside it.
BIOSPB:
FUNC:   DB      0
AREG:   DB      0
BCREG:  DW      0
DEREG:  DW      0
HLREG:  DW      0
        DS      18
