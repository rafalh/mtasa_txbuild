# MAKEFILE HACK: http://devicesoftware.blogspot.com/2010/06/handling-path-with-white-spaces-in.html
NULLSTR := # creating a null string
SPACE   := $(NULLSTR) # end of the line

# nativePath(path)
nativePath = $(subst /,\,$(subst \$(SPACE),$(SPACE),$(1)))

# createDir(path)
createDir = $(MKDIR) "$(call nativePath,$(1))"

# copyFile(src,dest)
copyFile = $(COPY) "$(call nativePath,$(1))" "$(call nativePath,$(2))" >NUL

# preprocessFile(src,dest)
preprocessFile = $(LUAPP) -o "$(call nativePath,$(2))" $(DEFINES) "$(call nativePath,$(1))"

# compileFile(dir,path)
compileFile = $(CD) $(1) && $(LUAC) $(LUAC_FLAGS) -o "$(call nativePath,$(2))" "$(call nativePath,$(2))"

# checkLuaSyntax(path)
checkLuaSyntax = $(LUAC_PARSE) "$(call nativePath,$(1))"

# addUtf8BOM(path)
addUtf8BOM = $(ADDUTF8BOM) "$(call nativePath,$(1))"

# genStrListFromLua(output,path,type)
genStrListFromLua = $(TXSTRLIST) -o "$(call nativePath,$(1))" -l $(3) "$(call nativePath,$(2))"

# protectFile(output,path)
protectFile = $(TXPROTECT) -l "$(call nativePath,$(LOADER_PATH))" -o "$(call nativePath,$(1))" "$(call nativePath,$(2))"

# delFile(path)
delFile = $(RM) "$(call nativePath,$(1))"
