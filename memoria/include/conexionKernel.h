#ifndef MEMORIA_CONEXION_KERNEL_H
#define MEMORIA_CONEXION_KERNEL_H

#include<stdio.h>
#include<stdlib.h>
#include <stdint.h>
#include "memoria/include/auxiliaresMemoria.h"
#include  <commons/log.h>
#include<commons/config.h>
#include <commons/collections/list.h>
#include "shared/include/utilsCliente.h"
#include "shared/include/utilsServidor.h"
#include "manejoSegmentacion.h"
#include "compactacion.h"


extern t_config* config;
extern t_log* logger;
extern t_segmento* segmento0;
extern void* espacioDeUsuario;
extern t_list* huecosLibres; 
extern t_list* tablaDeTablasDeSegmentos; 
extern int sockets[3];

int ejecutarServidorKernel(int*);
t_list* crearTablaDeSegmentosInicial(uint32_t); 
void enviarTablaSegmentos(t_proceso* procesoEnMemoria);
void agregarSegmentoAPaquete(t_paquete* paquete, t_segmento* segmento);

t_proceso* crearProcesoEnMemoria(uint32_t pid); 
uint32_t recibirPID(int socketCliente); 

t_peticion* recibirPeticionCreacionDeSegmento(int); 
void recibirYProcesarPeticionEliminacionSegmento(int socketCliente); 

void procesarResultado(int resultado, int socketKernel); 

void eliminarProcesoDeMemoria(t_proceso* proceso); 
void deleteSegment(uint32_t pid, uint32_t segmentId);

void convertirSegmentoEnHuecoLibre(void* segmentoAEliminar);

void listarHuecosLibres ();

void liberarTodosLosSegmentos(uint32_t pid);

bool hayHuecoLibreArriba(void* huecoLibre);

bool hayHuecoLibreAbajo(void* huecoLibre);


#endif