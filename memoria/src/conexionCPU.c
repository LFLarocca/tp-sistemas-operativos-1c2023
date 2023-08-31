#include "memoria/include/conexionCPU.h"

int tiempo;
char* valorLeido; 

int ejecutarServidorCPU(int * socketCliente){

	tiempo = config_get_int_value(config,"RETARDO_MEMORIA");

	while (1) {
		int peticion = recibirOperacion(*socketCliente);
		debug("Se recibio peticion %d del CPU", peticion); 

		switch (peticion) {
			case READ:
				recibirPeticionDeLectura(*socketCliente);
				enviarValorObtenido(*socketCliente); 
				break;
			case WRITE:
				recibirPeticionDeEscritura(*socketCliente);
                enviarMensaje("OK",*socketCliente); 
				break;
			case -1:
				log_error(logger, "El CPU se desconecto");
				return EXIT_FAILURE;
				break; 
			default:
				log_warning(logger,"Operacion desconocida del CPU.");
				break;
		}
	}
} 

char* leer(int32_t direccionFisica,int tamanio) {

	usleep(tiempo*1000); 

	char* punteroDireccionFisica = espacioDeUsuario + direccionFisica; 

	char* valor = malloc(sizeof(char)*tamanio); 
	
	memcpy(valor, punteroDireccionFisica, tamanio);

	return valor; 

}

void recibirPeticionDeLectura(int socketCPU) {

	int size, desplazamiento=0, pid, tamanio;
	int32_t direccionFisica;

	void* buffer = recibirBuffer(socketCPU, &size);
	desplazamiento += sizeof(uint32_t);
	memcpy(&(pid), buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(uint32_t) + sizeof (int);
	memcpy(&(direccionFisica), buffer + desplazamiento, sizeof(int32_t));
	desplazamiento += sizeof(uint32_t) + sizeof(int); 
	memcpy(&(tamanio),buffer+desplazamiento,sizeof(int)); 

	valorLeido = leer(direccionFisica, tamanio); 
	valorLeido = realloc (valorLeido, tamanio + 1);
	valorLeido[tamanio] = '\0';

	log_info(logger, "PID: <%d> - Acción: <%s> - Dirección física: <%d> - Tamaño: <%d> - Origen: <%s>", pid, "LEER", direccionFisica, tamanio, "CPU");

	free (buffer);
}

void recibirPeticionDeEscritura(int socketCPU) {
	
	int size, desplazamiento=0, tamanio, pid;
	int32_t direccionFisica;

	void* buffer = recibirBuffer(socketCPU, &size);
	desplazamiento += sizeof(uint32_t);
	memcpy(&(pid), buffer + desplazamiento, sizeof(int));
	desplazamiento+=sizeof(uint32_t) + sizeof (int); 
	memcpy(&(direccionFisica), buffer + desplazamiento, sizeof(int32_t));
	desplazamiento+=sizeof(uint32_t); 
	memcpy (&(tamanio), buffer + desplazamiento, sizeof tamanio);
	desplazamiento += sizeof tamanio;
	char* valorAEscribir = malloc (sizeof (char) * tamanio);
	memcpy(valorAEscribir,buffer+desplazamiento,sizeof(char)*tamanio);
	
	tamanio--; 

	escribir(valorAEscribir,direccionFisica, tamanio);  


	log_info(logger, "PID: <%d> - Acción: <%s> - Dirección física: <%d> - Tamaño: <%d> - Origen: <%s>", pid, "ESCRIBIR", direccionFisica, tamanio, "CPU");
	free (buffer);
}

void enviarValorObtenido(int socketCPU){
	
	enviarMensaje(valorLeido, socketCPU);
	free(valorLeido);
}

void escribir(char* valor, int32_t direccionFisica, int tamanio){
	
	usleep(tiempo*1000); 

	char* punteroADirFisica = espacioDeUsuario + direccionFisica; 

	memcpy(punteroADirFisica, valor, tamanio);

	free(valor); 
}