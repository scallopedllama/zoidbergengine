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

.PHONY: $(TARGET).arm7 $(TARGET).arm9

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: $(TARGET).nds

#---------------------------------------------------------------------------------
$(TARGET).nds	:	$(TARGET).arm7 $(TARGET).arm9
	ndstool	-c $(TARGET).nds -7 $(TARGET).arm7 -9 $(TARGET).arm9
	$(TOPDIR)/tools/cliCreator/cliCreator.sh $(TARGET).zbe
	dlditool $(TOPDIR)/tools/mpcf.dldi $(TARGET).nds

#---------------------------------------------------------------------------------
$(TARGET).arm7	: $(SOURCES)/arm7/$(TARGET).elf
$(TARGET).arm9	: $(SOURCES)/arm9/$(TARGET).elf

#---------------------------------------------------------------------------------
$(SOURCES)/arm7/$(TARGET).elf:
	$(MAKE) -C $(SOURCES)/arm7
	
#---------------------------------------------------------------------------------
$(SOURCES)/arm9/$(TARGET).elf:
	$(MAKE) -C $(SOURCES)/arm9

#---------------------------------------------------------------------------------
clean:
	$(MAKE) -C $(SOURCES)/arm9 clean
	$(MAKE) -C $(SOURCES)/arm7 clean
	rm -f $(TARGET).nds $(TARGET).arm7 $(TARGET).arm9
