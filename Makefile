CC=g++

INC=-DIL_STD -I/opt/ibm/ILOG/CPLEX_Studio128/cplex/include -I/opt/ibm/ILOG/CPLEX_Studio128/concert/include

LIB =-L/opt/ibm/ILOG/CPLEX_Studio128/concert/lib/x86-64_linux/static_pic/ -L/opt/ibm/ILOG/CPLEX_Studio128/cplex/lib/x86-64_linux/static_pic/

CFLAGS=-c -std=c++11 -O2 -Wno-ignored-attributes



LDFLAGS=-lm -lilocplex -lconcert -lcplex -lpthread -ldl -g

SOURCES= Source/Node.cpp Source/Graph.cpp Source/Input.cpp Source/Group.cpp Source/Solution.cpp Source/Heuristic.cpp ExactSources/Environment.cpp ExactSources/Opt.cpp ExactSources/OptCplex.cpp ExactSources/ProblemSolver.cpp  main.cpp

#pega os paths de sources e altera .cpp para .o
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=Solve

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIB) -o $@

all: $(SOURCES) $(EXECUTABLE)

.cpp.o:
	$(CC) $(CFLAGS) $(INC) $< -o $@

clean:
	rm -rf *o Solve rm -rf Source/*o
