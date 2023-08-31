#include "memoria/include/conexionFS.h"

int ejecutarServidorFS(int *socketCliente){

	tiempo = config_get_int_value(config,"RETARDO_MEMORIA"); 

	log_info(logger, "Conectado el File System");
	
	while (1) {
		int peticion = recibirOperacion(*socketCliente);
		debug("Se recibio peticion %d de File System", peticion); 
		switch (peticion) {	
			case READ:
				log_debug (logger, "Solicitud de lectura");
				char * temp = recibirPeticionDeLecturaFS(*socketCliente); 
				debug (temp);
				enviarMensaje(temp,*socketCliente); 
				free(temp);
				break;
			case WRITE:
                log_debug (logger, "Solicitud de escritura");
				recibirPeticionDeEscrituraFS(*socketCliente); 
				enviarMensaje("Ejecucion correcta :)", *socketCliente); 
				break;
			case -1:
				log_warning (logger, "El File System se desconecto");
				return 0;
				break; 
			default:
				log_warning (logger,"Operacion desconocida. No quieras meter la pata");
				break;
		}
	}
}

//FREAD
char * recibirPeticionDeLecturaFS(int socketFS) {

	int size, desplazamiento=0, pid, tamanio;
	int32_t direccionFisica;

	void* buffer = recibirBuffer(socketFS, &size);
	desplazamiento += sizeof(uint32_t);
	memcpy(&(pid), buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(uint32_t) + sizeof (int);
	memcpy(&(direccionFisica), buffer + desplazamiento, sizeof(int32_t));
	desplazamiento += sizeof(uint32_t) + sizeof(int); 
	memcpy(&(tamanio),buffer+desplazamiento,sizeof(int)); 

	char * temp = leer_fs(direccionFisica, tamanio); 
	char * valorLeido = malloc (tamanio + 1);
	memcpy (valorLeido, temp, tamanio);
	valorLeido[tamanio] = '\0';	
	
	log_info(logger, "PID: <%d> - Acción: <%s> - Dirección física: <%d> - Tamaño: <%d> - Origen: <%s>", pid, "LEER", direccionFisica, tamanio, "FS");

	free (buffer), free (temp);
	return valorLeido;
}

char* leer_fs(int32_t direccionFisica,int tamanio) {

	usleep(tiempo*1000); 

	char* punteroDireccionFisica = espacioDeUsuario + direccionFisica; 

	char* valorLeido = malloc(sizeof(char)*tamanio); 
	
	memcpy(valorLeido, punteroDireccionFisica, tamanio);

	return valorLeido; 

}

void escribir_fs(char* valor, int32_t direccionFisica, int tamanio){
	
	usleep(tiempo*1000); 

	char* punteroADirFisica = espacioDeUsuario + direccionFisica; 

	memcpy(punteroADirFisica, valor, tamanio);
}


//FWRITE

void recibirPeticionDeEscrituraFS(int socketFS){

	int size, desplazamiento=0, tamanio, pid;
	int32_t direccionFisica;

	void* buffer = recibirBuffer(socketFS, &size);
	desplazamiento += sizeof(uint32_t);
	memcpy(&(pid), buffer + desplazamiento, sizeof(int));
	desplazamiento+=sizeof(uint32_t) + sizeof (int); 
	memcpy(&(direccionFisica), buffer + desplazamiento, sizeof(int32_t));
	desplazamiento+=sizeof(uint32_t); 
	memcpy (&(tamanio), buffer + desplazamiento, sizeof tamanio);
	desplazamiento += sizeof tamanio;
	char* valorAEscribir = malloc (sizeof (char) * tamanio);
	memcpy(valorAEscribir,buffer+desplazamiento,sizeof(char)*tamanio); 

	escribir_fs(valorAEscribir, direccionFisica, tamanio); 

	log_info(logger, "PID: <%d> - Acción: <%s> - Dirección física: <%d> - Tamaño: <%d> - Origen: <%s>", pid, "ESCRIBIR", direccionFisica, tamanio, "FS");

	free (buffer);

	free(valorAEscribir); 
}

