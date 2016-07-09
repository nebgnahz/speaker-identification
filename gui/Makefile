EXE = gui
OBJS = main.o imgui_impl_glfw.o
OBJS += imgui.o imgui_demo.o imgui_draw.o ../wav-reader.o

UNAME_S := $(shell uname -s)

ECHO_MESSAGE = "Mac OS X"
LIBS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
LIBS += -L/usr/local/lib -lglfw3
LIBS += `pkg-config --cflags --libs sndfile`

CXXFLAGS = -I. -I../ -I/usr/local/include
CXXFLAGS += -Wall -Wformat
#	CXXFLAGS += -D__APPLE__
CFLAGS = $(CXXFLAGS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)

$(EXE): $(OBJS)
	$(CXX) -o $(EXE) $(OBJS) $(CXXFLAGS) $(LIBS)

clean:
	rm $(EXE) $(OBJS)
