#ifndef PETICION_MEMORIA_H

 
#include<stdio.h>
#include<stdlib.h>
#include <stdint.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include "shared/include/utilsCliente.h"
#include "shared/include/utilsServidor.h"
#include "manejoSegmentacion.h"
#include "conexionKernel.h"

/*
typedef struct {
    uint32_t pid; 
    t_segmento* segmento; 
} t_peticion; 
*/

extern int sockets[3]; 
extern t_log* logger; 
extern t_log* loggerError; 
extern t_config* config; 

#endif