#ifndef BLOQUES_H
#define BLOQUES_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/bitarray.h>

#define TAMANIO_PUNTERO 4
#define CANT_BLOQUES(bytes) ((bytes != 0) ? (bytes / tamanioBloques) + (bytes < (uint32_t) tamanioBloques) : 0) 
#define MAX_BLOQUES tamanioBloques / TAMANIO_PUNTERO + 1

extern int cantBloques;
extern int tamanioBitmap;
extern int tamanioBloques;
extern t_log * logger; 
extern t_log * loggerError; 
extern t_config * config; 
extern t_bitarray * bitmap;
extern char * ptrBloques;
extern char * ptrBitMap;
extern char ** bloques;

int copiarABloque (uint32_t numBloque, char * texto, uint32_t longTexto);
uint32_t proximoBloqueLibre ();
uint32_t direccionIndirectaAReal (uint32_t ptrIndirecto, uint32_t bloqueABuscar);
void eliminarBloque (uint32_t ptr);


#endif