CXXFLAGS ?= -Wall -Wextra -std=c++17
LDFLAGS ?=
CXX ?= g++
INCLUDES := -I ../include

OBJECTS := board.o board-content.o piece.o
HEADERS := ../include/board.hpp ../include/piece.hpp
LIBRARY := board.a

debug : CXXFLAGS += -Og -g -pg
release : CXXFLAGS += -O3 -fno-exceptions
debug : LDFLAGS +=
release : LDFLAGS += -O -s

debug: $(LIBRARY)
release: $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(RM) $@
	$(AR) rcs $@ $(OBJECTS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -c

.PHONY: clean
clean:
	$(RM) $(LIBRARY) $(OBJECTS)
