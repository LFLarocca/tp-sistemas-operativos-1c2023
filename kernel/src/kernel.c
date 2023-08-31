/* - Interprete entre el hardware y el software - */
#include "kernel/include/kernel.h"


int socketCliente;
t_log* logger;
t_log* loggerError;
t_config* config;
pthread_t planificadorLargoPlazo_h, planificadorCortoPlazo_h, recibirConsolas_h;


int main () {
    //Inicializar variables
    logger = iniciarLogger("kernel.log", "Kernel");
	loggerError = iniciarLogger("errores.log", "Errores - Kernel"); 
    config = iniciarConfiguracion("kernel.config");
	atexit (terminarPrograma);
	char* puertoDeEscucha = confGet("PUERTO_ESCUCHA"); 
	iniciarTablaGlobalDeArchivos();

	inicializarSemaforos();
	atexit (destruirSemaforos);
	inicializarListasPCBs(); 
	atexit (destruirListasPCBs);
	
	conexionCPU();
	conexionFileSystem(); 
	conexionMemoria(); 

    //Inicializar Hilos
	int opCodes [3] = {
		pthread_create(&planificadorLargoPlazo_h, NULL, (void *) planificarALargoPlazo, NULL),
		pthread_create(&planificadorCortoPlazo_h, NULL, (void*) planificarACortoPlazoSegunAlgoritmo, NULL),
		pthread_create(&recibirConsolas_h, NULL,(void *) recibirConsolas, puertoDeEscucha)
	};

    if (opCodes [0]) {
        error ("Error al generar hilo para el planificador de largo plazo, terminando el programa.");	
	}
	if (opCodes [1]) {
        error ("Error al generar hilo para el planificador de corto plazo, terminando el programa.");
	}	
  	if (opCodes [2]) {
		error ("Error al generar hilo para recibir consolas, terminando el programa.");
	}
	//Hilo Planificador Largo Plazo -> Mueve procesos de NEW a READY
	pthread_join(planificadorLargoPlazo_h, NULL);
	//Hilo Planificador Corto Plazo --> Mueve procesos de READY a EXEC
	pthread_join(planificadorCortoPlazo_h, NULL);
	// Hilo Principal -> Recibe consolas y crea PCBs 
	pthread_join(recibirConsolas_h, NULL);
    exit (0);
}
