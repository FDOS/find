goto %1

:watcom
rem ############# WATCOM ########################
set PATH=C:\bin;C:\watcom\binw;%PATH%
rem # -we treat all warnings as errors
rem # -wx set warning level to max
rem # -zq operate quietly
rem # -fm[=map_file]  generate map file
rem # -fe=executable  name executable file
rem # -m{t,s,m,c,l,h}  memory model

set WATCOM=C:\watcom
set INCLUDE=C:\watcom\h
set CC=wcl
set COMFLAGS=-mt -lt
set EXEFLAGS=-mc
set CFLAGS=-bt=DOS -D__MSDOS__ -oas -zp1 -s -0 -wx -we -zq -fm %EXEFLAGS% -fe=
goto doit

:tcc
rem ############# TURBO_C ########################
set PATH=C:\bin;C:\tc\bin;%PATH%
rem # -w warn -M create map -f- no floating point -Z register optimize
rem # -O jump optimize -k- no standard stack frome -K unsigned char
rem # -exxx executable name (must be last) -mt tiny (default is small)
rem # -N stack checking -a- byte alignment  -ln no default libs
rem # -lt create .com file -lx no map file ...

set CC=tcc
set COMFLAGS=-mt -lt -Z -O -k-
set EXEFLAGS=-mc -N -Z -O -k-
set CFLAGS=-w -M -f- -a- -K -ln %EXEFLAGS% -e
rem tcc looks for includes from the current directory, not the location of the
rem file that's trying to include them, so add kitten's location
set CFLAGS=-I../kitten -I../tnyprntf %CFLAGS%
goto doit


:doit
set EXTRA_OBJS=

set EXTRA_OBJS=%EXTRA_OBJS% tnyprntf.obj
rem # if you want to build without tnyprntf comment the above and uncomment
rem the following
rem set CFLAGS=-DNOPRNTF %CFLAGS%

set EXTRA_OBJS=%EXTRA_OBJS% kitten.obj
rem # if you want to build without kitten comment the above and uncomment
rem the following
rem set CFLAGS=-DNOCATS %CFLAGS%

set UPXARGS=upx --8086 --best
rem if you don't want to use UPX set
rem     UPXARGS=-rem
rem if you use UPX: then options are
rem     --8086 for 8086 compatibility
rem   or
rem     --best for smallest

make -C src
