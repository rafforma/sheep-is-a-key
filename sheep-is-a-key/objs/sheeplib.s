.export     _flip_h

.export     _x0,_x1,_y0,_y1,_resbox
.export     _list_of_rect
.export     _resbox
.export     _init_raster
.export     _stop_raster
.export     _frame_ready
.export     _waitjingle
.export     _musicid
.export     _drawlevel
.export     _rastermode
.export     _waitjinglewithkey
.define     SHEEP_ADDR $3c00

sid_init = $9000
sid_play = $9003
siddata = $9000

.segment	"DATA"	
_counter:
	.word $0
_rastermode:
	.word $0
	
_flip_lookup:
		.byte $00, $80, $40, $c0, $20, $a0, $60, $e0
        .byte $10, $90, $50, $d0, $30, $b0, $70, $f0
        .byte $08, $88, $48, $c8, $28, $a8, $68, $e8
        .byte $18, $98, $58, $d8, $38, $b8, $78, $f8
        .byte $04, $84, $44, $c4, $24, $a4, $64, $e4
        .byte $14, $94, $54, $d4, $34, $b4, $74, $f4
        .byte $0c, $8c, $4c, $cc, $2c, $ac, $6c, $ec
        .byte $1c, $9c, $5c, $dc, $3c, $bc, $7c, $fc
        .byte $02, $82, $42, $c2, $22, $a2, $62, $e2
        .byte $12, $92, $52, $d2, $32, $b2, $72, $f2
        .byte $0a, $8a, $4a, $ca, $2a, $aa, $6a, $ea
        .byte $1a, $9a, $5a, $da, $3a, $ba, $7a, $fa
        .byte $06, $86, $46, $c6, $26, $a6, $66, $e6
        .byte $16, $96, $56, $d6, $36, $b6, $76, $f6
        .byte $0e, $8e, $4e, $ce, $2e, $ae, $6e, $ee
        .byte $1e, $9e, $5e, $de, $3e, $be, $7e, $fe
        .byte $01, $81, $41, $c1, $21, $a1, $61, $e1
        .byte $11, $91, $51, $d1, $31, $b1, $71, $f1
        .byte $09, $89, $49, $c9, $29, $a9, $69, $e9
        .byte $19, $99, $59, $d9, $39, $b9, $79, $f9
        .byte $05, $85, $45, $c5, $25, $a5, $65, $e5
        .byte $15, $95, $55, $d5, $35, $b5, $75, $f5
        .byte $0d, $8d, $4d, $cd, $2d, $ad, $6d, $ed
        .byte $1d, $9d, $5d, $dd, $3d, $bd, $7d, $fd
        .byte $03, $83, $43, $c3, $23, $a3, $63, $e3
        .byte $13, $93, $53, $d3, $33, $b3, $73, $f3
        .byte $0b, $8b, $4b, $cb, $2b, $ab, $6b, $eb
        .byte $1b, $9b, $5b, $db, $3b, $bb, $7b, $fb
        .byte $07, $87, $47, $c7, $27, $a7, $67, $e7
        .byte $17, $97, $57, $d7, $37, $b7, $77, $f7
        .byte $0f, $8f, $4f, $cf, $2f, $af, $6f, $ef
        .byte $1f, $9f, $5f, $df, $3f, $bf, $7f, $ff			

_nspr:
		.byte $4
		
			
.segment "CODE"
.proc _flip_h: near
	;;sei
	lda #4
	sta _nspr
	ldy #0
@next_row:
	ldx SHEEP_ADDR,y
	lda SHEEP_ADDR+2,y
	sta SHEEP_ADDR,y
	txa
	sta SHEEP_ADDR+2,y
	ldx SHEEP_ADDR+2,y
	lda _flip_lookup,x
	sta SHEEP_ADDR+2,y
	ldx SHEEP_ADDR,y
	lda _flip_lookup,x
	sta SHEEP_ADDR,y
	ldx SHEEP_ADDR+1,y
	lda _flip_lookup,x
	sta SHEEP_ADDR+1,y
	iny
	iny
	iny	
	tya
	and #63
	cmp #63
	bne @next_row
	iny
	dec _nspr
	bne @next_row

	lda $d025
	ldx $d027
	sta $d027
	stx $d025
	;;cli
	rts
.endproc

.segment	"DATA"
_resbox:
	.word $0
_x0: 
	.word $0
_x1:
	.word $0
_y0: 
	.word $0
_y1:
	.word $0
	
_list_of_rect:
	.res 10*24
	
.segment "CODE"

