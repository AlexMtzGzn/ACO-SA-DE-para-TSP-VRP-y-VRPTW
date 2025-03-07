CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lm
TARGET = main
SRCS = main.c aed.c configuracion_vrp_tw.c control_memoria.c vrp_tw_aco.c lista_flota.c lista_ruta.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

# Regla para compilar el ejecutable final
$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

# Regla para compilar archivos objeto
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean