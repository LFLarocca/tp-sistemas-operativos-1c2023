#ifndef MEMORIA_CONEXION_CPU_H
#define MEMORIA_CONEXION_CPU_H

#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include "shared/include/utilsServidor.h"
#include "shared/include/utilsCliente.h"
#include "shared/include/global.h"
#include "manejoSegmentacion.h"


int ejecutarServidorCPU(int*); 

extern void* espacioDeUsuario; 


extern t_config* config;
extern t_log* logger; 
extern t_log* loggerError; 


char* leer(int32_t direccionFisica, int tamanio); 

void recibirPeticionDeLectura(int socketCPU);

void recibirPeticionDeEscritura(int socketCPU); 

void enviarValorObtenido(int socketCPU); 

void escribir(char* valor, int32_t direccionFisica, int tamanio);

#endif 



