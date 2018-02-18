TXBUILD_DIR := $(dir $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))

TXMAIN_PATH        := $(TXBUILD_DIR)bin/txbuild$(APPEXT)
TXSTRLIST_PATH     := $(TXBUILD_DIR)bin/txstrlist$(APPEXT)
TXPROTECT_PATH     := $(TXBUILD_DIR)bin/txprotect$(APPEXT)
ADDUTF8BOM_PATH    := $(TXBUILD_DIR)bin/addutf8bom$(APPEXT)

ifndef VERBOSE
 Q := @
else
 Q :=
endif

ifdef windir
 COPY          := copy /Y
 MKDIR         := mkdir
 RM            := del /F /Q
 CD            := cd /D
else
 COPY          := cp
 MKDIR         := mkdir -p
 RM            := rm -rf
 CD            := cd
endif

ECHO          := $(Q)echo

ifdef windir
 LUA           := "$(call nativePath,$(TXBUILD_DIR))luajit\luajit.exe"
 LUAC_PARSE    := "$(call nativePath,$(TXBUILD_DIR))lua51\luac5.1.exe" -p
else
 LUA           := "luajit"
 LUAC_PARSE    := "luac5.1" -p
endif

LUAC          := "$(call nativePath,$(TXBUILD_DIR)luac_mta$(APPEXT))"
LUAPP         := $(LUA) "$(TXBUILD_DIR)preprocess.lua"
TXMAIN        := "$(call nativePath,$(TXMAIN_PATH))"
TXPROTECT     := "$(call nativePath,$(TXPROTECT_PATH))"
TXSTRLIST     := "$(call nativePath,$(TXSTRLIST_PATH))"
ADDUTF8BOM    := "$(call nativePath,$(ADDUTF8BOM_PATH))"
