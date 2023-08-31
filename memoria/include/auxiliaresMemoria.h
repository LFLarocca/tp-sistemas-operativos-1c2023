#ifndef AUXILIAR_MEMORIA_H
#define AUXILIAR_MEMORIA_H

#include <stdint.h>
#include <commons/collections/list.h>

typedef struct {
    uint32_t id;
    uint32_t direccionBase; 
    uint32_t tamanio; 
    uint32_t pid;
} t_segmento;

typedef struct {
    uint32_t direccionBase; 
    uint32_t tamanioHueco; 
} t_hueco_libre;

typedef struct {
    uint32_t pid; 
    t_list* tablaDeSegmentosAsociada;
} t_proceso; 

typedef struct {
    uint32_t pid; 
    t_segmento* segmento; 
} t_peticion; 


#endif 