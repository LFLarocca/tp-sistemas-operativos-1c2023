#ifndef UTILS_SERVIDOR_H
#define UTILS_SERVIDOR_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <string.h>
#include <assert.h>
#include "configuraciones.h"

extern t_log *logger;
extern t_config *config;

int iniciarServidor(char * puerto);
int esperarCliente(int socketServidor);
int recibirOperacion(int socketCliente);
void * recibirBuffer(int socket, int  * size);
t_list * recibirPaquete(int socketCliente);
char * recibirMensaje(int socketCliente);
int alistarServidor(char * puerto);
void elementDestroyer(void * palabra);

#endif /* UTILS_SERVIDOR_H */ 