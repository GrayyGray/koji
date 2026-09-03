COMPILER = clang++
EXE = koji
SOURCES = $(wildcard src/*.cpp)
BACKEND_DIRECTORY = deps/imgui
SOURCES += $(BACKEND_DIRECTORY)/imgui.cpp $(BACKEND_DIRECTORY)/imgui_demo.cpp $(BACKEND_DIRECTORY)/imgui_draw.cpp $(BACKEND_DIRECTORY)/imgui_tables.cpp $(BACKEND_DIRECTORY)/imgui_widgets.cpp
SOURCES += $(BACKEND_DIRECTORY)/backends/imgui_impl_sdl3.cpp $(BACKEND_DIRECTORY)/backends/imgui_impl_sdlrenderer3.cpp

OBJECTS = $(SOURCES:.cpp=.o)
FLAGS = -std=c++20 -I$(BACKEND_DIRECTORY) -I$(BACKEND_DIRECTORY)/backends -g -Wall -Wformat $(shell pkg-config --cflags sdl3 mpv taglib)
LIBS = -ldl $(shell pkg-config --libs sdl3 mpv taglib) 

all: $(EXE)

$(EXE): $(OBJECTS)
	$(COMPILER) $(OBJECTS) -o $(EXE) $(LIBS)

%.o: %.cpp
	$(COMPILER) $(FLAGS) -c -o $@ $<

clean:
	rm -f $(EXE) $(OBJECTS)

.PHONY: all clean
