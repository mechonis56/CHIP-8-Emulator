# Source files
SOURCES = CHIP8emu.c CHIP8emu.h font4x5.c font4x5.h machine/machine.c machine/machine.h display/display.c display/display.h main.c

# Output executable
EXE = emulator

# Compilation flags
CFLAGS = -Wall -g -ggdb

# Linking flags
LDFLAGS = -I sdl/include -L sdl/lib 

# Libraries
LIBS = -lmingw32 -lSDL2main -lSDL2

# Use gcc when linking
LD = gcc

# Create a list of object files from source files
OBJECTS = $(SOURCES: %.c = %.o)

# First target, default if none specified
# Tells "make" to make the "all" target
default: all

# Customary to have "make all"
all: $(EXE)

# Link executable from object files
$(EXE): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $(EXE) $(LIBS)

# Compile source files into object files
%.o: %.c

# Clean up after
clean:
	-rm -f $(EXE) 			# Remove executable file
	-rm -f $(OBJECTS)		# Remove object files

# Tell make what source and header files each object file depends on
CHIP8emu.o: CHIP8emu.c CHIP8emu.h
font4x5.o: font4x5.c font4x5.h
machine.o: machine.c machine.h
display.o: display.c display.h
main.o: main.c
