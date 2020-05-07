; feilipu, May 2020
;
; This file is In the public domain.  Use it as you see fit.
;
; "cp" is an extremely simple file copy program.
;
; It is as fast as CP/M PIP, but not slower.
; 
; Usage:  cp [destination][source]
;
; zcc +cpm --list -m cp.asm -ocp -create-app
;
DEFC    REBOOT      =   00000H  ;system reboot
DEFC    IOBYTE      =   00003H  ;i/o definition byte
DEFC    TDRIVE      =   00004H  ;current drive name and user number
DEFC    BDOS        =   00005H  ;bdos entry point
DEFC    FCB1        =   0005CH  ;first file name
DEFC    FCB2        =   0006CH  ;second file name
DEFC    TBUFF       =   00080H  ;i/o buffer and command line storage
DEFC    TPA         =   00100H  ;transient program storage area
;
DEFC    BUFFER      =   08000H  ;32kB of buffers each of 128 Byte Sectors
;
;   Set control character equates.
;
DEFC    CNTRLC      =   03H     ;control-c
DEFC    CNTRLE      =   05H     ;control-e
DEFC    BS          =   08H     ;backspace
DEFC    TAB         =   09H     ;tab
DEFC    LF          =   0AH     ;line feed
DEFC    FF          =   0CH     ;form feed
DEFC    CR          =   0DH     ;carriage return
DEFC    CNTRLP      =   10H     ;control-p
DEFC    CNTRLR      =   12H     ;control-r
DEFC    CNTRLS      =   13H     ;control-s
DEFC    CNTRLU      =   15H     ;control-u
DEFC    CNTRLX      =   18H     ;control-x
DEFC    CNTRLZ      =   1AH     ;control-z (end-of-file mark)
DEFC    DEL         =   7FH     ;rubout
;
;    BDOS Control codes
;
DEFC    CPM_RCON = 1
DEFC    CPM_WCON = 2
DEFC    CPM_RRDR = 3
DEFC    CPM_WPUN = 4
DEFC    CPM_WLST = 5
DEFC    CPM_DCIO = 6
DEFC    CPM_GIOB = 7
DEFC    CPM_SIOB = 8
DEFC    CPM_PRST = 9
DEFC    CPM_RCOB = 10
DEFC    CPM_ICON = 11
DEFC    CPM_VERS = 12
DEFC    CPM_RDS  = 13
DEFC    CPM_LGIN = 14
DEFC    CPM_OPN  = 15
DEFC    CPM_CLS  = 16
DEFC    CPM_FFST = 17
DEFC    CPM_FNXT = 18
DEFC    CPM_DEL  = 19
DEFC    CPM_READ = 20
DEFC    CPM_WRIT = 21
DEFC    CPM_MAKE = 22
DEFC    CPM_REN  = 23
DEFC    CPM_ILOG = 24
DEFC    CPM_IDRV = 25
DEFC    CPM_SDMA = 26
DEFC    CPM_SUID = 32
DEFC    CPM_RRAN = 33
DEFC    CPM_WRAN = 34
DEFC    CPM_CFS  = 35


SECTION data_user

nofile: DEFM    "no source files$"
nodir:  DEFM    "no directory space$"
fspace: DEFM    "out of data space$"
wrprot: DEFM    "write protected?$"
normal: DEFM    "copy complete$"

sfcb:   DEFS    33,0        ;source fcb

dfcb:   DEFS    33,0        ;destination fcb

DEFC    sfcbcr  = sfcb+32   ;current read record
DEFC    dfcbcr  = dfcb+32   ;current write record

rdsek: DEFB     0           ;read sector index
wrsek: DEFB     0           ;write sector index
endof: DEFB     0           ;end of file flag

       DEFS     32          ;stack
stack:


SECTION bss_user

udma:                       ;user dma space in bss


SECTION code_user

