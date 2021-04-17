CXXFLAGS ?= -Wall -Wextra -std=c++17
LDFLAGS ?=
CXX ?= g++
INCLUDES := -I ../include -I ../include/dogfish

OBJECTS := board.o piece.o dogfish/dogfish.o
HEADERS := ../include/board.hpp ../include/piece.hpp ../include/dogfish/dogfish.hpp ../include/dogfish/atomic_hashmap.hpp ../include/constants.hpp
LIBRARY := board.a

debug : CXXFLAGS += -Og -g -pg
release : CXXFLAGS += -O3 -fno-exceptions -DNDEBUG
benchmark : CXXFLAGS += -O3 -fno-exceptions
debug : LDFLAGS +=
release : LDFLAGS += -O -s
benchmark : LDFLAGS += -O

debug: $(LIBRARY)
release: $(LIBRARY)
benchmark: $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(RM) $@
	$(AR) rcs $@ $(OBJECTS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -c -o $@

.PHONY: clean
clean:
	$(RM) $(LIBRARY) $(OBJECTS)
