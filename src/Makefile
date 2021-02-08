# Minimal Makefile for FIND

all: find.exe
# can be find.com for tiny model

UPX=upx --8086
# if you don't want to use UPX set
#     UPX=-rem (DOS)
#   or
#     UPX=true (Unix)
# if you use UPX: then options are
#     --8086 for 8086 compatibility
#   or
#     --best for smallest

find.exe find.com: find.c find_str.c ../kitten/kitten.c
	$(CC) $(CFLAGS)$@ $^ $(LDFLAGS)
	$(UPX) $@

clean:
	$(RM) find.exe
	$(RM) find.com
	$(RM) *.o
	$(RM) *.obj
