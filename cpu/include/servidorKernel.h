#ifndef CPU_KERN_SER_H
#define CPU_KERN_SER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <commons/log.h>
#include <commons/config.h>

#include "shared/include/utilsServidor.h"
#include "shared/include/utilsCliente.h"
#include "shared/include/contextoEjecucion.h"
#include "cpu/include/cicloDeInstruccion.h"

extern t_log* loggerError; 

void escucharAlKernel(); 

extern int socketCliente;
extern t_contexto* contextoEjecucion;
extern t_temporal * rafagaCPU;

#endif