CC = gcc
CFLAGS = -lm
TARGET = main
SRCS = main.c AED.c TSP_ACO.c entrada_salida_datos.c control_memoria.c 

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) -o $(TARGET) $(SRCS) $(CFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
