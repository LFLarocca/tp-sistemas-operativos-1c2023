#ifndef ALGORITMO_SEGMENTACION_H
#define ALGORITMO_SEGMENTACION_H

#include<stdio.h>
#include<stdlib.h>

#include<commons/log.h>
#include<commons/config.h>
#include <pthread.h>
#include "auxiliaresMemoria.h"
#include "conexionKernel.h"

#include "shared/include/global.h"
#include "shared/include/utilsServidor.h"
#include "shared/include/utilsCliente.h"
#include "shared/include/configuraciones.h"
#include "memoria/include/manejoSegmentacion.h"

extern t_list* huecosLibres; 



t_proceso* buscarProcesoSegun(uint32_t pid); 

#endif 