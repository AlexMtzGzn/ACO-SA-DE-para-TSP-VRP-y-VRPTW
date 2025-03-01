#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vrp_tw_setting.h"

void abrimostxt_creamosxcvs(struct vrp_configuracion *vrp, char *ruta)
{
    FILE *file = fopen(ruta, "r");
    if (!file)
    {
        perror("Error al abrir el archivo");
        return;
    }

    char buffer[256];
    int linea_contador = 0;

    fgets(buffer, sizeof(buffer), file);

    while (fgets(buffer, sizeof(buffer), file))
    {
        if (strstr(buffer, "NUMBER") && strstr(buffer, "CAPACITY"))
        {
            if (fgets(buffer, sizeof(buffer), file))
            {
                char *ptr = buffer;
                while (*ptr == ' ' && *ptr != '\0')
                    ptr++;
                sscanf(ptr, "%d %d", &vrp->num_vehiculos, &vrp->num_capacidad);
                break;
            }
        }
    }

    while (fgets(buffer, sizeof(buffer), file))
    {
        if (strstr(buffer, "CUST NO."))
        {
            fgets(buffer, sizeof(buffer), file);
            break;
        }
    }

    int num_clientes = 0;
    long posicion_inicial = ftell(file);

    while (fgets(buffer, sizeof(buffer), file))
    {
        int id;
        if (sscanf(buffer, "%d", &id) == 1)
        {
            num_clientes++;
        }
    }

    vrp->num_clientes = num_clientes;

    vrp->clientes = (struct cliente *)malloc(vrp->num_clientes * sizeof(struct cliente));
    if (vrp->clientes == NULL)
    {
        printf("Error al asignar memoria para los clientes.\n");
        fclose(file);
        return;
    }
    fseek(file, posicion_inicial, SEEK_SET);

    int cliente_index = 0;

    while (fgets(buffer, sizeof(buffer), file) && cliente_index < vrp->num_clientes)
    {
        int id;
        double x, y, demanda, inicio, fin, servicio;

        if (sscanf(buffer, "%d %lf %lf %lf %lf %lf %lf",
                   &id, &x, &y, &demanda, &inicio, &fin, &servicio) == 7)
        {
            vrp->clientes[cliente_index].id = id;
            vrp->clientes[cliente_index].cordenada_x = x;
            vrp->clientes[cliente_index].cordenada_y = y;
            vrp->clientes[cliente_index].demanda = demanda;
            vrp->clientes[cliente_index].timpo_inicial = inicio;
            vrp->clientes[cliente_index].tiempo_final = fin;
            vrp->clientes[cliente_index].servicio = servicio;

            cliente_index++;
        }
    }

    fclose(file);
}

struct vrp_configuracion *leer_instancia(char *archivo_instancia)
{
    char ruta[100];
    struct vrp_configuracion *vrp = (struct vrp_configuracion *)malloc(sizeof(struct vrp_configuracion));

    if (vrp == NULL)
    {
        printf("Error al asignar memoria para la configuración VRP.\n");
        return NULL;
    }

    vrp->num_vehiculos = 0;
    vrp->num_capacidad = 0;
    vrp->num_clientes = 0;
    vrp->clientes = NULL;

    snprintf(ruta, sizeof(ruta), "../Instancias/%s.csv", archivo_instancia);
    FILE *archivo = fopen(ruta, "r");

    if (archivo)
    {
        printf("Archivo CSV abierto exitosamente.\n");
        fclose(archivo); // Cerrar archivo CSV si existe
        return vrp;
    }
    else
    {
        printf("No se encontró el archivo CSV, buscando el TXT...\n");
        snprintf(ruta, sizeof(ruta), "../VRP_Solomon/%s.txt", archivo_instancia);
        abrimostxt_creamosxcvs(vrp, ruta);
        return vrp;
    }
}
