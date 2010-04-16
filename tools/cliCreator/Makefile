
# The default task here is to make the cliCreator tool. 
all: cliCreator

cliCreator: cliCreator.cpp
	g++ -o cliCreator -Wall -Wextra cliCreator.cpp ./lib/tinyxml/tinyxml.cpp ./lib/tinyxml/tinyxmlparser.cpp ./lib/tinyxml/tinyxmlerror.cpp ./lib/tinyxml/tinystr.cpp

clean:
	rm -f cliCreator

