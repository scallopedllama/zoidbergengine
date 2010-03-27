
# Build everything in this folder
all: numPrefix16 numPrefix32 numPrefix8 objParser

# numPrefix tools
numPrefix16: numPrefix16.c
	cc -o numPrefix16 numPrefix16.c

numPrefix32: numPrefix32.c
	cc -o numPrefix32 numPrefix32.c

numPrefix8: numPrefix8.c
	cc -o numPrefix8 numPrefix8.c

objParser: objParser.cpp
	g++ -o objParser objParser.cpp ./lib/tinyxml/tinyxml.cpp ./lib/tinyxml/tinyxmlparser.cpp ./lib/tinyxml/tinyxmlerror.cpp ./lib/tinyxml/tinystr.cpp

clean:
	rm -f numPrefix16 numPrefix8 numPrefix32 objParser
