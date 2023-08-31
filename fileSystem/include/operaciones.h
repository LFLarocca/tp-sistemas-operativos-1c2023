#ifndef FCB_H
#define FCB_H

#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/bitarray.h>

#include <fileSystem/include/bloques.h>

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
extern char * pathFCBs;

typedef struct {
    char * nombre;
    uint32_t tamanio;
    uint32_t ptrDirecto;
    uint32_t ptrIndirecto;
} fcb_t;

int crearArchivo (char * nombre);
fcb_t * abrirArchivo (char * nombre);
void cerrarArchivo (fcb_t * archivo);
int truncarArchivo (fcb_t * archivo, uint32_t tamanio);
char * leerArchivo (fcb_t * archivo, uint32_t puntero, uint32_t tamanio);
int escribirArchivo (fcb_t * archivo, char * data, uint32_t tamanio, uint32_t puntero);

int asignarBloqueAArchivo (fcb_t * archivo, uint32_t ptr);
uint32_t ultimoBloqueDeArchivo (fcb_t * archivo);
uint32_t espacioParaGuardarPuntero (fcb_t * archivo);
int eliminarPtr (fcb_t * archivo, uint32_t bloque);
int actualizarFCB (fcb_t * archivo);

#endif 