.proc _init_raster
	sei
	lda _musicid
	;;;jsr sid_init
	lda #>_raster_service
	sta $0315
	sta $ffff
	lda #<_raster_service
	sta $0314
	sta $fffe
	lda #$7f
	sta $dc0d
	lda #$01
	sta $d01a
	sta $d011
	ora #$0b
	sta $d011
	lda #$30
	sta $d012
	lda #$00
	sta irqcnt
	cli
	rts
.endproc

.proc _stop_raster
	sei
	lda #$ea
	sta $0315
	lda #$31
	sta $0314
	inc $d019
	lda #$81
	sta $dc0d
	lda #$00
	sta $d01a
	cli
	rts
.endproc
	
.segment "DATA"
_frame_ready:
	.word $1
_musicid:
	.word $0
_curmusic:
	.word $ff
	
raster_line:
	.byte 50
	.byte 50+64-2
	.byte 50+64+8
    .byte 50+128-2
	.byte 50+128+8
	.byte 50+192-2
    .byte 50+200
	
irqcnt:
	.byte $0
	
num_lines:
	.byte $8

colors:
	;;;.byte 2,3,4,11
	.byte 11,0,11,0,11,0,11,$ff
	
.segment "CODE"



.proc _raster_service 


	pha
	txa
	pha
	;;tya
	;;pha
	php
	

	lda irqcnt
	tax
	lda colors,x	
	
	ldx _rastermode
	beq @L5
	lda #11
	
@L5:
	sta    $d021
	inc irqcnt	
	lda irqcnt
	cmp num_lines
	bne @L1
	lda #0
	sta   _frame_ready	
	sta irqcnt
	lda _curmusic
	cmp _musicid
	beq @nochangemusic
	lda _musicid
	sta _curmusic
	jsr sid_init
@nochangemusic:
	 jsr sid_play
@L1:
	
	lda #<_raster_service  
	sta $0314  
	lda #>_raster_service
	STA $0315
	
	lda irqcnt	
	tax
	lda raster_line,x
	sta $d012		
	
	inc    $d019
	
	nop ; Wait some cycles to make the loop work fine
    nop
    nop
    nop
	nop
	lda 0

	
	plp
	pla
	;;tay
	;;pla
	tax
	pla
	
	jmp $ea81

	
	rti
	
.endproc

.proc _waitjingle
	lda #0
	sta _counter
	sta _counter+1
@wait:
	 lda $d012
	 cmp #$ff
	 bne @wait
	 lda _counter
	 clc
	 adc #1
	 sta _counter
	 lda #0
	 adc _counter+1
	 sta _counter+1
	 cmp #3	 
	 bne @wait
	 rts
.endproc

.proc _waitjinglewithkey
	lda #0
	sta _counter
	sta _counter+1
@wait:
	lda #$ff
	sta $dc00
	lda $dc00
	eor #$ff
    and #16
	bne	@end
	 lda $d012
	 cmp #$ff
	 bne @wait
	 lda _counter
	 clc
	 adc #1
	 sta _counter
	 lda #0
	 adc _counter+1
	 sta _counter+1
	 cmp #2	 
	 bne @wait
@end:
	 rts
.endproc

.proc _drawlevel 

	lda #0
	sta    $d020
	
	lda #9
	sta $d022
	lda #8
	sta $d023
	
	ldy #0
@nc:	
	lda _screen,y
	sta $400+7*40,y
	lda #13
	sta $d800+7*40,y
	
	lda _screen1,y
	sta $400+8*40,y
	lda #13
	sta $d800+8*40,y
	

	lda _screen,y
	sta $400+15*40,y
	lda #13
	sta $d800+15*40,y
	
	lda _screen1,y
	sta $400+16*40,y
	lda #13
	sta $d800+16*40,y
	

	lda _screen,y
	sta $400+23*40,y
	lda #13
	sta $d800+23*40,y
	
	lda _screen1,y
	sta $400+24*40,y
	lda #13
	sta $d800+24*40,y
	
	
	
	iny
	cpy #40
	bne @nc
	
	rts
.endproc

.segment "DATA"
_screen:
	.byte $3c
	.byte $3d,$3e,$3f,$40,$41,$42,$3d,$3e,$3f,$40,$41,$42,$3d,$3e,$3f,$40,$41,$42,$3d,$3e,$3f,$40,$41,$42,$3d,$3e,$3f,$40,$41,$42,$3d,$3e,$3f,$40,$41,$42,$3d,$3e,$3f
	
	.byte $43
	
_screen1:
	.byte $44
	.byte $45,$46,$47,$48,$49,$4A,$45,$46,$47,$48,$49,$4A,$45,$46,$47,$48,$49,$4A,$45,$46,$47,$48,$49,$4A,$45,$46,$47,$48,$49,$4A,$45,$46,$47,$48,$49,$4A,$45,$46,$48
	.byte $4b
.segment "CODE"
.res $2000,$1

