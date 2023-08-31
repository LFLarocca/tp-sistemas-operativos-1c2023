#include "memoria/include/memoria.h"

int sockets[3];
pthread_t threadCPU, threadFS, threadKernel;

int main() {

    logger = iniciarLogger ("memoria.log", "Memoria");
	loggerError = iniciarLogger ("memoriaErrores.log","Memoria (Errores)"); 
	config = iniciarConfiguracion ("memoria.config");

	atexit (terminarPrograma);

	creacionEspacio();

	creacionListaHuecosLibres(); 
	tablaDeTablasDeSegmentos=list_create(); 
	atexit (liberarTabladeTablasDeSegmentos);

	//Inicio servidor y queda a la espera de las conexiones de CPU, Kernel y File System
	
	int server_fd = iniciarServidor (confGet("PUERTO_ESCUCHA"));

	log_info (logger, "Memoria lista para recibir conexiones.");
	log_info (logger, "Esperando CPU...");

	sockets[0] = esperarCliente(server_fd);
	
	usleep (1000 * 500);
	log_info (logger, "Esperando File System...");
	sockets[1] = esperarCliente (server_fd);

	usleep (1000 * 500);
	log_info (logger, "Esperando Kernel...");
	sockets[2] = esperarCliente (server_fd);

	int opCodes[3] = {
		pthread_create (&threadCPU, NULL, (void*) ejecutarServidorCPU, (void*) &sockets[0]),
		pthread_create (&threadFS, NULL, (void*)ejecutarServidorFS, (void*) &sockets[1]),
		pthread_create (&threadKernel, NULL, (void*) ejecutarServidorKernel, (void*) &sockets[2])
	};

    if (opCodes [0]) {
        error ("Error en iniciar el servidor a CPU");
	}
    if (opCodes [1]) {
        error ("Error en iniciar el servidor a FS");
	}
    if (opCodes [2]) {
        error ("Error en iniciar el servidor a Kernel");
	}

	pthread_join (threadCPU, NULL);
	pthread_join (threadFS, NULL);
	pthread_join (threadKernel, NULL);

	exit (0);
}

void liberarTabladeTablasDeSegmentos () {

	list_destroy_and_destroy_elements(tablaDeTablasDeSegmentos, (void*)eliminarTabla);
}

void eliminarTabla(t_proceso* proceso){

	list_remove(proceso->tablaDeSegmentosAsociada,0);
	list_destroy_and_destroy_elements(proceso->tablaDeSegmentosAsociada, free);
	free(proceso);
}