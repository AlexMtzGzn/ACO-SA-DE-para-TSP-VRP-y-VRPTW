#ifndef TSP_ANT_H
#define TSP_ANT_H
typedef struct
{
    int *ruta;
    int *tabu;
    double *probabilidades;
    double *fitness;
    
}hormiga;

void calcular_costo(hormiga *hor, double **instancia_distancias, int tamanio_instancia);
void actualizar_feromona(hormiga *hor, individuo *ind, double **instancia_distancias, double **instancia_feromona, int tamanio_instancia);
void ruta_hormiga(hormiga * hor,individuo * ind, double ** instancia_distancia, double ** instancia_feromona, double ** instancia_visibilidad, int tamanio_instancia);
void imprime_ruta_hormiga(hormiga * hor,int tamanio_instancia);
void ant_system(hormiga *hor, individuo *ind, double **instancia_distancias, double **instansia_feromona,double ** instancia_visibilidad, int tamanio_instancia);
void inicializar_visibilidad(double ** instancia_visibilidad,double ** instancia_distancias, int tamanio_instancia);
void inializacionHormiga(hormiga *hor, int tamanio_instancia, int numHormigas);
void aco_tsp(individuo * ind, double ** instancia_feromona,double ** instancia_distancias,int tamanio_instancia);

#endif // TSP_ANT_H