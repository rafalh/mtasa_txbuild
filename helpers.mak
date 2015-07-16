# createDir(path)
createDir = $(Q)$(MKDIR) "$(call nativePath,$(1))"

# copyFile(src,dest)
ifdef WINDIR
 copyFile = $(Q)$(COPY) "$(call nativePath,$(1))" "$(call nativePath,$(2))" >NUL
else
 copyFile = $(Q)$(COPY) "$(call nativePath,$(1))" "$(call nativePath,$(2))"
endif

# preprocessFile(src,dest)
preprocessFile = $(Q)$(LUAPP) -o "$(call nativePath,$(2))" $(DEFINES) "$(call nativePath,$(1))"

# compileFile(dir,path)
compileFile = $(Q)$(CD) $(1) && $(LUAC) $(LUAC_FLAGS) -o "$(call nativePath,$(2))" "$(call nativePath,$(2))"

# compileFile2(dir,dstpath,srcpath)
compileFile2 = $(Q)$(CD) $(1) && $(LUAC) $(LUAC_FLAGS) -o "$(call nativePath,$(2))" "$(call nativePath,$(3))"

# checkLuaSyntax(path)
checkLuaSyntax = $(Q)$(LUAC_PARSE) "$(call nativePath,$(1))"

# addUtf8BOM(path)
addUtf8BOM = $(Q)$(ADDUTF8BOM) "$(call nativePath,$(1))"

# genStrListFromLua(output,path,type)
genStrListFromLua = $(Q)$(TXSTRLIST) -o "$(call nativePath,$(1))" -l $(3) "$(call nativePath,$(2))"

# protectFile(output,path)
protectFile = $(Q)$(TXPROTECT) -l "$(call nativePath,$(LOADER_PATH))" -o "$(call nativePath,$(1))" "$(call nativePath,$(2))"

# delFile(path)
delFile = $(Q)$(RM) "$(call nativePath,$(1))"
