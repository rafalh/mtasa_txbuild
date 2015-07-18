# Default configuration
PREPROCESS     := 1
COMPILE        := 1
UTF8_BOM       := 0
JOIN           := 0
PROTECT        := 0
STRING_LIST    := 0
DEFINES        :=
LOADER_PATH    := $(TXBUILD)/loader.lua
ifdef windir
 RESOURCES_PATH := C:/Program\ Files\ (x86)/MTA\ San\ Andreas\ 1.4/server/mods/deathmatch/resources/
else
 RESOURCES_PATH := /mnt/c/Program\ Files\ (x86)/MTA\ San\ Andreas\ 1.4/server/mods/deathmatch/resources/
endif
TEMP_DIR       := build
LUAC_FLAGS     := -d
