CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -Iinclude \
        	$(shell pkg-config --cflags allegro-5 allegro_main-5 allegro_font-5 \
                                    	allegro_image-5 allegro_primitives-5 \
                                    	allegro_audio-5 allegro_acodec-5)
LIBS    = $(shell pkg-config --libs allegro-5 allegro_main-5 allegro_font-5 \
                                    allegro_image-5 allegro_primitives-5 \
                                    allegro_audio-5 allegro_acodec-5) -lm

TARGET  = bin/chaos_zoo
SRCS    = $(wildcard src/*.c)
OBJS    = $(patsubst src/%.c, build/%.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS) | bin
	$(CC) -o $@ $^ $(LIBS)

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

bin:
	mkdir -p bin

build:
	mkdir -p build

clean:
	rm -rf build bin

.PHONY: all clean