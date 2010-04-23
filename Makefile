#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/ds_rules

export TARGET		:=	bin/$(shell basename $(CURDIR))
export TOPDIR		:=	$(CURDIR)
export SOURCES		:=	source
export CLICREATOR	:=	tools/cliCreator

.PHONY: $(TARGET).arm7 $(TARGET).arm9

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: $(TARGET).nds
testing: $(TARGET)_testing.nds

#---------------------------------------------------------------------------------
$(TARGET).nds	:	$(TARGET).arm7 $(TARGET).arm9
	ndstool	-c $(TARGET).nds -7 $(TARGET).arm7 -9 $(TARGET).arm9
	dlditool $(TOPDIR)/tools/mpcf.dldi $(TARGET).nds
	
$(TARGET)_testing.nds	:	clean $(TARGET).arm7 $(TARGET)_testing.arm9
	ndstool	-c $(TARGET)_testing.nds -7 $(TARGET).arm7 -9 $(TARGET)_testing.arm9
	dlditool $(TOPDIR)/tools/mpcf.dldi $(TARGET)_testing.nds

#---------------------------------------------------------------------------------
$(TARGET).arm7	: $(SOURCES)/arm7/$(TARGET).elf
$(TARGET).arm9	: $(SOURCES)/arm9/$(TARGET).elf
$(TARGET)_testing.arm9	: $(SOURCES)/arm9/$(TARGET)_testing.elf

#---------------------------------------------------------------------------------
$(SOURCES)/arm7/$(TARGET).elf:
	$(MAKE) -C $(SOURCES)/arm7

#---------------------------------------------------------------------------------
$(SOURCES)/arm9/$(TARGET).elf:
	$(MAKE) -C $(SOURCES)/arm9
	
$(SOURCES)/arm9/$(TARGET)_testing.elf:
	$(MAKE) OTHERCFLAGS=-D\ ZBE_TESTING TARGET=$(TARGET)_testing -C $(SOURCES)/arm9

#---------------------------------------------------------------------------------
assets:
	$(MAKE) -C $(CLICREATOR)

#---------------------------------------------------------------------------------
clean:
	$(MAKE) -C $(SOURCES)/arm9 clean
	$(MAKE) -C $(SOURCES)/arm7 clean
	rm -f $(TARGET).nds $(TARGET).arm7 $(TARGET).arm9
