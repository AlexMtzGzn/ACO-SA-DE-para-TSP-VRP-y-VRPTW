#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CUSTOMERS 100  // Ajusta según necesidad

typedef struct {
    int id;
    int x, y;
    int demand;
    int ready_time, due_date;
    int service_time;
} Customer;

void read_file(const char *filename, Customer customers[], int *num_customers, int *vehicle_capacity) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    char line[100];
    *num_customers = 0;

    // Leer líneas hasta encontrar "CAPACITY"
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%*s %*s %d", vehicle_capacity) == 1) break;
    }

    // Leer líneas hasta encontrar "CUSTOMER"
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "CUSTOMER", 8) == 0) break;
    }

    // Saltar la línea de encabezado
    fgets(line, sizeof(line), file);

    // Leer los datos de los clientes
    while (fgets(line, sizeof(line), file)) {
        Customer c;
        if (sscanf(line, "%d %d %d %d %d %d %d",
                   &c.id, &c.x, &c.y, &c.demand, 
                   &c.ready_time, &c.due_date, &c.service_time) == 7) {
            customers[(*num_customers)++] = c;
        }
    }

    fclose(file);
}

void print_customers(Customer customers[], int num_customers, int vehicle_capacity) {
    printf("Capacidad del vehículo: %d\n", vehicle_capacity);
    printf("ID\tX\tY\tDemanda\tReady\tDue\tService\n");
    for (int i = 0; i < num_customers; i++) {
        printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
               customers[i].id, customers[i].x, customers[i].y,
               customers[i].demand, customers[i].ready_time,
               customers[i].due_date, customers[i].service_time);
    }
}

int main() {
    Customer customers[MAX_CUSTOMERS];
    int num_customers, vehicle_capacity;

    read_file("/home/amg/Documentos/Proyecto-Terminal-Green-VRP/VRP-TW/VRP_Solomon/C101.txt", customers, &num_customers, &vehicle_capacity);
    print_customers(customers, num_customers, vehicle_capacity);

    return 0;
}
