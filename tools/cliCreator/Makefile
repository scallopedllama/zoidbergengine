
# The default task here is to make the assets.zbe file. It needs to make sure all the tools are built first
all: cliCreator

cliCreator: cliCreator.cpp
	g++ -o cliCreator cliCreator.cpp ./lib/tinyxml/tinyxml.cpp ./lib/tinyxml/tinyxmlparser.cpp ./lib/tinyxml/tinyxmlerror.cpp ./lib/tinyxml/tinystr.cpp

clean:
	rm -f cliCreator
