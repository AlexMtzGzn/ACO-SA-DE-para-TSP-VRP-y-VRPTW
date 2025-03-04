#ifndef CONFIGURACION_VRP_TW_H
#define CONFIGURACION_VRP_TW_H

//Estructura Cliente

typedef struct cliente{
    int id_cliente;
    double cordenada_x;
    double cordenada_y;
    double demanda;
    double tiempo_inicial;  
    double tiempo_final;
    double servicio;
} cliente;

//Estructura Vrp_configuracion

typedef struct vrp_configuracion {
    int num_clientes;
    int num_vehiculos;
    int num_capacidad;
    cliente *clientes; 
} vrp_configuracion;

//Declaramos prototipos

void leemos_csv(vrp_configuracion *vrp, char *archivo_instancia);
void creamos_csv(vrp_configuracion *vrp, char *archivo_instancia);
void leemos_txt(vrp_configuracion *vrp, char *ruta);
vrp_configuracion *leer_instancia(char *archivo_instancia);

#endif // CONFIGURACION_VRP_TW_H
