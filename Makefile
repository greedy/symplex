CXXFLAGS = -g -Wall -std=c++11 -MMD -MP -fpermissive

main: main.o grammar.o lex.o parser.o ast.o eval.o
	g++ -g `pkg-config --libs cvc3` -o $@ $^

clean:
	rm *.o main grammar.cpp lex.cpp

-include *.d

%.cpp: %.rl
	ragel -C -G2 -o $@ $<

%.cpp: %.y
	lemon T=lempar.cpp $<
	mv $(basename $@).c $@
	sed -i -e 's@"$(basename $@).c"@"$@"@' $@

grammar.h: grammar.cpp

%.c : %.y
