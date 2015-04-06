TXBUILD_DIR := $(dir $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))

TXMAIN_PATH        := $(TXBUILD_DIR)bin\txbuild.exe
TXSTRLIST_PATH     := $(TXBUILD_DIR)bin\txstrlist.exe
TXPROTECT_PATH     := $(TXBUILD_DIR)bin\txprotect.exe
ADDUTF8BOM_PATH    := $(TXBUILD_DIR)bin\addutf8bom.exe

ifndef VERBOSE
Q := @
else
Q :=
endif

COPY          := $(Q)copy /Y
MKDIR         := $(Q)mkdir
RM            := $(Q)del /F /Q
CD            := $(Q)cd /D
ECHO          := $(Q)echo
LUA           := $(Q)"$(TXBUILD_DIR)luajit\luajit.exe"
LUAC          := $(Q)"$(TXBUILD_DIR)luac_mta.exe"
LUAC_PARSE    := $(Q)"$(TXBUILD_DIR)lua51\luac5.1.exe" -p
LUAPP         := $(Q)$(LUA) "$(TXBUILD_DIR)preprocess.lua"
TXMAIN        := $(Q)"$(TXMAIN_PATH)"
TXPROTECT     := $(Q)"$(TXPROTECT_PATH)"
TXSTRLIST     := $(Q)"$(TXSTRLIST_PATH)"
ADDUTF8BOM    := $(Q)"$(ADDUTF8BOM_PATH)"
