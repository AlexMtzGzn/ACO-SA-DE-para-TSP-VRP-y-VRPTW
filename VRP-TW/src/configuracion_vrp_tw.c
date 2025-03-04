#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "control_memoria.h"
#include "configuracion_vrp_tw.h"

void leemos_csv(struct vrp_configuracion *vrp, char *archivo_instancia)
{
    char ruta[100];
    snprintf(ruta, sizeof(ruta), "../Instancias/%s.csv", archivo_instancia);

    FILE *archivo = fopen(ruta, "r");
    if (archivo == NULL)
    {
        printf("Error al abrir el archivo CSV para lectura.\n");
        return;
    }

    char buffer[256];
    fgets(buffer, sizeof(buffer), archivo);

    int num_vehiculos, num_capacidad, num_clientes;
    if (fscanf(archivo, "%d, %d, %d\n", &num_vehiculos, &num_capacidad, &num_clientes) != 3)
    {
        printf("Error al leer los parámetros de configuración del archivo CSV.\n");
        fclose(archivo);
        return;
    }

    vrp->num_vehiculos = num_vehiculos;
    vrp->num_capacidad = num_capacidad;
    vrp->num_clientes = num_clientes;

    vrp->clientes = (struct cliente *)malloc(vrp->num_clientes * sizeof(struct cliente));
    if (vrp->clientes == NULL)
    {
        printf("Error al asignar memoria para los clientes.\n");
        fclose(archivo);
        return;
    }

    int cliente_index = 0;
    while (fgets(buffer, sizeof(buffer), archivo) && cliente_index < vrp->num_clientes)
    {
        int id;
        double x, y, demanda, inicio, fin, servicio;

        if (sscanf(buffer, "%d, %lf, %lf, %lf, %lf, %lf, %lf",
                   &id, &x, &y, &demanda, &inicio, &fin, &servicio) == 7)
        {
            vrp->clientes[cliente_index].id_cliente = id;
            vrp->clientes[cliente_index].cordenada_x = x;
            vrp->clientes[cliente_index].cordenada_y = y;
            vrp->clientes[cliente_index].demanda = demanda;
            vrp->clientes[cliente_index].tiempo_inicial = inicio;
            vrp->clientes[cliente_index].tiempo_final = fin;
            vrp->clientes[cliente_index].servicio = servicio;

            cliente_index++;
        }
    }

    fclose(archivo);
}

void creamos_csv(struct vrp_configuracion *vrp, char *archivo_instancia)
{
    char ruta[100];
    snprintf(ruta, sizeof(ruta), "../Instancias/%s.csv", archivo_instancia);

    FILE *archivo = fopen(ruta, "w");

    if (archivo == NULL)
    {
        printf("Error al abrir el archivo CSV para escritura.\n");
        return;
    }

    fprintf(archivo, "%s\n%d, %d, %d\n",
            archivo_instancia,
            vrp->num_vehiculos,
            vrp->num_capacidad,
            vrp->num_clientes);

    for (int i = 0; i < vrp->num_clientes; i++)
    {
        fprintf(archivo, "%d, %lf, %lf, %lf, %lf, %lf, %lf\n",
                vrp->clientes[i].id_cliente,
                vrp->clientes[i].cordenada_x,
                vrp->clientes[i].cordenada_y,
                vrp->clientes[i].demanda,
                vrp->clientes[i].tiempo_inicial,
                vrp->clientes[i].tiempo_final,
                vrp->clientes[i].servicio);
    }

    fclose(archivo);
}

void leemos_txt(struct vrp_configuracion *vrp, char *ruta)
{
    FILE *file = fopen(ruta, "r");
    if (!file)
    {
        perror("Error al abrir el archivo");
        return;
    }

    char buffer[256];
    int linea_contador = 0;

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

    vrp->clientes = asignar_memoria_clientes(vrp);
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
            vrp->clientes[cliente_index].id_cliente = id;
            vrp->clientes[cliente_index].cordenada_x = x;
            vrp->clientes[cliente_index].cordenada_y = y;
            vrp->clientes[cliente_index].demanda = demanda;
            vrp->clientes[cliente_index].tiempo_inicial = inicio;
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
    struct vrp_configuracion *vrp = asignar_memoria_vrp_configuracion();

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

        leemos_csv(vrp, archivo_instancia);
        return vrp;
    }
    else
    {
        snprintf(ruta, sizeof(ruta), "../VRP_Solomon/%s.txt", archivo_instancia);

        if (!ruta)
        {
            leemos_txt(vrp, ruta);
            creamos_csv(vrp, archivo_instancia);
        }

        return vrp;
    }
}
