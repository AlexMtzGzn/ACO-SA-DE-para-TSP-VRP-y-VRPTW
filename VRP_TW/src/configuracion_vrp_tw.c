#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/control_memoria.h"
#include "../includes/configuracion_vrp_tw.h"
#include "../includes/estructuras.h"

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
            vrp->clientes[cliente_index].coordenada_x = x;
            vrp->clientes[cliente_index].coordenada_y = y;
            vrp->clientes[cliente_index].demanda_capacidad = demanda;
            vrp->clientes[cliente_index].vt_inicial = inicio;
            vrp->clientes[cliente_index].vt_final = fin;
            vrp->clientes[cliente_index].tiempo_servicio = servicio;

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
        fprintf(archivo, "%d, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf\n",
                vrp->clientes[i].id_cliente,
                vrp->clientes[i].coordenada_x,
                vrp->clientes[i].coordenada_y,
                vrp->clientes[i].demanda_capacidad,
                vrp->clientes[i].vt_inicial,
                vrp->clientes[i].vt_final,
                vrp->clientes[i].tiempo_servicio);
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
            vrp->clientes[cliente_index].coordenada_x = x;
            vrp->clientes[cliente_index].coordenada_y = y;
            vrp->clientes[cliente_index].demanda_capacidad = demanda;
            vrp->clientes[cliente_index].vt_inicial = inicio;
            vrp->clientes[cliente_index].vt_final = fin;
            vrp->clientes[cliente_index].tiempo_servicio = servicio;

            cliente_index++;
        }
    }

    fclose(file);
}

struct vrp_configuracion *leer_instancia(char *archivo_instancia)
{
    char ruta[100];
    struct vrp_configuracion *vrp = asignar_memoria_vrp_configuracion(); // Asiganamos memoria para la estructura vrp_configuracion

    if (vrp == NULL)
    {
        printf("Error al asignar memoria para la configuración VRP.\n");
        return NULL;
    }

    vrp->num_vehiculos = 0; // Inicializamos numero de vehiculos en 0
    vrp->num_capacidad = 0; // Inicializamos la capacidad del vehiculo en 0
    vrp->num_clientes = 0;  // Inicializamos numero de clientes en 0
    vrp->clientes = NULL;   // Inicializamos la estructura vrp_clientes en NULL

    snprintf(ruta, sizeof(ruta), "../Instancias/%s.csv", archivo_instancia); // Concatenamos el archivo_instancia con la ruta de la carpeta y la extension de formato
    FILE *archivo = fopen(ruta, "r");

    if (archivo) // Verificamos si el archivo existe con el formato csv
    {
        leemos_csv(vrp, archivo_instancia); // Vamos a
        return vrp;
    }
    {
        snprintf(ruta, sizeof(ruta), "../VRP_Solomon/%s.txt", archivo_instancia); // Cambiomos la concatenacion de la instancia

        leemos_txt(vrp, ruta);
        creamos_csv(vrp, archivo_instancia);

        return vrp;
    }
}
