CC = gcc
CFLAGS = -Wall -Wextra -std=c11 $(shell pkg-config --cflags allegro-5 allegro_main-5 allegro_font-5 allegro_image-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5)
LIBS = $(shell pkg-config --libs allegro-5 allegro_main-5 allegro_font-5 allegro_image-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5)

TARGET = caos_in_the_zoo
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIBS)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)