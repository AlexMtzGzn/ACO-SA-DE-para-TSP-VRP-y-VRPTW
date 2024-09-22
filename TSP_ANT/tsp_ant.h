#ifndef TSP_ANT_H
#define TSP_ANT_H
typedef struct
{
    double *ruta;
    double *tabu;
    double fitness;
}hormiga;

void ant_system(hormiga *hor, individuo *ind, double **instancia_distancias, double **instansia_feromona,double ** probabilidad, double ** visibilidad, int tamanio_instancia);
void inicializar_visibilidad(double ** instancia_visibilidad,double ** instancia_distancias, int tamanio_instancia);
void inializacionHormiga(hormiga *hor, int tamanio_instancia, int numHormigas);
double aco_tsp(individuo * ind, double ** instancia_feromona,double ** instancia_distancias,int tamanio_instancia);

#endif // TSP_ANT_H