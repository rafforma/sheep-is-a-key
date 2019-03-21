/////exomizer sfx 2061 plasma.prg music.prg -o final.prg

#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include "jumptable.c"
typedef  unsigned int byte;
typedef  unsigned short word;
#include "level.h"
#define CHARSET 0xE800
extern void flip_h();
extern void init_raster();

extern word musicid;
extern word rastermode;
//extern int check_box();

extern word x0,x1;
extern word list_of_rect[16];
extern word resbox;
extern word frame_ready;
extern void drawlevel();
extern void waitjingle();
extern void waitjinglewithkey();
#define poke(m,v) *((char *)m)=v
#define peek(m) *((char *)m)

static word color_lookup[] = {
	0xd800,
	0xd828,
	0xd850,
	0xd878,
	0xd8a0,
	0xd8c8,
	0xd8f0,
	0xd918,
	0xd940,
	0xd968,
	0xd990,
	0xd9b8,
	0xd9e0,
	0xda08,
	0xda30,
	0xda58,
	0xda80,
	0xdaa8,
	0xdad0,
	0xdaf8,
	0xdb20,
	0xdb48,
	0xdb70,
	0xdb98,
	0xdbc0
};

static word lookup[] = {
	0x0400,
	0x0428, 
	0x0450, 
	0x0478, 
	0x04a0, 
	0x04c8, 
	0x04f0, 
	0x0518, 
	0x0540, 
	0x0568, 
	0x0590, 
	0x05b8, 
	0x05e0, 
	0x0608,
	0x0630, 
	0x0658, 
	0x0680, 
	0x06a8, 
	0x06d0, 
	0x06f8,  
	0x0720, 
	0x0748, 
	0x0770, 
	0x0798,
	0x07c0
};
		
/* Use static local variables for speed */
#pragma static-locals (1); 

#define lda_i(b) asm ("lda #%b",(char)b)
#define sta_a(b) asm ("sta %w",b)
#define lda_a(b) asm ("lda %w",b)
#define eor_i(b) asm ("eor #%b",(char)b)

	
#define wr(idx) (__AX__=(idx), 	       	\
       	       	asm ("sta $2000"),      \
                asm ("lda $2000"),      \
                asm ("ldx #$00"),	\
		__AX__)
		
#define joy() (\
		lda_i(255),\
		sta_a(0xdc00),\
		lda_a(0xdc00),\
		eor_i(0xff),\
		__A__\
		)

#define store(v) (\
	asm("sta %v",v),\
	__A__ \
)		

#define bit(v,b) (\
	asm("lda %v",v),\
	asm("and #%b",(char) b),\
	__A__)
	
#define addZP8(v,b) (\
	asm("clc"),\
	asm("lda #%v",(char) b),\
	asm("adc (%v)",v),\
	asm("sta %v",v),\
	asm("bcc ok"),\
	asm("inc (%v+1)",v),\
	asm	("ok:"),\
	)
byte db_cell_color=1;		
byte db_ch=0;
byte db_w=0;
byte db_h=0;
byte db_color=0;
byte db_y=0;
byte db_x=0;

/*void drawbox () {
	
	asm ("lda %v",db_y);
	asm ("asl");
	asm ("tay");
	asm ("lda %v,y",lookup);
	asm ("sta $02");
	asm ("lda %v+1,y",lookup);
	asm ("sta $03");
	
	asm("clc");
	asm("lda %v",db_x);
	asm("adc $02");
	asm("sta $02");	
	asm("lda #$0");
	asm("adc $03");
	asm("sta $03");

	asm ("lda %v",db_y);
	asm ("asl");
	asm ("tay");
	asm ("lda %v,y",color_lookup);
	asm ("sta $04");
	asm ("lda %v+1,y",color_lookup);
	asm ("sta $05");
	
	asm("clc");
	asm("lda %v",db_x);
	asm("adc $04");
	asm("sta $04");	
	asm("lda #$0");
	asm("adc $05");
	asm("sta $05");
	
asm ("next_row:");	
	asm ("ldy #0");
asm ("next_col:");
	asm ("lda %v",db_ch);
	asm ("sta ($02),y");
	asm ("lda %v",db_cell_color);
	asm ("sta ($04),y");
	asm ("iny");
	asm ("cpy %v",db_w);
	asm ("jne next_col");
	asm ("clc");
	asm ("lda #40");
	asm ("adc $02");
	asm ("sta $02");
	asm ("lda #$0");
	asm ("adc $03");
	asm ("sta $03");
	
	asm ("clc");
	asm ("lda #40");
	asm ("adc $04");
	asm ("sta $04");
	asm ("lda #$0");
	asm ("adc $05");
	asm ("sta $05");
	asm ("dec %v",db_h);
	asm ("jne next_row");
	
	
}*/


