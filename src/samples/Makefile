# Makefile
#
# makefile for the samples
# (c) 1999 by Dirk Ohme

#---| includes |----------------------------------------------------------------
include ..\..\include\os.mak

#---| project |-----------------------------------------------------------------
MODULES =\
 ..\..\bin\mod_stat$(DLL_EXT)\
 ..\..\bin\mod_test$(DLL_EXT)

#---| local dependencies |------------------------------------------------------
all:    dir $(MODULES)
clean:  dir
        -$(RM) *$(OBJ_EXT) >> nul 2>&1
delete: clean
        -$(RM) $(MODULES) >> nul 2>&1
dist:

#---| project dependencies |----------------------------------------------------
dir:
        @echo ---[ samples ]-------------------------------\
---------------------------------

..\..\bin\mod_stat$(DLL_EXT): mod_stat$(OBJ_EXT)
        $(LD) mod_stat$(OBJ_EXT), ..\..\bin\mod_stat$(DLL_EXT), nul, ,\
              ..\..\include\mod_all.def $(LD_END)
mod_stat$(OBJ_EXT): mod_stat.c
        $(CC) $(CDFLAGS) $(CDEFS) mod_stat.c

..\..\bin\mod_test$(DLL_EXT): mod_test$(OBJ_EXT)
        $(LD) mod_test$(OBJ_EXT), ..\..\bin\mod_test$(DLL_EXT), nul, ,\
              ..\..\include\mod_all.def $(LD_END)
mod_test$(OBJ_EXT): mod_test.c
        $(CC) $(CDFLAGS) $(CDEFS) mod_test.c

#===| end of file 'samples.mak' |===============================================
