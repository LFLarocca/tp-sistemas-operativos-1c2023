#ifndef PCB_H_
#define PCB_H_

#include <stdlib.h>
#include <string.h> 
#include <stdint.h> 
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/temporal.h>
#include "shared/include/global.h"
#include "kernel/include/configuraciones.h"


typedef enum estadoProceso{
    NEW, 
    READY,
    EXEC,
    BLOCK,
    SALIDA,
} estadoProceso; 

typedef struct {
    char AX[4], BX[4], CX[4], DX[4];
    char EAX[8], EBX[8], ECX[8], EDX[8];
    char RAX[16], RBX[16], RCX[16], RDX[16];
}t_reg;


typedef struct {

    uint32_t pid; 
    int socketPCB; 
    t_list* instrucciones; 
    uint32_t programCounter;   
    estadoProceso estado; 
    t_dictionary* registrosCPU;
    double estimadoProximaRafaga; 
    t_temporal* tiempoEnReady; 
    t_list* tablaDeSegmentos; 
    t_list* tablaDeArchivos; 
    t_list* recursosAsignados;

} t_pcb; 


extern t_list *pcbsNEW;
extern t_list *pcbsREADY;
extern t_list *pcbsEnMemoria;
extern t_log* logger;
extern int32_t procesosCreados;
extern char* pidsInvolucrados; 

t_pcb* crearPCB();

/**
 * @brief Libera la memoria asociada a un pcb
 * @param pcb puntero a una estructura de tipo pcb
 * @return
 */
void destruirPCB(t_pcb* pcb);

t_dictionary* crearDiccionarioDeRegistros(); 

// Lista de PCBs

/**
 * @brief Crea todas las listas de PCBs en los distintos estados
 * @param
 * @return
 */
void inicializarListasPCBs(); 

void destruirListasPCBs();

/**
 * @brief Libera la memoria de una lista de pcbs
 * @param pcbs lista de pcbs
 * @return
 */
void destruirListaPCB(t_list* pcbs);

/**
 * @brief se agrega un nuevo pcb al final de la lista de news 
 */

/**
 * @brief Selecciona el proximo pcb a ejecutar segun el algoritmo FIFO
 * @param pcbs lista de pcbs donde se encola el pcb
 * @param pcb el puntero a t_pcb a encolar en la lista de pcbs
 */
void encolar(t_list* pcbs,t_pcb* pcb);

/**
 * @brief Selecciona el proximo pcb a ejecutar segun el algoritmo FIFO
 * @param pcbs lista de pcbs de la cual se va a extraer el primer elemento
 * @return el proximo puntero a t_pcb a ejecutar
 */
t_pcb* desencolar(t_list* pcbs);

void agregarPID(void *value); 
void listarPIDS(t_list *pcbs); 
t_pcb* buscarPID(t_list* listaPCBs, uint32_t pid);



#endif