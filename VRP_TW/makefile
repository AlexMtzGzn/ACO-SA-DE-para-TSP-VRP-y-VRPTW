# Compilador
CC = gcc

# Directorios
INCLUDE_DIR = includes
SRC_DIR = src
BUILD_DIR = build

# Archivos fuente y objeto
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Ejecutable
TARGET = main

# Flags de compilación
CFLAGS = -Wall -I$(INCLUDE_DIR) -O2

# Regla principal
all: $(TARGET)

# Compilar el ejecutable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -lm

# Compilar archivos fuente a objetos
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Crear el directorio build si no existe
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Limpiar archivos compilados
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Limpiar todo (incluyendo ejecutable)
dist-clean: clean
	rm -f $(TARGET)
