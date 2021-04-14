LDFLAGS += -pthread
OBJECTS := main.o
CXXFLAGS ?= -Wall -Wextra -O3
CXXFLAGS += -std=c++2a
CXX ?= g++
INCLUDES := -I ../include -I ../include/fossile_chess

BOARD_LIB := ../lib/board.a

HEADERS := ../include/board.hpp ../include/piece.hpp ../include/constants.hpp ../include/fossile_chess/fossile_chess.hpp
EXECUTABLE := main

$(EXECUTABLE): $(OBJECTS) $(HEADERS) $(BOARD_LIB)
	$(CXX) $(OBJECTS) $(LDFLAGS) $(BOARD_LIB) -o $@

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -c

.PHONY: clean $(BOARD_LIB)

$(BOARD_LIB):
	$(MAKE) -C ../lib release

clean:
	$(RM) $(EXECUTABLE) $(OBJECTS)
	$(MAKE) -C ../lib clean

perf_calls: $(EXECUTABLE)
	perf record $(PERF_ARGS) ./$(EXECUTABLE)
	perf report --no-children

perf_calls_lbr : PERF_ARGS += --call-graph=lbr
perf_calls_lbr: perf_calls

perf_stat: $(EXECUTABLE)
	perf stat -d ./$(EXECUTABLE)