void drawbox () {
	int i,j;
	
	static byte b0[]={81,80};
	static byte b1[]={80,80};
	byte *ptr;
	char *s = (char *) lookup[db_y]+db_x;
	char *c = (char *) color_lookup[db_y]+db_x;
	int w=db_x+db_w;
	int h=db_y+db_h;
	int dw = 40-db_w;
	if (db_h==1) ptr=b1;
	if (db_h==2) ptr = b0;
	
	db_ch=ptr[0];
	for (i=db_y;i<h;i++) {
		for ( j=db_x;j<w;j++) {
			*s=db_ch;
			*c=db_cell_color;
			s++;
			c++;
		}
		db_ch=ptr[1];
		s+=dw;
		c+=dw;
	}
}

/*void drawblock () {
	
	byte rowlen=5;
	
	asm("lda #$00");
	asm("sta $02");
	asm("lda #$04");
	asm("sta $03");

	asm("ldy #$0");
	asm("lda #$00");
	asm("sta ($02),y");
	asm("iny");
asm("nch:");
	asm("lda #$01");
	asm("sta ($02),y");
	asm("iny");
	asm("cpy #$10");
	asm("jne nch");

	asm("lda #$02");
	asm("sta ($02),y");

	asm ("clc");
	asm ("lda #40");
	asm ("adc $02");
	asm ("sta $02");
	asm ("bcc ok2");
	asm ("inc $3");
	asm	("ok2:");	
	
asm ("next_row1:");
	asm("ldy #$0");
	asm("lda #$03");
	asm("sta ($02),y");
	asm("iny");
asm("nch1:");
	asm("lda #$04");
	asm("sta ($02),y");
	asm("iny");
	asm("cpy #$10");

	asm("jne nch1");

	asm("lda #$05");
	asm("sta ($02),y");

	asm ("clc");
	asm ("lda #40");
	asm ("adc $02");
	asm ("sta $02");
	asm ("bcc ok3");
	asm ("inc $3");
	asm	("ok3:");	
	
	asm ("dec %v",rowlen);
	asm ("jne next_row1");
	
	
}*/
/*static const unsigned char data[] = 
{
	4,64,0,4,64,0,21,127,224,121,127,248,93,255,254,117,255,251,85,255,255,86,255,250,86,255,236,87,255,248,26,255,236,11,190,184,10,171,168,6,174,164,4,106,164,1,64,17,1,16,17,0,80,68,0,68,68,0,4,4,0,0,0,
	140,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,140,0,0,0,4,64,0,4,64,0,21,127,224,121,127,248,93,255,254,117,255,251,85,
	255,255,86,255,250,86,255,236,87,255,248,26,255,236,11,190,184,10,171,168,6,174,164,20,106,164,81,64,68,69,0,17,68,0,17,4,0,17,4,0,1,140,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,140,0,1,16,0,1,16,11,253,84,47,253,109,191,255,117,239,255,93,255,255,85,175,255,149,59,255,149,47,255,213,59,255,164,46,190,224,42,234,160,
	26,186,144,26,169,16,68,1,64,68,4,64,17,5,0,17,17,0,16,16,0,0,0,0,140,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,140,0,
	0,0,0,1,16,0,1,16,11,253,84,47,253,109,191,255,117,239,255,93,255,255,85,175,255,149,59,255,149,47,255,213,59,255,164,46,190,224,42,234,160,26,186,144,26,169,20,17,1,69,68,0,81,68,0,17,68,0,16,64,0,16,
	140,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,140,0 	
};
*/

