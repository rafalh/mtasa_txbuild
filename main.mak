TXBUILD_DIR := $(dir $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))

# Load useful macros
include $(TXBUILD_DIR)utils.mak
include $(TXBUILD_DIR)commands.mak
include $(TXBUILD_DIR)helpers.mak

# Load configuration
include $(TXBUILD_DIR)config.mak
ifneq ($(wildcard ../../config.mak),)
  include ../../config.mak
endif
ifneq ($(wildcard ../config.mak),)
  include ../config.mak
endif
ifneq ($(wildcard config.mak),)
  include config.mak
endif

# Check if configuration is valid
ifeq ($(wildcard $(RESOURCES_PATH)),) 
  $(error RESOURCES_PATH is invalid: $(RESOURCES_PATH))
endif

# Setup TXMAIN_FLAGS
TXMAIN_FLAGS := -p$(PREPROCESS) -c$(COMPILE) -u$(UTF8_BOM) -j$(JOIN) -e$(PROTECT) -s$(STRING_LIST)
