#ifndef MEMORIA_COMPACTACION_H
#define MEMORIA_COMPACTACION_H

#include <commons/collections/list.h>
#include <commons/config.h>
#include "auxiliaresMemoria.h"
#include "manejoSegmentacion.h"
#include "algoritmosAsignacion.h"
#include "peticiones.h"

extern void* espacioDeUsuario; 
extern t_list* huecosLibres; 
extern t_segmento* segmento0; 
extern t_list* tablaDeTablasDeSegmentos; 


extern t_config* config; 

void compactar(); 

void reubicacionDeSegmentos();

void unificarSegmentos(); 
void asignarProcesoalHueco();
void actualizarDirecciones();
void enviarTablasActualizadas(); 
void actualizarSegmento(uint32_t,uint32_t, t_segmento*); 

bool direccionBaseMenor (void * unSegmento, void * otroSegmento); 

bool ordenadoPorID (void * unSegmento, void * otroSegmento); 

bool direccionMasBaja (void * huecoLibreUno, void * huecoLibreDos); 

void crearUnicoHuecoLibre(uint32_t direccionFinal); 

#endif