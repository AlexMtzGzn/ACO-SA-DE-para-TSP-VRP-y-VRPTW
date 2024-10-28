import matplotlib.pyplot as plt
import numpy as np

def plot_evolution_from_file(direccion):
    with open(direccion, 'r') as file:
        lineas = file.readlines()
    
    
    primera_linea = lineas[0].strip().split()
    tiempo = float(primera_linea[0])
    poblacion = int(primera_linea[1])
    generaciones = int(primera_linea[2])
    
    datos = []
    for linea in lineas[1:]: 
        if linea.strip():  
            try:
                ind, gen, fit = map(float, linea.strip().split(','))
                datos.append([ind, gen, fit])
            except ValueError:
                continue 
    

    datos = np.array(datos)
    

    plt.figure(figsize=(12, 6))
    

    for individuo in range(1, poblacion + 1):
        mask = datos[:, 0] == individuo
        plt.plot(datos[mask, 1], datos[mask, 2], 
                marker='o', label=f'Individuo {individuo}')
    
    plt.title(f'Trayectoria del Fitness por Generación TSP-SA-AED\nTiempo: {tiempo:.6f} segundos')
    plt.xlabel('Generación')
    plt.ylabel('Fitness')
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.show()


direccion = 'TSP-SA-AED/src/poblacion_generacion_tsp_sa_aed.txt'
plot_evolution_from_file(direccion)