LDFLAGS ?= -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system
OBJECTS := main.o
CXXFLAGS ?= -Wall -Wextra
CXX ?= g++

HEADERS :=
EXECUTABLE := main

debug : CXXFLAGS += -Og -g -pg
release : CXXFLAGS += -O3
debug : LDFLAGS +=
release : LDFLAGS += -O -s

debug: $(EXECUTABLE)
release: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(HEADERS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $< -c -o $@

.PHONY: clean
clean:
	$(RM) $(EXECUTABLE) $(OBJECTS)