;   IF FASTCOPY THEN /* COPY DIRECTLY TO DBUFF */
;       DO; CALL SET$DBLEN; /* EXTEND DBUFF */
;           DO WHILE NOT REAL$EOF;
;           CALL FILLSOURCE;
;           IF REAL$EOF THEN
;               NDEST = HARDEOF; ELSE NDEST = DBLEN;
;           CALL WRITEDEST;
;           END;

PUBLIC  _main

_main:
    ld      sp,stack        ;set stack

;   move second file name to sfcb
    ld      hl,FCB2
    ld      de,sfcb
    ld      bc,16
    ldir

;   move first file name to dfcb
    ld      hl,FCB1
    ld      de,dfcb
    ld      bc,16
    ldir

;   open source file
    ld      de,sfcb
    call    open
    ld      de,nofile       ;no file message
    inc     a
    jp      Z,exit

;   source file open, prepare destination
    ld      de,dfcb
    call    delete
    ld      de,dfcb
    call    make
    ld      de,nodir        ;no directory message
    inc     a
    jp      Z,exit

;   zero the current records
    xor     a
    ld      (sfcbcr),a
    ld      (dfcbcr),a
    ld      (endof),a       ;we're not at the end of file

;   source file open, dest file open
;   copy until end of file from source
copy_loop:

;   check we're not eof
    xor     a
    ld      hl,endof
    or      (hl)
    jp      NZ,eofile       ;jump to end of file

;   zero the sector indexes
    ld      (rdsek),a
    ld      (wrsek),a

;   set dma address
    ld      de,udma
    call    setdma

read_loop:  
    ld      de,sfcb         ;source
    call    read            ;read next record
    or      a               ;end of file?
    jr      NZ,file_end     ;skip to fileend if so
 
    ld      a,(rdsek)
    inc     a
    ld      (rdsek),a
    cp      BUFFER/128-1    ;number of buffers we have
    jp      NC,write_start
    
    ld      l,a             ;ready to shift
    xor     a
    rr      l               ;shift left 7, for 128 bytes sectors
    rra
    ld      h,l
    ld      l,a

    ld      de,udma
    add     hl,de           ;create new destination dma
    ex      de,hl
    call    setdma
    
    jp      read_loop

file_end:
    ld      a,1             ;set end of file flag
    ld      (endof),a

write_start:
;   set dma address
    ld      de,udma
    call    setdma

write_loop:
    ld      a,(rdsek)
    or      a
    jr      Z,copy_loop     ;reached end of file before end of buffers
    dec     a 
    ld      (rdsek),a

    ld      de,dfcb         ;destination
    call    write           ;write record
    ld      de,fspace       ;no free space message
    or      a               ;0 if write OK
    jp      NZ,exit         ;end if so

    ld      a,(wrsek)
    inc     a
    ld      (wrsek),a
    cp      BUFFER/128-1    ;number of buffers we have
    jp      NC,copy_loop

    ld      l,a             ;ready to shift
    xor     a
    rr      l               ;shift left 7, for 128 bytes sectors
    rra
    ld      h,l
    ld      l,a

    ld      de,udma
    add     hl,de           ;create new source dma
    ex      de,hl
    call    setdma
    
    jp      write_loop      ;loop until buffer


eofile:
    ld      de,dfcb         ;destination close
    call    close           ;255 if error
    ld      de,wrprot       ;write protected message
    inc     a
    jr      Z,exit

    ld      de,sfcb         ;source close
    call    close
    ld      de,normal       ;normal finish message
exit:
    ld      c,CPM_PRST
    call    BDOS
    ret                     ;exit to CCP


open:
    ld      c,CPM_OPN
    jp      BDOS

close:
    ld      c,CPM_CLS
    jp      BDOS

delete:
    ld      c,CPM_DEL
    jp      BDOS

read:
    ld      c,CPM_READ
    jp      BDOS

write:
    ld      c,CPM_WRIT
    jp      BDOS

make:
    ld      c,CPM_MAKE
    jp      BDOS

setdma:
    ld      c,CPM_SDMA
    jp      BDOS

