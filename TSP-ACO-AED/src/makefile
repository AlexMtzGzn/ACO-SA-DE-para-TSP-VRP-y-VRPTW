# Variables
CC = gcc
CFLAGS = -lm
TARGET = main
SRCS = main.c AED.c TSP_ACO.c entrada_salida_datos.c

# Regla por defecto
all: $(TARGET)

# Regla para compilar el ejecutable
$(TARGET): $(SRCS)
	$(CC) -o $(TARGET) $(SRCS) $(CFLAGS)

# Limpiar los archivos generados
clean:
	rm -f $(TARGET)
