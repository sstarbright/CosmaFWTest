# screw windows honestly, why does compiling for it have to be so terrible?
MAIN_SRC := $(wildcard src/*.c)
MAIN_OBJ := $(MAIN_SRC:src/%.c=build/obj/%.o)
CFW_SRC := $(wildcard CosmaFW/src/*.c)
CFW_OBJ := $(CFW_SRC:CosmaFW/src/%.c=build/obj/CosmaFW/%.o)
$(info $(MAIN_OBJ))
$(info $(CFW_OBJ))

LINUX_COMPILER = gcc
WINDOWS_COMPILER = x86_64-w64-mingw32-gcc
#-w suppresses warnings
COMPILER_FLAGS = -w
LINKER_FLAGS = -lSDL2

LINUX_OUT = build/linux/cosmafwtest

# builds the main executable
main.out: $(CFW_OBJ) $(MAIN_OBJ)
	mkdir -p build/linux
	$(LINUX_COMPILER) $(CFW_OBJ) $(MAIN_OBJ) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(LINUX_OUT)

# builds the CosmaFW objects
$(CFW_OBJ): $(CFW_SRC)
	mkdir -p build/obj/CosmaFW
	$(LINUX_COMPILER) -c $(CFW_SRC) $(COMPILER_FLAGS) -o $(CFW_OBJ)

# builds the main executable's objects
$(MAIN_OBJ): $(MAIN_SRC)
	mkdir -p build/obj
	$(LINUX_COMPILER) -c $(MAIN_SRC) $(COMPILER_FLAGS) -o $(MAIN_OBJ)

clean:
	rm -r build