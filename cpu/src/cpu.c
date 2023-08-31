#include "cpu/include/cpu.h"


int main(void){
	
	logger = iniciarLogger("cpu.log", "CPU");
	loggerError = iniciarLogger("errores.log", "Errores CPU");
	
	config = iniciarConfiguracion("cpu.config");

	atexit(terminarPrograma);
	
	conexionMemoria(); 

	char * nombre = string_duplicate("CPU-KERNEL");
	cambiarNombre(logger, nombre);
    escucharAlKernel();

	free (nombre);
	return EXIT_SUCCESS;
}	