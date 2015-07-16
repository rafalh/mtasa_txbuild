# MAKEFILE HACK: http://devicesoftware.blogspot.com/2010/06/handling-path-with-white-spaces-in.html
NULLSTR := # creating a null string
SPACE   := $(NULLSTR) # end of the line

ifdef WINDIR

 SEP: = \
 APPEXT := .exe

 # nativePath(path)
 nativePath = $(subst /,\,$(subst \$(SPACE),$(SPACE),$(1)))

else

 SEP := /
 APPEXT :=

 # nativePath(path)
 nativePath = $(subst \,/,$(subst \$(SPACE),$(SPACE),$(1)))

endif
