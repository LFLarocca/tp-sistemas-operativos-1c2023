#ifndef ALGORITMOS_CORTO_PLAZO_H
#define ALGORITMOS_CORTO_PLAZO_H

#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/temporal.h>
#include <string.h>
#include "shared/include/global.h"
#include "kernel/include/configuraciones.h"
#include "kernel/include/peticiones/pcb.h"
#include "kernel/include/planificacion/planificacion.h"


extern t_list* pcbsNEW; 
extern t_list* pcbsREADY;
extern int64_t rafagaCPU;
extern t_log* loggerError;

/** 
 * @brief planificador a corto plazo segun algoritmo de planificacion extraido del archivo de configuraciones
*/
void planificarACortoPlazoSegunAlgoritmo();

/**
 * @brief Selecciona el proximo pcb a ejecutar segun el algoritmo FIFO
 * @param
 * @return el proximo puntero a t_pcb a ejecutar
 */
t_pcb* proximoAEjecutarFIFO();

/**
 * @brief indica el proximo elemento de la lista ready a ejecutar siguiendo el algoritmo HRRN
*/
 t_pcb* proximoAEjecutarHRRN();
void* mayorRR(void* unPCB, void* otroPCB); 
double calcularRR(void* pcb); 
void listarPIDS(t_list* pcbs); 
void detenerYDestruirCronometro(t_temporal* ); 
void calcularEstimadoProximaRafaga(t_pcb* , int64_t );
void estimacionNuevaRafaga(t_pcb*); 


#endif 