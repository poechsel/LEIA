CFLAGS = -Wall -O2 -std=c++11
LDFLAGS = -lSDL2 -lpthread -lstdc++
EXEC = LEIA
SRC = $(wildcard *.cpp)
OBJS = $(SRC:.cpp=.o)
 
all : $(EXEC) 
 
$(EXEC) : $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

main.o: main.cpp debug.h screen.h simulateur.h structs.h utilities.h sdlscreen.h
	$(CXX) $(CFLAGS) -o $@ -c $<
utilities.o : utilities.cpp structs.h utilities.h
	$(CXX) $(CFLAGS) -o $@ -c $<
debug.o: debug.cpp structs.h debug.h
	$(CXX) $(CFLAGS) -o $@ -c $<
sdlscreen.o: sdlscreen.cpp structs.h sdlscreen.h screen.h
	$(CXX) $(CFLAGS) -o $@ -c $<
screen.o: screen.cpp structs.h screen.h
	$(CXX) $(CFLAGS) -o $@ -c $<
simulateur.o: simulateur.cpp structs.h debug.h
	$(CXX) $(CFLAGS) -o $@ -c $<
structs.o: structs.cpp
	$(CXX) $(CFLAGS) -o $@ -c $<

#%.o : %.cpp
#	$(CXX) $(CFLAGS) -o $@ -c $<
clean :
	@rm *.o
cleaner : clean
	@rm $(EXEC)

