TARGET_EXEC := demodulator

BUILD_DIR := build
SRC_DIRS := src

SRCS := $(shell find $(SRC_DIRS) -name '*.c')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_DIRS += fftw/include
INC_DIRS += liquid-dsp/include
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS := -Wall -Wextra -Wpedantic -Werror -Wno-error=deprecated-declarations $(INC_FLAGS) -MMD -MP
LDFLAGS := -Lfftw/lib -lfftw3f liquid-dsp/lib/libliquid.a.1.5 -lm

EXECUTABLE := $(BUILD_DIR)/$(TARGET_EXEC)

san: debug
san: CFLAGS += -fsanitize=address,undefined
san: LDFLAGS += -fsanitize=address,undefined

all: CFLAGS += -O3 -DNDEBUG
all: executable

debug: CFLAGS += -g3 -D_FORTIFY_SOURCE=2
debug: executable

executable: $(EXECUTABLE)

$(EXECUTABLE): fftw liquid-dsp $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

fftw:
	mkdir -p fftw/src && \
	curl -L https://www.fftw.org/fftw-3.3.10.tar.gz | \
	tar --strip-components=1 -xz -C fftw/src && \
	cd fftw/src && \
	PREFIX=$$(realpath ..) && \
	./configure --prefix $$PREFIX --enable-float && \
	make && \
	make install

liquid-dsp:
	mkdir liquid-dsp && \
	cd liquid-dsp && \
	git clone https://github.com/jgaeddert/liquid-dsp src && \
	cd src && \
	PREFIX=$$(realpath ..) && \
	./bootstrap.sh && \
	./configure --prefix $$PREFIX && \
	make && \
	make install

.PHONY: clean compdb valgrind

clean:
	@rm -rf $(BUILD_DIR)

compdb: clean
	@bear -- $(MAKE) san
	@mv compile_commands.json build

valgrind: debug
	@valgrind ./$(EXECUTABLE)

-include $(DEPS)
