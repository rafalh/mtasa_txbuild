$(info Toxic Build 2.4)

TXBUILD_DIR := $(dir $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))
include $(TXBUILD_DIR)main.mak

ifeq ($(PROJECT_DIR),)
  OUTPUT := $(RESOURCES_PATH)$(PROJECT_NAME)
else
  OUTPUT := $(RESOURCES_PATH)$(PROJECT_DIR)/$(PROJECT_NAME)
endif

ifndef TARGET_PREFIX
  TARGET_PREFIX := 
endif

FINAL_TARGETS := meta.xml
TARGETS_STR :=
TARGETS_LUA :=
TARGETS := meta.xml
TEMP_DIR:= build

ifeq ($(wildcard $(TEMP_DIR)/Makefile.auto),)
 $(PROJECT_NAME): $(TEMP_DIR)/Makefile.auto
	$(MAKE)

 $(TEMP_DIR):
	$(MKDIR) "$(call nativePath,$(TEMP_DIR))"

 $(TEMP_DIR)/Makefile.auto: | meta.xml $(TEMP_DIR)
	$(TXMAIN) $(TXMAIN_FLAGS)
else
 $(PROJECT_NAME): $(TARGET_PREFIX)all
 include $(TEMP_DIR)/Makefile.auto
 
 .PHONY: $(PROJECT_NAME) $(TARGET_PREFIX)all $(TARGET_PREFIX)clean
endif

.PHONY: deploy
deploy:
# Dry run: -n
	ssh "$(DEPLOY_HOST)" mkdir -p "$(DEPLOY_DIR)/$(PROJECT_DIR)/$(PROJECT_NAME)"
	rsync -ruvz $(RSYNC_FLAGS) "$(call nativePath,$(OUTPUT))/" "$(DEPLOY_HOST):$(DEPLOY_DIR)/$(PROJECT_DIR)/$(PROJECT_NAME)/"
