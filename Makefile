version = 0.0.1
arch ?= x86_64

LINKER = gcc
CC = gcc
ASM = gcc
NASM = nasm


programname = utpwrapper

#names
executable_fullname := $(programname)-$(version)-$(arch)
test: executable_fullname := $(programname)-$(version)-$(arch)-test
testmemgraph: executable_fullname := $(programname)-$(version)-$(arch)-test
debug: executable_fullname := $(programname)-$(version)-$(arch)-debug

lib: executable := pylibutp/$(executable_fullname).so
executable := bin/$(executable_fullname)

#include paths
dirs = $(shell find src/ -type d -print)
includedirs :=  $(sort $(foreach dir, $(foreach dir1, $(dirs), $(shell dirname $(dir1))), $(wildcard $(dir)/include)))

#linkerflags (include lm (math.h) for advanced math)
lib: LFLAGS = -L./lib -shared 
LFLAGS = -L./lib
LIBRARIES = -lm -lutp

test: LIBRARIES = -lm 
testmemgraph: LIBRARIES = -lm


#cflags
CFLAGS= -O2 -Wall $(foreach dir, $(includedirs), -I./$(dir))
debug: CFLAGS= -g -O0 -Wall $(foreach dir, $(includedirs), -I./$(dir))
test: CFLAGS= -g -O2 -Wall -DTEST -fprofile-arcs -ftest-coverage $(foreach dir, $(includedirs), -I./$(dir)) 
testmemgraph: CFLAGS=-g -O2 -Wall -DTEST $(foreach dir, $(includedirs), -I./$(dir)) 

#assembly
ASMFLAGS = $(foreach dir, $(includedirs), -I./$(dir))

NASMFLAGS =

#support for .S
assembly_source_files := $(foreach dir,$(dirs),$(wildcard $(dir)/*.S))
assembly_object_files := $(patsubst src/%.S, \
	build/%.o, $(assembly_source_files))

#support for .asm
nassembly_source_files := $(foreach dir,$(dirs),$(wildcard $(dir)/*.asm))
nassembly_object_files := $(patsubst src/%.asm, \
	build/%.o, $(nassembly_source_files))


c_source_files := $(foreach dir,$(dirs),$(wildcard $(dir)/*.c))
c_object_files := $(patsubst src/%.c, \
    build/%.o, $(c_source_files))
#qemu

.PHONY: all clean run install leaktest memgraph docs

all: $(executable)

install:
	@echo installing libutp
	@rm -rf libutp
	@git clone https://github.com/bittorrent/libutp
	@cd libutp && make
	@cp libutp/libutp.a lib
	@cp libutp/libutp.so lib
	@cp libutp/*.h src/include
	@rm -rf libutp
	
	

docs:
	@doxygen

memgraph: $(executable)
	valgrind --tool=massif --massif-out-file=massif.out.1 $(executable) 
	massif-visualizer massif.out.1

leaktest: $(executable)
	valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all $(executable) 

debug: $(executable)
	gdb --args $(executable) 

clean:
	@rm -r build
	@rm -r bin

run: $(executable)
	@echo starting
	@./$(executable)

lib: $(executable)


$(executable): $(assembly_object_files) $(c_object_files) $(nassembly_object_files)
	@echo linking...
	@mkdir -p bin
	@$(LINKER) -L/usr/lib $(LFLAGS) -o $(executable) $(assembly_object_files) $(nassembly_object_files) $(c_object_files) $(LIBRARIES)

# compile assembly files
build/%.o: src/%.S
	@mkdir -p $(shell dirname $@)
	@echo compiling $<
	@$(ASM) -c $(ASMFLAGS) $< -o $@

# compile assembly files
build/%.o: src/%.asm
	@mkdir -p $(shell dirname $@)
	@echo compiling $<
	@$(NASM) $(NASMFLAGS) $< -o $@

# compile c files
build/%.o: src/%.c
	@mkdir -p $(shell dirname $@)
	@echo compiling $<
	@$(CC) -c $(CFLAGS) $< -o $@