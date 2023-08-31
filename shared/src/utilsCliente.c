#include "shared/include/utilsCliente.h"

void *serializarPaquete(t_paquete *paquete, int bytes)
{
	void *magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento += paquete->buffer->size;

	return magic;
}

int crearConexion(char *ip, char *puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socketCliente = socket(server_info->ai_family,
								server_info->ai_socktype,
								server_info->ai_protocol);

	if (!connect(socketCliente, server_info->ai_addr, server_info->ai_addrlen))
	{
		freeaddrinfo(server_info);
		return socketCliente;
	}
	else
	{
		freeaddrinfo(server_info);
		return -1;
	}
}

void enviarMensaje(char *mensaje, int socket)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2 * sizeof(int);

	void *a_enviar = serializarPaquete(paquete, bytes);

	send(socket, a_enviar, bytes, 0);

	free(a_enviar);
	eliminarPaquete(paquete);
}

//lo que hacen las funciones crear buffer y crear paquete ya lo hace la funcion enviar mensaje
void crearBuffer(t_paquete *paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete *crearPaquete(void)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = PAQUETE;
	crearBuffer(paquete);
	return paquete;
}

void agregarAPaquete(t_paquete *paquete, void *valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void enviarPaquete(t_paquete *paquete, int socket)
{
	int bytes = paquete->buffer->size + 2 * sizeof(int);
	void *a_enviar = serializarPaquete(paquete, bytes);

	debug ("Enviando paquete con tamaño %d, de %d bytes.", paquete->buffer->size, bytes);

	send(socket, a_enviar, bytes, 0);

	free(a_enviar);
}

void eliminarPaquete(t_paquete *paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void enviarCodOp (op_code codigoDeOperacion, int socket) {
	void * buffer = malloc (sizeof (int));
	memcpy (buffer, &(codigoDeOperacion), sizeof (int));
	send (socket, buffer, sizeof (int), 0);
	free (buffer);
}

int conexion(char *SERVIDOR)
{
	char *KEYS[] = {
		string_from_format("PUERTO_%s", SERVIDOR),
		string_from_format("IP_%s", SERVIDOR),
	};
	char *puerto = confGet(KEYS[0]);
	free(KEYS[0]);
	char *ip = confGet(KEYS[1]);
	free(KEYS[1]);
	int conexion = -1;
	while (conexion == -1) {
		conexion = crearConexion(ip, puerto);
		if (conexion == -1) {
			log_warning (logger, "No se pudo conectar a %s, esperando %d segundos para intentar nuevamente.", SERVIDOR, SEGS_ANTES_DE_REINTENTO);
			sleep (SEGS_ANTES_DE_REINTENTO);
		}
	}
	log_info(logger, "Conectado a %s, en socket %d", SERVIDOR, conexion);

	return conexion;
}