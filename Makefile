COMPILER = clang++

EXE = koji
OUT_DIR = out
BACKEND_DIRECTORY = dependencies/imgui

SOURCES = $(shell find src -name '*.cpp')
SOURCES += $(BACKEND_DIRECTORY)/imgui.cpp $(BACKEND_DIRECTORY)/imgui_draw.cpp $(BACKEND_DIRECTORY)/imgui_tables.cpp $(BACKEND_DIRECTORY)/imgui_widgets.cpp
SOURCES += $(BACKEND_DIRECTORY)/backends/imgui_impl_sdl3.cpp $(BACKEND_DIRECTORY)/backends/imgui_impl_sdlrenderer3.cpp
SOURCES += $(BACKEND_DIRECTORY)/misc/cpp/imgui_stdlib.cpp

OBJECTS = $(SOURCES:%.cpp=$(OUT_DIR)/%.o)
FLAGS = -std=c++20 -I$(BACKEND_DIRECTORY) -I$(BACKEND_DIRECTORY)/backends -g -Wall -Wformat $(shell pkg-config --cflags sdl3 mpv taglib) 
LIBS = -ldl $(shell pkg-config --libs sdl3 mpv taglib) 
FONT_OBJECT = $(OUT_DIR)/dependencies/assets/GoNotoCurrent_Regular.o

all: $(EXE)

$(EXE): $(OBJECTS) $(FONT_OBJECT)
	$(COMPILER) $^ -o $@ $(LIBS)

$(OUT_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(COMPILER) $(FLAGS) -MMD -MP -c $< -o $@


$(FONT_OBJECT): dependencies/assets/GoNotoCurrent_Regular.ttf
	@mkdir -p $(dir $@)
	ld -r -b binary -o $@ $<

-include $(OBJECTS:.o=.d)

clean:
	rm -rf $(EXE) $(OUT_DIR)

.PHONY: all clean
