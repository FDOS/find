
all: find.exe

# Watcom has different flag for output file name at compile vs link
_CFLAGS = $(subst -fe=,-fo=,$(CFLAGS))

kitten.obj: ../kitten/kitten.c
	$(CC) $(_CFLAGS)$@ -c $^

tnyprntf.obj: ../tnyprntf/tnyprntf.c
	$(CC) $(_CFLAGS)$@ -c $^

find.exe find.com: find.c find_str.c $(EXTRA_OBJS)
	$(CC) $(CFLAGS)$@ $^ $(LDFLAGS)
	$(UPXARGS) $@

clean:
	$(RM) find.exe
	$(RM) find.com
	$(RM) *.o
	$(RM) *.obj
	$(RM) *.map
