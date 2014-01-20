# Makefile
#
# OS dependend settings for OS/2 3.x and 4.x
# (c) 1999 by Dirk Ohme

#---| project |-----------------------------------------------------------------
NAME    = shttpd
VERSION = 0.06
#DEBUG   = yes
VACPP   = yes

#---| environment |-------------------------------------------------------------
BIN_EXT = .exe
DLL_EXT = .dll
LIB_EXT = .lib
OBJ_EXT = .obj
CP      = copy
MV      = move
RM      = del
STRIP   = rem
ZIP     = zip -r9

#---| compiler and linker |-----------------------------------------------------
AR      = ilib /nologo
CC      = icc
CDEFS   = /DPROG_NAME=\"$(NAME)\" /DPROG_VERSION=\"$(VERSION)\"
!ifdef DEBUG
CFLAGS  = /c /DDEBUG /G5 /Gm+ /I..\..\include /I..\zlib /Q /Ti+ /Tm+
CDFLAGS  = /c /DDEBUG /G5 /Ge- /Gm+ /I..\..\include /I..\zlib /Q /Ti+ /Tm+
!else
CFLAGS  = /c /G5 /Gi+ /Gm+ /I..\..\include /I..\zlib /O /Oi /Ol /Q
CDFLAGS  = /c /G5 /Ge- /Gi+ /Gm+ /I..\..\include /I..\zlib /O /Oi /Ol /Q
!endif
LN      = $(CC)
!ifdef DEBUG
LFLAGS  = /B"/e:2" /Gm+ /Q /Ti+ /Fe
!else
LFLAGS  = /B"/e:2" /Gm+ /Q /Fe
!endif
RANLIB  = @echo static library:
        #---| Visual Age C++ |------------------------------------------
        #---| note: /e:2 = /exepack:2 works only under OS/2 >= 3.x! |---
!ifdef VACPP
!  ifdef DEBUG
DEBUGGER=ipmd
LD      = ilink /nofree /deb /dll /e:2 /noi /nol /packc /packd /pm:VIO\
 /se:1024 /st:32768
LD_END  =
!  else
DEBUGGER=
LD      = ilink /nofree /dll /e:2 /noi /nol /packc /packd /pm:VIO\
 /se:1024 /st:32768
LD_END  =
!  endif
        #---| IBM C/C++ Set/2 |-----------------------------------------
!else
!  ifdef DEBUG
DEBUGGER=ipmd
LD      = link386 /deb /noi /nol /pm:VIO /se:1024 /st:32768
LD_END  = ;
!else
DEBUGGER=
LD      = link386 /noi /nol /pm:VIO /se:1024 /st:32768
LD_END  = ;
!  endif
!endif
        #---| common |--------------------------------------------------
LIBS    = so32dll.lib tcp32dll.lib ..\zlib\libz$(LIB_EXT)

#---[ OpenSSL ]-----------------------------------------------------------------
SSL_MOD = mod_ssl
SSL_DLL = ..\..\bin\$(SSL_MOD)$(DLL_EXT)
SSL_INCL= /I..\..\..\ssl\include
SSL_LIBS= ..\..\..\ssl\lib\ssleay2.lib

#---[ Oracle RDBMS with OCI ]---------------------------------------------------
DB_MOD  = mod_ora
DB_DLL  = ..\..\bin\$(DB_MOD)$(DLL_EXT)
DB_HOME = E:\PUBLIC\PROGRAMS\ORACLE
DB_INCL = /IE:\PUBLIC\PROGRAMS\ORACLE\RDBMS72\OCI
DB_LIBS = E:\PUBLIC\PROGRAMS\ORACLE\RDBMS72\OCI\ORA_D72O.LIB

#---| Java compiler |-----------------------------------------------------------
JAVAC   = javac
JAVAH   = javah
!ifdef DEBUG
J_OPT   = -g -debug
!else
J_OPT   = -O
!endif
JAR     = jar c0f

#---| global dependencies |-----------------------------------------------------
.SUFFIXES: .c .class .java $(OBJ_EXT)
.c$(OBJ_EXT):
        $(CC) $(CFLAGS) $(CDEFS) $<
.java.class:
        $(JAVAC) $(J_OPT) $<

#===| end of file 'os.mak' |====================================================
