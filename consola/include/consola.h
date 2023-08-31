#ifndef CLIENT_H_
#define CLIENT_H_

/** 
 * @defgroup Consola Consola
 * @addtogroup Consola
 * @{
 * 
 * @brief La consola tiene el trabajo de mandar al Kernel las instrucciones recibidas de los archivos.
 * 
 * @authors To The End
 * 
 * La consola se divide en tres etapas principales:
 * 
 * - Inicio: Se prepara el programa abriendo la configuracion, el logger y el archivo de pseudocodigo, y se abre conexion con el Kernel, en caso de fallar cualquiera de estos puntos termina el programa con codigo de error = 1.
 * - Proceso: Se lee el archivo de pseudocodigo y se envia, linea por linea (y eliminando el corte de linea), al Kernel, en caso de falla tambien termina el programa.
 * - Final: Cierra los archivos y termina el programa con codigo de error 0.
 * 
 * A su vez, el proceso se divide en 4 partes:
 * 
 * + Se crea la linea de codigo.
 * + Se lee la linea y se eliminan los cortes de linea.
 * + Se crea el paquete, se prepara, y se envia.
 * + Se libera la memoria.
 * 
 * @file consola.h
 * 
 */

#include <stdio.h>
#include <stdlib.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>

#include "shared/include/global.h"
#include "shared/include/utilsCliente.h"
#include "shared/include/utilsServidor.h"
#include "shared/include/configuraciones.h"

/**
 * @def MAX_CHARS
 * Cantidad maxima de lineas que se pueden leer en el codigo, de ser mas se expandira el limite.
 * 
 */
#define MAX_CHARS 2048

/**
 * @var socketKernel
 * Socket utilizado para conectarse y enviar informacion al Kernel.
 */
extern int socketKernel;
extern t_log *logger;
extern t_log *loggerError;
extern t_config *config;
extern FILE *codigo;

/**
 * @fn int   main  (int, char *archivos[])
 * @param archivos La consola debe recibir dos archivos necesariamente; 
 *                 el primero es el archivo de codigo que le envia al Kernel, 
 *                 y el segundo es el archivo de configuracion para conocer IP y Puerto del Kernel.
 * @return Se retorna un 0 en corrida correcta y un 1 en caso de un fallo.
 */
int main (int, char *archivos[]);

void esperarPID(char * archivo);

/**
 * @fn void esperarFinalizacion ()
 * Funcion de espera para recibir el mensaje de finalizacion de la Kernel.
 */
void esperarFinalizacion ();

void cerrarConexion ();
void cerrarArchivo ();

/**
 * Fin de la consola. 
 * 
 * @}
 * 
 */

#endif /* CLIENT_H_ */
