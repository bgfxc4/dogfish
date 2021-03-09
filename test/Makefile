CXX ?= g++
CXXFLAGS ?= -Wall -Wextra -std=c++17 -Og -g -pg

INCLUDES := -I ../include
BOARD_LIB := ../lib/board.a

HEADERS := ../include/board.hpp ../include/piece.hpp
OBJECTS := main.o
EXECUTABLE := main

$(EXECUTABLE): $(OBJECTS) $(HEADERS) $(BOARD_LIB)
	$(CXX) $(OBJECTS) $(LDFLAGS) $(BOARD_LIB) -o $@

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -c

.PHONY: clean $(BOARD_LIB)

$(BOARD_LIB):
	$(MAKE) -C ../lib $(BUILD_TYPE)

clean:
	$(RM) $(EXECUTABLE) $(OBJECTS)
	$(MAKE) -C ../lib clean
