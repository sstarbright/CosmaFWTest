# screw windows honestly, why does compiling for it have to be so terrible?
MAIN_SRC := $(wildcard src/*.c)
MAIN_LINUX_OBJ := $(MAIN_SRC:src/%.c=build/linux/obj/%.o)
MAIN_WINDOWS_OBJ := $(MAIN_SRC:src/%.c=build/windows/obj/%.o)

CFW_SRC := $(wildcard CosmaFW/src/*.c)
CFW_LINUX_OBJ := $(CFW_SRC:CosmaFW/src/%.c=build/linux/obj/CosmaFW/%.o)
CFW_WINDOWS_OBJ := $(CFW_SRC:CosmaFW/src/%.c=build/windows/obj/CosmaFW/%.o)

LINUX_COMPILER = gcc
WINDOWS_COMPILER = x86_64-w64-mingw32-gcc
#-w suppresses warnings
COMPILER_FLAGS = -w
LINKER_FLAGS = -lm -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf

OUT_NAME = terracon
LINUX_OUT = build/linux/$(OUT_NAME)
WINDOWS_OUT = build/windows/$(OUT_NAME).exe
RUN_OUT = $(LINUX_OUT)

main_out: main_linux_out main_windows_out

# builds the main executable
main_linux_out: libcfw.a $(MAIN_LINUX_OBJ)
	@mkdir -p build/linux
	@cp -R assets build/linux/assets
	@$(LINUX_COMPILER) $(CFW_LINUX_OBJ) $(MAIN_LINUX_OBJ) $(COMPILER_FLAGS) -Lbuild/linux/static $(LINKER_FLAGS) -lcfw -o $(LINUX_OUT)

main_windows_out: libcfw.lib $(MAIN_WINDOWS_OBJ)
	@mkdir -p build/windows
	@cp -R assets build/windows/assets
	@$(WINDOWS_COMPILER) $(CFW_WINDOWS_OBJ) $(MAIN_WINDOWS_OBJ) $(COMPILER_FLAGS) -Lbuild/windows/static $(LINKER_FLAGS) -lcfw -o $(WINDOWS_OUT)
	@cp /usr/x86_64-w64-mingw32/bin/SDL2.dll build/windows/SDL2.dll
	@cp /usr/x86_64-w64-mingw32/bin/SDL2_image.dll build/windows/SDL2_image.dll
	@cp /usr/x86_64-w64-mingw32/bin/SDL2_mixer.dll build/windows/SDL2_mixer.dll
	@cp /usr/x86_64-w64-mingw32/bin/SDL2_ttf.dll build/windows/SDL2_ttf.dll


# builds the main executable's objects
build/linux/obj/%.o: src/%.c
	@mkdir -p build/linux/obj
	@$(LINUX_COMPILER) -c $< $(COMPILER_FLAGS) -o $@

build/windows/obj/%.o: src/%.c
	@mkdir -p build/windows/obj
	@$(WINDOWS_COMPILER) -c $< $(COMPILER_FLAGS) -o $@


# builds the CosmaFW objects
build/linux/obj/CosmaFW/%.o: CosmaFW/src/%.c
	@mkdir -p build/linux/obj/CosmaFW
	@$(LINUX_COMPILER) -c $< $(COMPILER_FLAGS) -o $@

build/windows/obj/CosmaFW/%.o: CosmaFW/src/%.c
	@mkdir -p build/windows/obj/CosmaFW
	@$(WINDOWS_COMPILER) -c $< $(COMPILER_FLAGS) -o $@


#builds the CosmaFW static library
libcfw.a: $(CFW_LINUX_OBJ)
	@mkdir -p build/linux/static
	@ar rcs build/linux/static/libcfw.a $(CFW_LINUX_OBJ)

libcfw.lib: $(CFW_WINDOWS_OBJ)
	@mkdir -p build/windows/static
	@ar rcs build/windows/static/libcfw.lib $(CFW_WINDOWS_OBJ)


run: main_out
	@$(LINUX_OUT)

clean:
	@rm -rf build