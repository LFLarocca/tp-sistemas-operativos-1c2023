#ifndef CONEXION_CPU_H
#define CONEXION_CPU_H

#include <commons/config.h>
#include <commons/string.h>
#include "shared/include/contextoEjecucion.h"
#include "shared/include/utilsCliente.h"
#include "shared/include/utilsServidor.h"
#include "shared/include/global.h"
#include "kernel/include/peticiones/pcb.h"

extern int conexionACPU;
extern t_log* loggerError; 
extern t_contexto* contextoEjecucion;


void conexionCPU();
t_contexto* procesarPCB(t_pcb* procesoEnEjecucion);


// FUNCIONES PARA ENVIO DE CONTEXTO DE EJECUCION

t_dictionary *registrosDelCPU(t_dictionary *aCopiar);
void asignarPCBAContexto(t_pcb*  proceso);

void actualizarPCB(t_pcb* proceso);

t_dictionary *registrosDelCPU(t_dictionary *aCopiar);


#endif /* CONEXION_CPU_H */