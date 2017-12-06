# only works with the Java extension of yacc:
# byacc/j from http://troi.lincom-asg.com/~rjamison/byacc/

JFLEX  = java -jar JFlex.jar
BYACCJ = ./yacc.linux -tv -J
JAVAC  = javac
JAVA  = java

# targets:

all: Parser.class

run: Parser.class
	java Parser

build: clean Parser.class

clean:
	rm -f *~ *.class Yylex.java Parser.java y.output

success:
	$(JAVA) -cp . Parser tests_success/$(filename)

fail:
	$(JAVA) -cp . Parser tests_fail/$(filename)

Parser.class: TS_entry.java TabSimb.java Yylex.java Parser.java
	$(JAVAC) Parser.java

Yylex.java: lexicoE4.flex
	$(JFLEX) lexicoE4.flex

Parser.java: sintaticoE4.y
	$(BYACCJ) sintaticoE4.y
