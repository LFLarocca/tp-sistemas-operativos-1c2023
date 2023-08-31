#ifndef FILE_SYS_KERN_SER_H
#define FILE_SYS_KERN_SER_H

#include "shared/include/utilsCliente.h"
#include "shared/include/utilsServidor.h"
#include "fileSystem/include/operaciones.h"
#include "fileSystem/include/bloques.h"
#include "fileSystem/include/conexionMemoria.h"

void escucharAlKernel();
void ejecutarServidor();
fcb_t * recibirArchivo (void * data, int *desplazamiento);
int enviarArchivo (fcb_t * archivo, int socket);

extern int socketCliente;
extern int conexionAMemoria;
extern int tiempoDeEspera;

#endif
