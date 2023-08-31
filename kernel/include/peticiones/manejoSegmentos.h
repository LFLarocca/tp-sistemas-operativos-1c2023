#ifndef MANEJO_SEGMENTOS_H_
#define MANEJO_SEGMENTOS_H_

#include <commons/collections/list.h>
#include "kernel/include/conexiones/conexionMemoria.h"
#include "kernel/include/peticiones/pcb.h"
#include "shared/include/contextoEjecucion.h"

extern int conexionAMemoria;

void recibirTablaDeSegmentosActualizada(t_pcb* pcb);
uint32_t recibirPID(int socketCliente) ;

#endif /* MANEJO_SEGMENTOS_H_ */