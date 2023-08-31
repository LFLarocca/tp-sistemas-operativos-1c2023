#include "fileSystem/include/servidorKernel.h"

int tiempoDeEspera;

void escucharAlKernel() {
    char* puertoEscucha = confGet ("PUERTO_ESCUCHA");
    socketCliente = alistarServidor (puertoEscucha);
	log_info(logger, "Se conecto el Kernel");
    ejecutarServidor();  
}

void ejecutarServidor() {
	tiempoDeEspera = config_get_int_value (config, "RETARDO_ACCESO_BLOQUE");
	while (1) {
		int cod_op = recibirOperacion(socketCliente); 
		if (cod_op == -1) return;
		int size, longDeNombre, desplazamiento = 0;
		char * nombreArchivo;
		fcb_t * nuevoArchivo, * fcbRecibido;
		uint32_t puntero, tamanio, direccionFisica, pid;
				usleep (tiempoDeEspera * 1000);
		void * data = recibirBuffer (socketCliente, &size);
		switch (cod_op) {
			case FOPEN:
				memcpy (&(longDeNombre), data + desplazamiento, sizeof (int)), desplazamiento += sizeof longDeNombre;
				nombreArchivo = malloc (sizeof (char) * longDeNombre);
				memcpy (nombreArchivo, data + desplazamiento, longDeNombre), desplazamiento += longDeNombre;
				log_info (logger, "Abrir Archivo: <%s>", nombreArchivo);
				nuevoArchivo = abrirArchivo (nombreArchivo);
				if (nuevoArchivo == NULL) enviarMensaje ("Apertura de archivo fallida, desea crear el archivo?", socketCliente);
				else enviarArchivo (nuevoArchivo, socketCliente);
				free (nombreArchivo), free (data);
				break;

			case FCREATE:
				memcpy (&(longDeNombre), data + desplazamiento, sizeof (int)), desplazamiento += sizeof longDeNombre;
				nombreArchivo = malloc (sizeof (char) * longDeNombre);
				memcpy (nombreArchivo, data + desplazamiento, longDeNombre), desplazamiento += longDeNombre;
				log_info (logger, "Crear Archivo: <%s>", nombreArchivo);
				if (crearArchivo(nombreArchivo) < 0) {
					enviarMensaje ("Fallo en la creacion de archivo :(", socketCliente);
					free (data), free (nombreArchivo);
					break;
				}
				nuevoArchivo = abrirArchivo (nombreArchivo);
				if (nuevoArchivo == NULL) enviarMensaje ("Fallo en la apertura de archivo :(", socketCliente);
				else enviarArchivo (nuevoArchivo, socketCliente);
				free (data), free (nombreArchivo);
				break;

			case FTRUNCATE:
				fcbRecibido = recibirArchivo (data, &(desplazamiento));
				desplazamiento += sizeof (int);
				uint32_t tamanioNuevo;
				memcpy (&(tamanioNuevo), data + desplazamiento, sizeof tamanioNuevo);
				log_info (logger, "Truncar Archivo: <%s> - Tamaño: <%d>", fcbRecibido->nombre, tamanioNuevo);
				if (truncarArchivo (fcbRecibido, tamanioNuevo) < 0) enviarMensaje ("Fallo truncacion de archivo :(", socketCliente);
				else enviarArchivo (fcbRecibido, socketCliente);
				free (data);
				break;

			case FREAD:
				fcbRecibido = recibirArchivo (data, &(desplazamiento));
				desplazamiento += sizeof (int);
				memcpy (& (puntero), data + desplazamiento, sizeof puntero);
				desplazamiento += sizeof (int) + sizeof puntero;
				memcpy (& (direccionFisica), data + desplazamiento, sizeof direccionFisica);
				desplazamiento += sizeof (int) + sizeof direccionFisica;
				memcpy (& (tamanio), data + desplazamiento, sizeof tamanio);
				desplazamiento += sizeof (int) + sizeof tamanio;
				memcpy (& (pid), data + desplazamiento, sizeof pid);
				log_info (logger, "Leer Archivo: <%s> - Puntero: <%d> - Memoria: <%d> - Tamaño: <%d>", fcbRecibido->nombre, puntero, direccionFisica, tamanio);
				char * leido = leerArchivo (fcbRecibido, puntero, tamanio);
				enviarAMemoria (leido, direccionFisica, tamanio, conexionAMemoria, pid);
				recibirOperacion (conexionAMemoria);
				char * mensaje = recibirMensaje (conexionAMemoria);
				if (!strcmp (mensaje, "Ejecucion correcta :)"))
					enviarMensaje("Yayay :)", socketCliente);
				free (mensaje), free (leido), free (fcbRecibido->nombre), free (fcbRecibido), free (data);
				break;

			case FWRITE:
				fcbRecibido = recibirArchivo (data, &(desplazamiento));
				desplazamiento += sizeof (int);
				memcpy (& (puntero), data + desplazamiento, sizeof puntero);
				desplazamiento += sizeof (int) + sizeof puntero;
				memcpy (& (direccionFisica), data + desplazamiento, sizeof direccionFisica);
				desplazamiento += sizeof (int) + sizeof direccionFisica;
				memcpy (& (tamanio), data + desplazamiento, sizeof tamanio);
				desplazamiento += sizeof (int) + sizeof tamanio;
				memcpy (& (pid), data + desplazamiento, sizeof pid);
				log_info (logger, "Escribir Archivo: <%s> - Puntero: <%d> - Memoria: <%d> - Tamaño: <%d>", fcbRecibido->nombre, puntero, direccionFisica, tamanio);
				char * aEscribir = solicitarAMemoria (direccionFisica, tamanio, conexionAMemoria, pid);
				if (escribirArchivo (fcbRecibido, aEscribir, tamanio, puntero) < 0)
					enviarMensaje ("Fallo :(", socketCliente);
				else enviarMensaje ("Yayayay!", socketCliente);
				free (aEscribir), free (fcbRecibido->nombre), free (fcbRecibido), free (data);
				break;

			case -1:
				log_info (logger, "Kernel desconectado.");
				return;

			default:
				break;
		}
	}
}