static const unsigned char data[] = 
{
	0,0,0,0,0,0,4,64,0,4,64,0,21,127,224,121,127,248,93,255,254,117,255,251,85,255,255,86,255,250,86,255,236,87,255,248,26,255,236,11,190,184,10,171,168,22,174,164,81,106,164,69,0,65,68,0,65,4,0,16,0,0,16,
	140,0,0,0,0,0,0,4,64,0,4,64,0,21,127,224,121,127,248,93,255,254,117,255,251,85,255,255,86,255,250,86,255,236,87,255,248,26,255,236,11,190,184,10,171,168,6,174,164,4,106,164,16,64,68,17,0,68,17,0,68,1,0,
	64,140,0,0,0,4,64,0,4,64,0,21,127,224,121,127,248,93,255,254,117,255,251,85,255,255,86,255,250,86,255,236,87,255,248,26,255,236,11,190,184,10,171,168,6,174,164,4,106,164,4,64,20,1,64,84,1,16,80,1,16,80,
	0,16,16,140,4,64,0,4,64,0,21,127,224,121,127,248,93,255,254,117,255,251,85,255,255,86,255,250,86,255,236,87,255,248,26,255,236,11,190,184,10,171,168,6,174,164,4,106,164,1,64,20,1,20,80,0,81,64,0,0,0,0,
	0,0,0,0,0,140,0 	
};

static const unsigned char sprite_title[] = 
{
	1,84,0,7,255,0,31,10,192,30,3,128,24,14,192,28,10,128,120,10,1,124,0,1,120,1,67,126,23,211,59,234,211,42,130,146,14,0,163,0,0,146,16,0,162,104,0,227,104,3,162,186,3,130,42,190,130,10,170,2,0,160,2,133,
	0,0,84,0,1,127,0,3,235,64,6,194,64,11,130,193,14,0,193,11,0,195,74,0,131,206,0,194,202,48,131,138,224,202,202,160,170,142,128,163,138,0,130,138,0,130,138,3,130,202,130,130,138,131,130,138,174,0,130,170,
	0,130,168,133,0,0,1,0,0,5,192,0,30,240,0,92,240,0,24,176,0,44,128,0,56,0,80,44,1,124,56,1,239,42,6,139,58,6,14,42,14,3,40,10,0,56,206,48,40,202,224,40,138,128,40,142,3,40,10,14,40,10,187,40,2,168,42,133,
	80,0,0,252,0,0,63,0,0,43,0,0,14,0,0,11,0,12,10,0,24,11,0,56,58,0,32,58,0,1,232,0,31,168,0,126,160,0,120,0,0,56,0,0,44,0,0,56,0,0,40,0,0,58,0,0,170,0,0,160,0,0,0,133,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,80,0,5,244,0,26,43,0,28,14,0,40,10,0,56,0,0,43,240,0,10,236,0,0,47,0,0,11,0,48,14,0,56,11,0,44,42,0,11,170,0,2,168,0,133,0,0,0,0,0,4,0,0,28,0,0,24,0,0,60,0,0,40,0,0,59,0,0,46,0,0,58,0,0,42,0,0,42,21,
	0,59,31,0,43,99,128,40,114,128,40,171,128,40,235,128,40,162,128,40,163,128,40,226,128,40,130,0,32,133,0,0,0,4,0,0,28,0,0,60,0,0,104,0,0,160,0,0,176,0,1,128,5,3,128,23,194,0,30,243,128,104,178,160,224,227,
	160,224,50,168,160,0,232,227,0,56,174,0,56,168,0,56,224,51,40,160,227,40,171,178,40,42,128,133,0,0,0,0,0,0,0,0,0,0,0,0,64,64,0,192,64,0,129,192,0,195,128,0,130,192,0,142,192,0,138,128,0,186,192,0,170,128,
	0,162,128,0,2,192,0,2,128,0,2,128,0,14,128,0,202,128,0,186,0,0,170,0,0,133,0 	
};


/*static const unsigned char charset[] = 
{
	21,127,127,127,127,127,127,127,85,255,255,255,255,255,255,255,84,253,253,253,253,253,253,253,127,127,127,127,127,127,127,127,255,255,255,255,255,255,255,255,253,253,253,253,253,253,253,253,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 	
};*/
byte anim=0;
byte frame=1;
byte jump=0,floor=0,newfloor=0;


