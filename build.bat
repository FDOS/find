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
set TARGET=find.exe
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
set CFLAGS=-I../kitten %CFLAGS%
set TARGET=find.exe
goto doit


:doit
rem We use GNU make for all targets
make -C src %TARGET%
