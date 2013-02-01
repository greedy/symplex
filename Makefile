OPTFLAGS = -O3 -flto
CXXFLAGS = $(OPTFLAGS) -Wall -std=c++11 -MMD -MP -fpermissive

BTOR_BASE = boolector-1.5.118-with-sat-solvers

CPPFLAGS = -I$(BTOR_BASE)/boolector


symplex: main.o grammar.o lex.o parser.o ast.o eval.o $(BTOR_BASE)/boolector/libboolector.a
	g++ $(OPTFLAGS) -o $@ $^ $(BTOR_BASE)/boolector/libboolector.a $(BTOR_BASE)/lingeling/liblgl.a $(BTOR_BASE)/minisat/build/release/lib/libminisat.a $(BTOR_BASE)/picosat/libpicosat.a

clean:
	rm -f *.o *.d symplex grammar.cpp lex.cpp

reallyclean: clean
	rm -rf $(BTOR_BASE)

-include *.d

%.cpp: %.rl
	ragel -C -G2 -o $@ $<

%.cpp: %.y
	lemon T=lempar.cpp $<
	mv $(basename $@).c $@
	sed -i -e 's@"$(basename $@).c"@"$@"@' $@

main.cpp: $(BTOR_BASE)/boolector/boolector.h

grammar.h: grammar.cpp

%.c : %.y

$(BTOR_BASE)/boolector/libboolector.a $(BTOR_BASE)/boolector/boolector.h:
	tar xzf $(BTOR_BASE).tar.gz
	cd $(BTOR_BASE) && ./build.sh