void move_player (word x,word y) {
	
	byte aid;
	if (jump>1) {	
		//if (jump==2)
			aid = 0xf3;
		//else if (jump==2) aid=0xf0;
		
		asm("lda %v",aid);        //set sprite data
		asm ("sta $07f8"); 
		frame=0;
		anim=0;
	} else {
		
		if (frame==0) {
			aid = 0xf0;

			/*if (floor==0 || floor==2) aid = 0xf4;
			else aid=0xf0;*/
			if (anim==0) {
				asm("lda %v",aid);        //set sprite data
				asm ("sta $07f8"); 			
				anim=1;
			} else {
				asm ("inc $07f8");			
				++anim;
				if (anim==4) anim=0;
			}
				
			frame=6;
		} else --frame;
	}
	//*((char *) 0xd021)=0;
	
/*	*((char *) 0xd025)=0;
	
	*((char *) 0xd026)=1;
	*((char *) 0xd027)=12;
	
	*((char *) 0xd028)=0;*/
	
	*((char *) 0xd015)=1;

	*((char *) 0xd000)=x;
	*((char *) 0xd001)=y;
	
	*((char *) 0xd002)=x;
	*((char *) 0xd003)=y;

	*((char *) 0xD01c )=1;
	
	if (x>255) {
		*((char *) 0xd010)=1;
	}
	else
		*((char *) 0xd010)=0;
	
	
}
 void waitvsync() {
	
	 asm("wait:");
	 asm ("lda $d012");
	 asm ("cmp #$ff");
	 asm ("jne wait");
 }
 
/*void waitjingle(){
	int counter;
	counter=0;
	while (counter!=0) {
		if (!frame_ready) {
			frame_ready=1;
			waitvsync();							
			counter--;
		}
	}
}*/
void draw_level (byte *level) {
	int i=0;
	clrscr();
	/* db_ch=27;
     db_cell_color=10;db_w=40;db_h=8;db_y=0;db_x=0;
	 drawbox();
	 db_cell_color=11;db_w=40;db_h=8;db_y=8;db_x=0;
	 drawbox();
	 
	 db_cell_color=12;db_w=40;db_h=8;db_y=16;db_x=0;
	 drawbox();
*/
	

	drawlevel();
	while (level[i]!=END) {
		byte type=level[i++];
		db_x=level[i++];
		db_y=level[i++];
		db_w=level[i++];
		db_h=level[i++];		
		db_cell_color=13;
		db_ch=80;
		drawbox();
		
		
	}
}

int check_collision (short px,short py) {
	short xx=px/8-3;
	short yy=py/8-5;
	

	char *s = (char *) lookup[yy]+xx;

	char *o=s;

	
if (xx<0)	 return 0;
if (xx>37) return 0;
		
	     //if (*s==1)return 1;	
	//s++;if (*s==1) return 1;
	//s++;if (*s==1) return 1;
	if (*s>=80)return 1;	s++;		
	if (*s>=80)return 1;	s++;		
	if (*s>=80) return 1;
	
	s=o+40;
	
	if (*s>=80)return 1;	s++;		
	if (*s>=80)return 1;	s++;		
	if (*s>=80) return 1;
	//s++;if (*s==1) return 1;
	
	

/*	s=o+40;
		if (*s==43)return 1;	s++;		
	if (*s==43) return 1;
*/
	
	   //  if (*s==1)return 1;	
	//s++;if (*s==1) return 1;
	//s++;if (*s==1) return 1;
		

		return 0;
	
}