fcb_t * recibirArchivo (void * data, int *desplazamiento) {
	fcb_t * fcbNuevo = malloc (sizeof (fcb_t));
	int longNombre;
	memcpy (&(longNombre), data + (* desplazamiento), sizeof longNombre);
	(* desplazamiento) += sizeof longNombre;
	fcbNuevo->nombre = malloc (sizeof (char) * longNombre);
	memcpy (fcbNuevo->nombre, data + (* desplazamiento), longNombre);
	(* desplazamiento) += longNombre + sizeof (int);
	memcpy (& (fcbNuevo->tamanio), data + (* desplazamiento), sizeof (uint32_t));
	(* desplazamiento) += sizeof (uint32_t) + sizeof (int);
	memcpy (& (fcbNuevo->ptrDirecto), data + (* desplazamiento), sizeof (uint32_t));
	(* desplazamiento) += sizeof (uint32_t) + sizeof (int);
	memcpy (& (fcbNuevo->ptrIndirecto), data + (* desplazamiento), sizeof (uint32_t));
	(* desplazamiento) += sizeof (uint32_t);
	return fcbNuevo;
	
}

int enviarArchivo (fcb_t * archivo, int socket) {
	t_paquete * paquete = crearPaquete ();
	agregarAPaquete (paquete, archivo->nombre, (string_length(archivo->nombre)+1) * sizeof (char));
	agregarAPaquete (paquete, &(archivo->tamanio), sizeof archivo->tamanio);
	agregarAPaquete (paquete, &(archivo->ptrDirecto), sizeof archivo->ptrDirecto);
	agregarAPaquete (paquete, &(archivo->ptrIndirecto), sizeof archivo->ptrIndirecto);
	enviarPaquete (paquete, socket);
	eliminarPaquete (paquete), cerrarArchivo (archivo);
	return 0;
}