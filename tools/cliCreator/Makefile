
# The default task here is to make the assets.zbe file
all: assets.zbe

# that depends on the gfx folder not having changed and the bin folder being built
# then running the cliCreator.sh script
assets.zbe: gfx bin
	bash cliCreator.sh

# build the bin folder by simply running make in there
bin:
	$(MAKE) -C bin