static const byte charset [] =
{
	0,60,124,76,92,64,124,0,0,0,0,124,76,124,76,0,0,120,120,72,124,76,124,0,0,0,0,120,88,64,120,0,0,112,120,92,76,76,124,0,0,124,124,64,112,64,124,0,0,0,124,124,64,112,64,0,0,124,124,64,92,76,124,0,0,0,0,76,
	124,76,76,0,0,48,0,48,48,48,48,0,0,60,60,4,4,4,60,0,0,0,0,76,120,76,76,0,0,0,0,96,96,96,124,0,0,108,124,76,76,76,76,0,0,0,0,108,108,92,76,0,0,124,124,76,76,76,124,0,0,0,0,124,76,124,64,0,0,124,124,76,76,
	92,116,0,0,0,0,124,76,120,76,0,0,124,124,64,124,12,124,0,0,0,0,124,124,16,16,0,0,76,76,76,76,76,124,0,0,0,0,76,76,76,48,0,0,76,76,76,76,124,108,0,0,102,102,24,24,102,102,0,0,0,0,76,124,12,124,0,0,126,126,
	6,24,96,126,0,0,60,60,32,32,32,60,0,0,0,0,0,0,24,24,0,0,60,60,4,4,4,60,0,0,0,0,0,24,24,8,16,0,0,0,60,60,0,0,0,0,0,0,0,0,0,0,0,0,0,0,124,76,76,124,0,0,0,0,24,56,24,60,0,0,120,124,12,124,64,124,0,0,120,124,
	12,28,12,124,0,0,0,0,76,76,124,12,0,0,60,124,64,124,12,124,0,0,60,124,64,124,76,124,0,0,0,0,124,12,28,12,0,0,120,124,76,124,76,124,0,0,120,124,76,124,12,124,0,255,255,255,255,255,255,255,255,12,204,63,
	207,63,63,243,4,0,48,195,255,255,255,255,195,12,48,51,255,255,255,243,196,0,204,48,255,255,255,252,15,48,12,204,255,255,255,207,19,51,12,51,255,255,255,252,49,0,195,51,255,255,255,60,0,0,12,51,252,255,
	255,204,19,5,6,0,5,9,0,0,1,20,84,88,0,16,17,1,0,21,86,64,65,1,80,97,1,16,21,20,0,70,5,64,128,16,20,20,24,0,5,70,64,69,85,0,64,4,20,24,0,21,21,0,80,97,0,24,0,16,20,20,0,80,96,0,64,37,41,229,233,229,255,
	255,195,4,8,37,41,37,41,37,41,0,0,0,0,
	
};

void screen_on () {
	asm ("sei");
	 asm ("LDA $D011");
	 asm ("ora #16");
     asm ("STA $D011");
	 asm ("cli");
}
void screen_off () {
	 asm ("sei");
	 asm ("LDA $D011");
	 asm ("and #$ef");
     asm ("STA $D011");
	 asm ("cli");
}
static byte bits[]={1,2,4,8,16,32,64,128};
	static byte cycle[]={0,12,15,1,1,15,12,0};
