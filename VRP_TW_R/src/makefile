CC = gcc
CFLAGS = -lm
TARGET = main
SRCS = main.c aed.c configuracion_vrp_tw.c control_memoria.c 

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) -o $(TARGET) $(SRCS) $(CFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
