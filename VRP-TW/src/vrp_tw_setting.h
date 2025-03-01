#ifndef VRP_TW_SETTING_H
#define VRP_TW_SETTING_H


struct cliente
{
    int id;
    double cordenada_x;
    double cordenada_y;
    double demanda;
    double timpo_inicial;
    double tiempo_final;
    double servicio;
};


struct vrp_configuracion
{
    int num_clientes;
    int num_vehiculos;
    int num_capacidad;
    struct cliente *clientes; 
};


void abrimostxt_creamosxcvs(int *num_clientes, int *num_vehiculos, int *capacidad, char *ruta);
void leer_instancia(int *num_clientes, int *num_vehiculos, int *capacidad, char *archivo_instancia);

#endif // VRP_TW_SETTING_H