void show_title () {
	
	word j,w=64+24,i;
	musicid=1;
	rastermode=1;
	for (j=0;j<512;j++) {
		*((char *) 0x3c00+j)=sprite_title[j];		
	}
	
	*((char *) 0xd020)=11;
	*((char *) 0xd021)=11;
	
	*((char *) 0xd025)=8;
	*((char *) 0xd026)=10;

	for (j=0;j<8;j++) {
		*((char *) 0xd000+j*2)=w;
		*((char *) 0xd000+j*2+1)=70;
		
		*((char *) 0xD027+j)=5;		
		*((char *) 0x07f8+j)=0xf0+j;
		
		if (w>255)  *((char *) 0xd010)|=bits[j];
		
		w=w+24;
		
	}

	*((char *) 0xD01c)=0xff;
	*((char *) 0xd015)=0xff;
	//*((char *) 0xd010)=0;

	
	j= 9;
	 w=7;
	 textcolor(7);
	 gotoxy ((40-w)/2,j++);
	 cprintf ("code by");
	 textcolor(3);
	  w=16;
	 gotoxy ((40-w)/2,j++);
	 cprintf ("raffaele formato");
	 
	 j++;
	 
	 w=10;gotoxy ((40-w)/2,j++);
	 textcolor(7);
	 cprintf ("artwork by");
	 w=15;gotoxy ((40-w)/2,j++);
	 textcolor(3);
	 cprintf ("federico sesler");

	 j++;
	 
	 w=8;gotoxy ((40-w)/2,j++);
	 textcolor(7);
	 cprintf ("music by");
	 w=15;gotoxy ((40-w)/2,j++);
	 textcolor(3);
	 cprintf ("michael zuurman");
	 	 	 
		 
	 j++;
	 j++;
	 w=11;gotoxy ((40-w)/2,j++);
	 textcolor(5);
	 cprintf ("[@] in 2017");
	 	 	 

	 j+=3;
	 w=4;
	 w=27;gotoxy ((40-w)/2,j);
	 textcolor(1);
	 cprintf ("- fire in port 2 to start -");
	 
	 screen_on();
	 w=0;
	 do {
		i=joy();			
		gotoxy (6,j);
		w++;
		if (w/4>7	) w=0;
		textcolor(cycle[w/4]);

		cprintf ("- fire in port 2 to start -");
		waitvsync();
	} while (bit(i,16)==0);
	
	/*w=0;
	for (i=0;i<25;i++) {
	 if (w==0)
		textcolor(1);
	else 
		textcolor(0);
		w^=1;
		
	 gotoxy (8,j);
	 cprintf ("fire in port 2 to start");
	 
	 waitvsync();
	 
	}*/
/*	if (x>255) {
		*((char *) 0xd010)=1;
	}
	else
		*((char *) 0xd010)=0;	*/
	musicid=0;
	rastermode=0;
	screen_off();
}
int main (void)
{
	int counter=0;
	int  start_fc;
	int  power=0;
	int ljmp=0;
	int numj=sizeof (jtab)/sizeof(int);
	int numjlong=sizeof (jtablong)/sizeof(int);
	word j,i;
	int qi=0,w;
	short nt=0;
	int vel=5;
    int levid=0;
	short tq;
	short q=0;
    short x=0x80;
	short y=0x80;
	byte d;
	short e;
	int orgchar;
	
	short posx[] = {0,320,0};
	short posy[] = {8*8+29-2,8*8+29+64-2,8*8+29+128-2};
	short ext[] = {350,0,350};
	char dir[] = {1,2,1};
	
	
	int kill=10;
	
	screen_off();
	clrscr();
	x0=10; x1=50;
	
/*	list_of_rect[0]= 100;
	list_of_rect[1]= 200;
	*/
	/* while(1) {
		 
		 check_box();
		 j=joy();
		 
		x0++;x1++;
		gotoxy (0,0);
		cprintf ("%d x1:%d x2:%d 100 200",resbox,x0,x1);
		
	 }*/
	//orgchar = *((char *) 0xd018)*40;
  
	
	/*for (j=0;j<384;j++) {
		*((char *) 0x3000+j)=charset[j];
	}*/


	
	/*poke (56334,(peek(56334) & 254));
	poke (1,(peek(1)&251));
	*/
	for (j=0;j<sizeof (charset);j++) {
		//*((char *) 0x3000+j)=*((char*)53248 +j) ;
		*((char *) 0x3800+j)=charset[j] ;
	}
	
	for (j=60*8,i=0;j<62*8;j++,i++) {
		//*((char *) 0x3000+j)=*((char*)53248 +j) ;
		*((char *) 0x3800+i+80*8)=charset[j] ;
	}
	
	for (j=44*8,i=0;j<59*8;j++,i++) {
		//*((char *) 0x3000+j)=*((char*)53248 +j) ;
		*((char *) 0x3800+i+60*8)=charset[j] ;
	}
	
	
	for (j=48*8;j<49*8+9*8;j++) {
		//*((char *) 0x3000+j)=*((char*)53248 +j) ;
		*((char *) 0x3800+j)=charset[j-15*8] ;
	}
	
	
	
	for (j=31*8,i=0;j<32*8;j++,i++) {
		//*((char *) 0x3000+j)=*((char*)53248 +j) ;
		*((char *) 0x3800+i+45*8)=charset[j] ;
	}

	

	
	/*poke (1,(peek(1)|4));
	poke (56334, (peek(56334) | 1));
*/
	
	poke (53272,21);

	

	asm ("lda #$1e");//      ;charset at $3800
	asm ("sta $d018");


	init_raster();
	show_title();
	screen_off();
	
		asm("lda #200");	
	asm("ora #16");
	asm ("sta $d016");
	
	for (j=0;j<255;j++) {
		*((char *) 0x3c00+j)=data[j];
	}
	
	asm("lda #$01");
	asm ("sta $d015");
	asm("lda #$80");
	asm ("sta $d000");
	asm("sta $d001");



 
	draw_level(levels[levid]);
	 
	 x=posx[floor];
	 y=posy[floor];
	 d=dir[floor];
	 e=ext[floor];
	 
	 /*asm("lda #$01");
	 asm ("sta $d021");
	 
	 asm("lda #$00");
	 asm ("sta $d022");

	 asm("lda #$09");
	 asm ("sta $d023");
	 */
	 
	*((char *) 0xd025)=0;
	*((char *) 0xd026)=1;
	*((char *) 0xd027)=12;	
	*((char *) 0xd028)=0;
	 
	flip_h();
	
	screen_on();
restart:

	 while(1) {
		
		 j=joy();	
	
		 if (d==1) {x++;if (x==e) newfloor=1;}
		else if (d==2) {x--; if (x==e)newfloor=1;}
	
		qi=q/256;
		if (newfloor==1) {
			++floor;		
			if (floor>2) {
					floor=0;
					levid++;
					
					if (levels[levid]==0) {
						break;
					}
					if (levid%4==0) {
						clrscr();									
						musicid=4;
						rastermode=1;
						gotoxy (13,10);
						cprintf ("level complete");					
						waitjingle();
					}
					
					draw_level(levels[levid]);	
					musicid=0;
					rastermode=0;
					
					//screen_on();
			} else flip_h();
			
			x=posx[floor];
			y=posy[floor];
			d=dir[floor];
			e=ext[floor];
			newfloor=0;
		} else if (check_collision (x,y+qi)==1) {
			x=posx[floor];
			y=posy[floor];
			d=dir[floor];
			e=ext[floor];
			kill--;
			if (kill==0)
				break;
		}
		
		if (jump==0 && bit(j,16)) {
			jump=1;
			q=0;
			qi=0;
			nt=0;
			frame=1;
			anim=0;
			power=0;
		}
		if (jump==1) {
			power+=51;
			ljmp=power/256;			
			if (ljmp>=1)  {						
				ljmp=1;
				jump=2;
			}
		}
		if (jump==1&& bit(j,16)==0) {
			jump=2;			
		}
		
		if (jump>1) {
			if (ljmp==0) {
				q+=jtab[nt++];
				if (nt>=numj) {
					jump=0;
				}
			} else  {
				q+=jtablong[nt++];
				if (nt>=numjlong) {
					jump=0;
				}

			}
				
		}
			
		
		qi=q/256;
		if (!frame_ready) {
			frame_ready=1;
			waitvsync();
			move_player(x,y+qi);
		}
		
		
		 //asm ("jsr %v+3",musicplayer);
		// waitvsync();
		
		 textcolor(1);

		 gotoxy (9,2);		 
		 cprintf ("level %d of 10 lives %d ",levid+1,kill);
		
	
	}
	
	*((char *) 0xd015)=0;
	if (kill==0) {
	clrscr();
	gotoxy (5,9);
	cprintf ("          you lose          ");
	
	textcolor(1);
	musicid=3;
	rastermode=1;

	waitjinglewithkey();
	
	w=0;
	/*do {
		i=joy();			
		gotoxy (4,11);
		w++;
		if (w/4>7	) w=0;
		textcolor(cycle[w/4]);
		cprintf ("- fire in port 2 to try again -");
		waitvsync();
	} while (bit(i,16)==0);*/
	
	show_title();
	for (j=0;j<255;j++) {
		*((char *) 0x3c00+j)=data[j];
	}
	
	asm("lda #$01");
	asm ("sta $d015");
	asm("lda #$80");
	asm ("sta $d000");
	asm("sta $d001");	
	
	*((char *) 0xd025)=0;
	*((char *) 0xd026)=1;
	*((char *) 0xd027)=12;	
	*((char *) 0xd028)=0;
	
	flip_h();
	
	
	clrscr();									
	kill=10;
	levid=0;	
	floor=0;

    x=posx[floor];
    y=posy[floor];
    d=dir[floor];
    e=ext[floor];
	 
	draw_level(levels[levid]);	
	musicid=0;
	rastermode=0;
	screen_on ();
	goto restart;
	} else {
	clrscr();
	gotoxy (5,10);
	cprintf ("congratulation you did it!!! ");
	gotoxy (11,13);
	cprintf ("only %03d death ",kill);

	}
	for (;;);
    /* Done */
    return EXIT_SUCCESS;
}
