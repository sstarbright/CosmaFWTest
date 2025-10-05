# screw windows honestly, why does compiling for it have to be so terrible?
MAIN_SRC := $(wildcard src/*.c)
MAIN_OBJ := $(MAIN_SRC:src/%.c=build/linux/obj/%.o)
CFW_SRC := $(wildcard CosmaFW/src/*.c)
CFW_OBJ := $(CFW_SRC:CosmaFW/src/%.c=build/linux/obj/CosmaFW/%.o)

LINUX_COMPILER = gcc
WINDOWS_COMPILER = x86_64-w64-mingw32-gcc
#-w suppresses warnings
COMPILER_FLAGS = -w
LINKER_FLAGS = -lSDL2

LINUX_OUT = build/linux/cosmafwtest

# builds the main executable
main.out: libcfw.a $(MAIN_OBJ)
	@mkdir -p build/linux
	@$(LINUX_COMPILER) $(CFW_OBJ) $(MAIN_OBJ) $(COMPILER_FLAGS) -Lbuild/linux/static $(LINKER_FLAGS) -lcfw -o $(LINUX_OUT)

# builds the main executable's objects
$(MAIN_OBJ): $(MAIN_SRC)
	@mkdir -p build/linux/obj
	@$(LINUX_COMPILER) -c $(MAIN_SRC) $(COMPILER_FLAGS) -o $(MAIN_OBJ)

# builds the CosmaFW objects
$(CFW_OBJ): $(CFW_SRC)
	@mkdir -p build/linux/obj/CosmaFW
	@$(LINUX_COMPILER) -c $(CFW_SRC) $(COMPILER_FLAGS) -o $(CFW_OBJ)

#builds the CosmaFW static library
libcfw.a: $(CFW_OBJ)
	@mkdir -p build/linux/static
	@ar rcs build/linux/static/libcfw.a $(CFW_OBJ)

clean:
	@rm -r build