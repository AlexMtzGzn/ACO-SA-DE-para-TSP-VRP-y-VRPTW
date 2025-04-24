# Compilador
CC = gcc

# Directorios
INCLUDE_DIR = includes
SRC_DIR = src
BUILD_DIR = build

# Archivos fuente y objeto
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
DEPS = $(OBJS:.o=.d)

# Ejecutable
TARGET = main

# Flags de compilación
CFLAGS = -Wall -I$(INCLUDE_DIR)
DEBUG_FLAGS = -g -DDEBUG
DEPFLAGS = -MMD -MP

# Regla principal
all: release

# Compilar en modo release
release: CFLAGS += -O2
release: $(TARGET)

# Compilar en modo debug
debug: CFLAGS += $(DEBUG_FLAGS)
debug: clean $(TARGET)

# Compilar el ejecutable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -lm -lcjson

# Compilar archivos fuente a objetos
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

# Incluir dependencias
-include $(DEPS)

# Crear el directorio build si no existe
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Limpiar archivos compilados
clean:
	$(RM) -rf $(BUILD_DIR) $(TARGET)

# Limpiar todo (incluyendo ejecutable)
dist-clean: clean

.PHONY: all release debug clean dist-clean
