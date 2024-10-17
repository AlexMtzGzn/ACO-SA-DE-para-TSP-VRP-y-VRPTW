# Variables
CC = gcc
CFLAGS = -lm
TARGET = main
SRCS = main.c AED.c tsp_ant_system.c

# Regla por defecto
all: $(TARGET)

# Regla para compilar el ejecutable
$(TARGET): $(SRCS)
	$(CC) -o $(TARGET) $(SRCS) $(CFLAGS)

# Limpiar los archivos generados
clean:
	rm -f $(TARGET)
