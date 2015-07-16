TXBUILD_DIR := $(dir $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))

TXMAIN_PATH        := $(TXBUILD_DIR)bin$(SEP)txbuild$(APPEXT)
TXSTRLIST_PATH     := $(TXBUILD_DIR)bin$(SEP)txstrlist$(APPEXT)
TXPROTECT_PATH     := $(TXBUILD_DIR)bin$(SEP)txprotect$(APPEXT)
ADDUTF8BOM_PATH    := $(TXBUILD_DIR)bin$(SEP)addutf8bom$(APPEXT)

ifndef VERBOSE
 Q := @
else
 Q :=
endif

ifdef WINDIR
 COPY          := copy /Y
 MKDIR         := mkdir
 RM            := del /F /Q
 CD            := cd /D
else
 COPY          := cp
 MKDIR         := mkdir
 RM            := rm -rf
 CD            := cd
endif

ECHO          := $(Q)echo

ifdef WINDIR
 LUA           := "$(TXBUILD_DIR)luajit\luajit.exe"
 LUAC_PARSE    := "$(TXBUILD_DIR)lua51\luac5.1.exe" -p
else
 LUA           := "luajit"
 LUAC_PARSE    := "luac5.1" -p
endif

LUAC          := "$(TXBUILD_DIR)luac_mta$(APPEXT)"
LUAPP         := $(LUA) "$(TXBUILD_DIR)preprocess.lua"
TXMAIN        := "$(TXMAIN_PATH)"
TXPROTECT     := "$(TXPROTECT_PATH)"
TXSTRLIST     := "$(TXSTRLIST_PATH)"
ADDUTF8BOM    := "$(ADDUTF8BOM_PATH)"
