CC = gcc
CFLAGS = -lm
TARGET = main
SRCS = main.c aed.c configuracion_vrp_tw.c vrp-tw-aco.c control_memoria.c #entrada_salida_datos.c 

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) -o $(TARGET) $(SRCS) $(CFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
