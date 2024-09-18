#include <stdlib.h>
#include "asignacion_memoria.h"

// Definición de la función
void* asignar_memoria(size_t tamano) {
    return malloc(tamano);
}
