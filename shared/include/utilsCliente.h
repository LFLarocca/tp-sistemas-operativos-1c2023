#ifndef UTILS_CLIENTE_H
#define UTILS_CLIENTE_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include "configuraciones.h"

#define SEGS_ANTES_DE_REINTENTO 3

extern t_log *logger;
extern t_config *config;

/**
 * @struct t_buffer
 * @var t_buffer.size 
 * 	Tamaño del buffer creado.
 * @var t_buffer.stream
 * 	Stream con toda la informacion copiada.
 * @brief Información dentro de un paquete utilizado para enviar información a un servidor.
 *        Se divide en un int componiendo el tamaño del stream, y el stream en sí, que es la composición de información.
 */
typedef struct
{
	int size;
	void* stream;
} t_buffer;

/**
 * @enum op_code
 * Codigo de operacion utilizado para dar a conocer que se esta enviando. Esto puede ser:
 * 
 * - Un mensaje simple de texto.
 * - Un paquete con uno o mas elementos.
 * - Un contexto de ejecucion.
 * 
 * @brief Código de operación utilizado para enviar al servidor para informarle que tipo de paquete se utiliza.
 *        Actualmente los únicos dos códigos que se utilizan son MENSAJE, para realizar Handshakes, y PAQUETE, para enviar información.
 */
typedef enum
{	
	MENSAJE,
	PAQUETE, 
	CONTEXTOEJECUCION,
	TABLADESEGMENTOS,
	READ, 
	WRITE,
	NEWPCB, 
    ENDPCB,
	CREATE_SEGMENT_OP,
	DELETE_SEGMENT_OP,
	TERMINAR_KERNEL,
	SUCCESS,
	OUTOFMEMORY,
	COMPACTACION,
	FOPEN,
	FCREATE,
	FTRUNCATE,
	FREAD,
	FWRITE

} op_code;


/**
 * @struct t_paquete
 * @var t_paquete.codigoOperacion
 * El codigo de la informacion que se envia para que el servidor sepa que se le esta enviando.
 * Ver enum op_code
 * @brief El paquete a enviar a un servidor, se compone por el código de operación y el buffer.
 */
typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;

} t_paquete;

/**
 * @fn int crearConexion(char* ip, char* puerto)
 * @brief Dados una direccion IP y un puerto, se conecta a dicha IP en dicho puerto y genera el socket de conexión para interactuar con este servidor.
 * @param ip Dirección a la que conectarse.
 * @param puerto Puerto al que solicitar un socket disponible.
 * @return Socket con la conexión realizada.
 */
int crearConexion(char* ip, char* puerto);
/**
 * @fn void enviarMensaje(char* mensaje, int socketCliente)
 * @brief Se envia un paquete del tipo MENSAJE que envia un string al socket solicitado.
 * @param mensaje El string a enviar.
 * @param socketCliente El socket mediante el cual enviar el mensaje.
 */
void enviarMensaje(char* mensaje, int socketCliente);
/**
 * @fn t_paquete* crearPaquete(void)
 * @brief Se inicializa un paquete con código de operación PAQUETE.
 * @return Retorna el paquete antes inicializado, o -1 en caso de error.
 */
t_paquete* crearPaquete(void);
/**
 * @fn void agregarAPaquete(t_paquete* paquete, void* valor, int tamanio)
 * @brief Se agrega un void* al paquete elegido, apendeandolo al final del paquete.
 * @param paquete Paquete al que se agrega el valor.
 * @param valor Dato a añadir al paquete.
 * @param tamanio Tamaño del valor a añadir, usualmente un sizeof(valor) servira, pero en el caso de strings se puede utilizar (strlen(valor) + 1) * sizeof(int)
 */
void agregarAPaquete(t_paquete* paquete, void* valor, int tamanio);
/**
 * @fn void enviarPaquete(t_paquete* paquete, int socketCliente)
 * @brief Se envia un paquete mediante el socket utilizado.
 * @param paquete Paquete a enviar.
 * @param socketCliente Socket utilizado.
 */
void enviarPaquete(t_paquete* paquete, int socketCliente);
/**
 * @fn void eliminarPaquete(t_paquete* paquete)
 * @brief Elimina un paquete y libera la memoria asociada a sus datos.
 * @param paquete El puntero al paquete a eliminar.
 */
void eliminarPaquete(t_paquete* paquete);

void enviarCodOp (op_code codigoDeOperacion, int socket);

/**
 * @fn int conexion(char *SERVIDOR)
 * 
 * @brief Se realiza la conexion y se devuelve el socket generado, o un -1 en caso de fallar.
 * @param SERVIDOR Se envia el nombre del servidor al que se debe conectar para agarrar la informacion correcta de la configuracion y hacer la conexion.
 * @return int Se retorna el socket generado o un -1 en caso de fallar.
 */
int conexion(char *SERVIDOR);

/* Funciones Privadas */

/**
 * @brief Se aloca el buffer del paquete recibido.
 * @param paquete Paquete que aun no tiene el buffer inicializado.
 */
void crearBuffer(t_paquete *paquete);

/**
 * @brief Dado un paquete y la cantidad de bytes que contiene, se empaqueta la cantidad de bytes en un void,
 *        serializandolo y preparandolo para enviar a un servidor con el formato:
 *        1. codigo de operación del paquete
 *        2. tamaño del paquete
 *        3. el contenido del paquete en sí
 * @param paquete El paquete a serializar.
 * @param bytes La cantidad de bytes a serializar (usualmente sizeof(paquete)).
 * @return Se retorna el formato serializado como un void*.
 */
void *serializarPaquete(t_paquete *paquete, int bytes);

#endif /* UTILS_H_ */