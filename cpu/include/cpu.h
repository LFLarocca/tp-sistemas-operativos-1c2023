#ifndef CPU_H
#define CPU_H

#include <stdint.h>

#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>

#include "shared/include/configuraciones.h"
#include "shared/include/utilsCliente.h"
#include "shared/include/utilsServidor.h"
#include "cpu/include/conexionMemoria.h"
#include "cpu/include/servidorKernel.h"

int socketCliente;
t_log* logger;
t_log* loggerError; 
t_config* config;

#endif 