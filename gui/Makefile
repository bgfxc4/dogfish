LDFLAGS += -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system -pthread
OBJECTS := main.o board-ui.o graphics.o parse_opts.o
CXXFLAGS ?= -Wall -Wextra
CXXFLAGS += -std=c++2a
CXX ?= g++
INCLUDES := -I ../include -I ../include/fossile_chess

BOARD_LIB := ../lib/board.a

HEADERS := board-ui.hpp graphics.hpp parse_opts.hpp ../include/board.hpp ../include/piece.hpp ../include/constants.hpp
EXECUTABLE := main

debug : CXXFLAGS += -Og -g -pg
release : CXXFLAGS += -O3 -fno-exceptions
debug : LDFLAGS +=
release : LDFLAGS += -O -s
debug : BUILD_TYPE := debug
release : BUILD_TYPE := release

debug: $(EXECUTABLE)
release: $(EXECUTABLE)

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
