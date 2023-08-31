#ifndef MANEJO_SEGMENTACION_H
#define MANEJO_SEGMENTACION_H

#include<stdio.h>
#include<stdlib.h>
#include <stdint.h>

#include<commons/log.h>
#include<commons/config.h>
#include <pthread.h>

#include "shared/include/global.h"
#include "memoria/include/auxiliaresMemoria.h"
#include "shared/include/utilsServidor.h"
#include "shared/include/utilsCliente.h"
#include "shared/include/configuraciones.h"
#include "memoria/include/algoritmosAsignacion.h"


extern t_segmento* segmento0; 
extern void* espacioDeUsuario; 
extern t_list* huecosLibres; 
void liberarEspacioDeUsuario ();
void liberarSegmentoCompartido ();

void creacionEspacio(); 
void creacionListaHuecosLibres(); 
void liberarListaDeHuecos ();

t_segmento* crearSegmentoCompartido(); 
op_code ubicarSegmentosEnEspaciosLibres(t_peticion* peticion); 
op_code ubicarSegmentosPorFirst(t_peticion* peticion); 
op_code ubicarSegmentosPorBest(t_peticion* peticion);
op_code ubicarSegmentosPorWorst(t_peticion* peticion); 


void loggearCreacionDeSegmento(t_peticion* peticion);
void asignacionAlEspacioDeMemoria(t_segmento* segmento);

void agregarSegmentoATablaDeSegmentosPCB(t_peticion* peticion); 
void reducirHuecosLibres(t_segmento* segmento, int indiceHueco); 

bool corroborarPosibilidadDeCompactacion(t_peticion* peticion); 

uint32_t sumaTamaniosHuecosLibres(); 





#endif 