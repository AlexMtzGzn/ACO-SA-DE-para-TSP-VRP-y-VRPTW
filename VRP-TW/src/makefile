CC = gcc
CFLAGS = -lm
TARGET = main
SRCS = main.c AED.c vrp_tw_setting.c control_memoria.c #entrada_salida_datos.c 

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) -o $(TARGET) $(SRCS) $(CFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
