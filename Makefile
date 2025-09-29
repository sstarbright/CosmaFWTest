SRC = src/main.c

LINUX_COMPILER = gcc
WINDOWS_COMPILER = x86_64-w64-mingw32-gcc
#-w suppresses warnings
COMPILER_FLAGS = -w
WINDOWS_COMPILER_FLAGS = $(pkg-config sdl2 --cflags)
LINKER_FLAGS = -lSDL2

LINUX_OUT = build/linux/cosmafwtest
WINDOWS_OUT = build/windows/cosmafwtest.exe
all : $(SRC)
	$(LINUX_COMPILER) $(SRC) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(LINUX_OUT) -DLINUX=1
	$(WINDOWS_COMPILER) $(SRC) $(COMPILER_FLAGS) $(WINDOWS_COMPILER_FLAGS) $(LINKER_FLAGS) -o $(WINDOWS_OUT) -DWINDOWS=1 -municode