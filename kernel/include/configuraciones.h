#ifndef CONFIGURACIONES_KERNEL_H
#define CONFIGURACIONES_KERNEL_H

#include <commons/config.h>
#include "shared/include/global.h"
#include "shared/include/configuraciones.h"

#define obtenerGradoMultiprogramacion() config_get_int_value   (config, "GRADO_MAX_MULTIPROGRAMACION")
#define obtenerEstimacionInicial()      config_get_double_value(config, "ESTIMACION_INICIAL")
#define obtenerAlgoritmoPlanificacion() config_get_string_value(config, "ALGORITMO_PLANIFICACION")
#define obtenerAlfaEstimacion()         config_get_double_value(config, "HRRN_ALFA")
#define obtenerRecursos()               config_get_array_value (config, "RECURSOS")
#define obtenerInstanciasRecursos()     config_get_array_value (config, "INSTANCIAS_RECURSOS")

extern t_config* config;

#endif