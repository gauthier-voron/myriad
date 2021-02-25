OBJ := obj/
LIB := lib/
BIN := bin/


check: $(BIN)test
	./$<

all: $(BIN)test


$(BIN)test: main.cxx $(wildcard include/myriad/net/*.hxx include/myriad/net/TcpThreadNetwork/*.hxx include/myriad/sched/*.hxx) | $(BIN)
	g++ -Wall -Wextra -O3 -std=c++20 -g -flto $< -o $@ -Iinclude/ -include include/myriad/cpp.hxx -include include/myriad/log.hxx -lmetasys -lpthread


$(BIN):
	mkdir $@


clean:
	-rm -rf $(OBJ) $(LIB) $(BIN)
