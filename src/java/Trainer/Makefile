# Makefile for UTAV coach
# 
# Requires antlr to be in your CLASSPATH
#

JAVAC=javac
#OPTSC=-Xlint:unchecked
OPTSC=
ANTLR=java antlr.Tool

all:
	${ANTLR} LogDrawParser.g
	${JAVAC} ${OPTSC} *.java

clean: 
	rm -f *.class LogDrawParser.java LogDrawTokenTypes.java LogDrawTokenTypes.txt *~


