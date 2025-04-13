import json
import os
import sys
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
from matplotlib.patches import Circle

class TSPSimulator:
    def __init__(self, archivo_json, numero_clientes):
        # Cargar los datos desde el archivo JSON
        with open(archivo_json, 'r') as file:
            self.data = json.load(file)
        
        
        self.filename = os.path.basename(archivo_json).split('.')[0]  
        self.directory = os.path.basename(os.path.dirname(archivo_json))  
        
        # Guardar el número de clientes
        self.numero_clientes = numero_clientes
        
        # Color para la ruta
        self.color = 'blue'
    
    def plot_static_route(self):
        """Dibuja la ruta de forma estática"""
        fig, ax = plt.subplots(figsize=(12, 10))
        
        # Extraer coordenadas de la ruta
        ruta = self.data['Ruta Coordenadas']
        x = [punto['X'] for punto in ruta]
        y = [punto['Y'] for punto in ruta]
        
        # Dibujar ruta
        ax.plot(x, y, '-o', color=self.color, markersize=5, label="Ruta TSP")
        
        # Dibujar el punto de inicio/fin con un símbolo especial
        ax.plot(ruta[0]['X'], ruta[0]['Y'], 'ks', markersize=8)
        
        # Dibujar todos los puntos de la ruta
        ax.scatter(x[1:-1], y[1:-1], c='black', s=20, alpha=0.5)
        
        # Añadir etiqueta al punto de inicio/fin
        inicio_x, inicio_y = ruta[0]['X'], ruta[0]['Y']
        ax.annotate('Inicio/Fin', (inicio_x, inicio_y), 
                    xytext=(5, 5), textcoords='offset points', fontsize=12)
        
        # Añadir etiquetas a los puntos
        for i, (punto_x, punto_y) in enumerate(zip(x[1:-1], y[1:-1])):
            ax.annotate(str(self.data['Ruta Clientes'][i+1]), (punto_x, punto_y), 
                       xytext=(3, 3), textcoords='offset points', fontsize=8)
        
        # Configurar el gráfico con el nombre del archivo en el título
        ax.set_xlabel('Coordenada X')
        ax.set_ylabel('Coordenada Y')
        ax.set_title(f'Ruta TSP - {self.filename} ({self.numero_clientes} clientes)')
        ax.legend(loc='upper right')
        ax.grid(True)
        
        plt.tight_layout()
        
        # Asegurar que el directorio existe
        os.makedirs(f'Resultados/Resultados_{self.numero_clientes}/Imagenes/{self.directory}', exist_ok=True)
        plt.savefig(f'Resultados/Resultados_{self.numero_clientes}/Imagenes/{self.directory}/{self.filename}.png')        
    
    def animate_route(self):
        """Anima la simulación de la ruta"""
        fig, ax = plt.subplots(figsize=(12, 10))
        
        # Extraer coordenadas de la ruta
        ruta = self.data['Ruta Coordenadas']
        x = [punto['X'] for punto in ruta]
        y = [punto['Y'] for punto in ruta]
        
        # Crear línea para la ruta
        line, = ax.plot([], [], '-', color=self.color, alpha=0.6, label="Ruta TSP")
        
        # Crear marcador para el viajero
        traveler = Circle((x[0], y[0]), 1.5, color=self.color, zorder=3)
        ax.add_patch(traveler)
        
        # Dibujar el punto de inicio/fin
        inicio_x, inicio_y = ruta[0]['X'], ruta[0]['Y']
        ax.plot(inicio_x, inicio_y, 'ks', markersize=8)
        ax.annotate('Inicio/Fin', (inicio_x, inicio_y), 
                    xytext=(5, 5), textcoords='offset points', fontsize=12)
        
        # Dibujar todos los puntos de la ruta
        ax.scatter(x[1:-1], y[1:-1], c='black', s=20, alpha=0.5)
        
        # Añadir etiquetas a los puntos
        for i, (punto_x, punto_y) in enumerate(zip(x[1:-1], y[1:-1])):
            cliente_id = self.data['Ruta Clientes'][i+1] if 'Ruta Clientes' in self.data else i+1
            ax.annotate(str(cliente_id), (punto_x, punto_y), 
                       xytext=(3, 3), textcoords='offset points', fontsize=8)
        
        # Configuración del gráfico con el nombre del archivo en el título
        ax.set_xlabel('Coordenada X')
        ax.set_ylabel('Coordenada Y')
        ax.set_title(f'Simulación de Ruta TSP - {self.filename} ({self.numero_clientes} clientes)')
        
        # Determinar los límites del gráfico
        x_min = min(x) - 5
        x_max = max(x) + 5
        y_min = min(y) - 5
        y_max = max(y) + 5
        
        ax.set_xlim(x_min, x_max)
        ax.set_ylim(y_min, y_max)
        ax.grid(True)
        ax.legend(loc='upper right')
        
        # Información de la simulación
        info_text = ax.text(0.02, 0.98, '', transform=ax.transAxes, fontsize=9,
                          verticalalignment='top', bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))
        
        # Total de pasos en la animación
        max_steps = len(x)
        
        # Función para inicializar la animación
        def init():
            line.set_data([], [])
            return [line, traveler, info_text]
        
        # Función para actualizar la animación en cada paso
        def update(frame):
            # Actualizar la línea
            line.set_data(x[:frame+1], y[:frame+1])
            
            # Actualizar la posición del viajero
            traveler.center = (x[frame], y[frame])
            
            # Añadir información del progreso
            if frame < len(x) - 1:
                # Obtener información del punto actual y siguiente
                punto_actual = self.data['Ruta Clientes'][frame] if 'Ruta Clientes' in self.data else frame
                punto_siguiente = self.data['Ruta Clientes'][frame+1] if 'Ruta Clientes' in self.data else frame+1
                
                # Calcular distancia recorrida hasta ahora
                distancia = 0
                for i in range(frame):
                    x1, y1 = x[i], y[i]
                    x2, y2 = x[i+1], y[i+1]
                    distancia += np.sqrt((x2-x1)**2 + (y2-y1)**2)
                
                frame_info = f"Paso: {frame+1}/{max_steps-1}\n"
                frame_info += f"Visitando: Punto {punto_actual} → {punto_siguiente}\n"
                frame_info += f"Distancia recorrida: {distancia:.2f}\n"
                
                if 'Fitness Global' in self.data:
                    frame_info += f"Distancia total óptima: {self.data['Fitness Global']:.2f}"
            else:
                # En el último paso
                distancia_total = 0
                for i in range(len(x)-1):
                    x1, y1 = x[i], y[i]
                    x2, y2 = x[i+1], y[i+1]
                    distancia_total += np.sqrt((x2-x1)**2 + (y2-y1)**2)
                
                frame_info = f"¡Ruta completada!\n"
                frame_info += f"Distancia total recorrida: {distancia_total:.2f}\n"
                
                if 'Fitness Global' in self.data:
                    frame_info += f"Distancia óptima: {self.data['Fitness Global']:.2f}"
            
            info_text.set_text(frame_info)
            return [line, traveler, info_text]
        
        # Crear la animación
        ani = animation.FuncAnimation(fig, update, frames=max_steps,
                                     init_func=init, blit=True, interval=200)
        
        plt.tight_layout()
        
        # Asegurar que el directorio existe
        os.makedirs(f'Resultados/Resultados_{self.numero_clientes}/Gifs/{self.directory}', exist_ok=True)
        
        # Guardar la animación como GIF con el nombre del archivo
        ani.save(f'Resultados/Resultados_{self.numero_clientes}/Gifs/{self.directory}/{self.filename}.gif', writer='pillow', fps=5)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Uso: python simulador_tsp.py <archivo_json> [numero_clientes]")
        sys.exit(1)
    
    # Obtener el número de clientes del segundo parámetro (si existe)
    numero_clientes = sys.argv[2] if len(sys.argv) > 2 else "unknown"
    
    # Construir la ruta del archivo correctamente
    archivo_json = sys.argv[1]
    
    # Si el usuario proporciona la ruta completa, usarla directamente
    if os.path.exists(archivo_json):
        ruta_archivo = archivo_json
    else:
        # Si proporciona solo el nombre o una ruta relativa
        if '/' in archivo_json:
            # Si es algo como "C100_(25)/C100_(25)_0.json"
            ruta_archivo = f"Resultados/Resultados_{numero_clientes}/Json/{archivo_json}"
        else:
            # Si es solo un nombre de archivo
            directorio = archivo_json.split('_')[0] + "_(" + str(numero_clientes) + ")"
            ruta_archivo = f"Resultados/Resultados_{numero_clientes}/Json/{directorio}/{archivo_json}"
    
    # Uso del simulador
    simulator = TSPSimulator(ruta_archivo, numero_clientes)
    
    # Dibujar la ruta estática
    simulator.plot_static_route()
    
    # Animar la ruta
    simulator.animate_route()