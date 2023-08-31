#ifndef MEMORIA_CONEXION_FS_H
#define MEMORIA_CONEXION_FS_H

#include <commons/config.h>
#include <commons/string.h>
#include "shared/include/utilsCliente.h"
#include "shared/include/utilsServidor.h"
#include "shared/include/global.h"

int ejecutarServidorFS(int*);

extern t_config* config;
extern t_log* logger; 
extern void* espacioDeUsuario; 

char * recibirPeticionDeLecturaFS(int socketFS);
char* leer_fs(int32_t direccionFisica, int tamanio);
void recibirPeticionDeEscrituraFS(int socketFS);
void escribir_fs(char* valor, int32_t direccionFisica, int tamanio);


extern int tiempo; 

#endif 