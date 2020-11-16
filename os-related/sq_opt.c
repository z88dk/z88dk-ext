
/* z88dk specific option, to squeeze the sq.c program at most.. may become slower */
/* zcc +cpm -create-app -O3 --opt-code-size -pragma-define:CRT_INITIALIZE_BSS=0 -custom-copt-rules sq.opt -DCOMPACT sq.c */

#asm

;  fix to avoid some odd overflow
defs 100

sq_sub1:
	ld	hl,_obuf
	push	hl
	ld	hl,(_oblen)
	inc	hl
	ld	(_oblen),hl
	dec	hl
	pop	de
	add	hl,de
	ex (sp),hl
	ld (sub1_ret+1),hl
	ld	hl,6	;const
sub1_ret:
	jp	0

sq_sub2:
	pop	de
	ld (sub2_ret+1),de
	pop	de
	ld	a,l
	ld	(de),a
	ld	hl,(_oblen)
	ld	de,128	;const
	ex	de,hl
	call	l_ge
	jr	nc,sub2_ret
	pop	bc
	pop	hl
	push	hl
	push	bc
	push	hl
	call	_oflush
	pop	bc
sub2_ret:
	jp	0

sq_sub3:
	ld	(sub3_smc1+1),bc
	ld	(sub3_smc2+1),bc
	pop	de
	ld (sub3_ret+1),de
	ld	hl,_node
	push	hl
sub3_smc1:
	ld	hl,0
	add	hl,sp
	call	l_gint7de	;
	inc	hl
	inc	hl
	call	l_gchar
	push	hl
	ld	hl,_node
	push	hl
sub3_smc2:
	ld	hl,0	;const
	add	hl,sp
	call	l_gint7de	;
	inc	hl
	inc	hl
	call	l_gchar
sub3_ret:
	jp	0

sq_sub4:
	inc	(hl)
	ld	a,(hl)
	inc	hl
	jr	nz,ASMPC+3
	inc	(hl)
	ret
	
sq_sub5:
	push	hl
	ld	h,d
	ld	l,e
	add	hl,sp
	call	l_gint7de	;
	call	l_gint	;
	ret
	

sq_sub6:
	push	hl
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	push	de
	ld	hl,(_ccode)
	push	hl
	ld	hl,6	;const
	call	l_gcharsp	;
	ld	de,8	;const
	ex	de,hl
	and	a
	sbc	hl,de
	pop	de
	call	l_asl
	pop	de
	call	l_or
	call	l_pint_pop
	ret

sq_sub7:
	pop	bc
	add	hl,hl
	pop	de
	push bc
	add	hl,de
	jp	l_gint	;

sq_sub8:
	pop	bc
	ld	e,h
	ld	h,0	;const
	call	l_gintspsp	;
	ld	l,e
	ld	h,0	;const
	call	l_gintsp	;
	push bc
	ret

sq_sub9:
	ld	de,65535	;const
	and	a
	sbc	hl,de
	scf
	ret

sq_sub10:
	dec	(hl)
	ld	a,(hl)
	inc	hl
	cp	255
	jr	nz,ASMPC+3
	dec	(hl)
	ld	h,(hl)
	ld	l,a
	ret

sq_sub11:
	ld	hl,_node
	push	hl
	ld	h,d
	ld	l,e
	inc hl
	inc hl
	add	hl,sp
	call	l_gint7de	;
	ld	bc,5
	add	hl,bc
	ret

sq_sub12:
	push	hl
	ld	hl,16-2	;const
	call	l_gintsp	;
	push	hl
	call	_putwe
	pop	bc
	pop	bc
	ret

#endasm
