#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE 1024  // Tamaño máximo de línea

int main() {
    FILE *file = fopen("/home/amg/Documentos/Proyecto-Terminal-Green-VRP/VRP-TW/Instances/C101.csv", "r");
    if (!file) {
        perror("Error al abrir el archivo");
        return EXIT_FAILURE;
    }

    char line[MAX_LINE]; // Buffer para leer cada línea
    int numVehiculos, capacidad;

    // Omitir la primera línea (nombre del dataset)
    fgets(line, MAX_LINE, file);

    // Leer la segunda línea con número de vehículos y capacidad
    if (fgets(line, MAX_LINE, file)) {
        sscanf(line, "%d,%d", &numVehiculos, &capacidad);
        printf("Número de vehículos: %d\nCapacidad: %d\n", numVehiculos, capacidad);
    }

    // Leer las demás líneas con la información de clientes
    printf("\nID\tX\tY\tDemanda\tInicio\tFin\tServicio\n");
    while (fgets(line, MAX_LINE, file)) {
        int id, x, y, demanda, inicio, fin, servicio;
        if (sscanf(line, "%d,%d,%d,%d,%d,%d,%d", &id, &x, &y, &demanda, &inicio, &fin, &servicio) == 7) {
            printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\n", id, x, y, demanda, inicio, fin, servicio);
        }
    }

    fclose(file);
    return EXIT_SUCCESS;
}
