SYS	= c64
CLIB = --lib $(SYS).lib
CL   = cl65
CC   = cc65
AS   = ca65
LD   = ld65
EXELIST	= sheep

# --------------------------------------------------------------------------
# Generic rules

sheep:
	@echo "src" $<
	@$(CC)   --codesize 500 -T -g -t $(SYS) sheep.c -o ..\objs\sheep.s
	@$(AS) ..\objs\sheeplib.s
	@$(AS)  -t $(SYS) ..\objs\sheep.s
	@$(LD) -t $(SYS)  -o ..\release\$@.prg ..\objs\sheep.o ..\objs\sheeplib.o $(CLIB)
	copy sheeplib.s ..\objs\sheeplib.s
	@exomizer sfx 2061 ..\release\sheep.prg ..\assets\music.prg -o ..\release\$@_final.prg


clean:
	del *.o
	del ..\objs\*.o
	del ..\release\*.prg



