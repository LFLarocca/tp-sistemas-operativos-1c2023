#include "shared/include/configuraciones.h"

bool imprimirLogs = true;
t_log_level nivelDeLog = LOG_LEVEL_INFO;

// CONFIG

t_config* iniciarConfiguracion(char* ruta){

    t_config* nuevoConfig = config_create(ruta); 
	
    if(nuevoConfig==NULL) {
		log_error(logger,"Error al generar archivo de config"); 
		log_destroy(logger); 
		exit(1); 
	}

    return nuevoConfig; 
}

// LOGS 

t_log* iniciarLogger(char* nombreArchivo, char* nombreLogger){
	t_log* nuevoLogger = log_create(nombreArchivo, nombreLogger, imprimirLogs, nivelDeLog);

	if(nuevoLogger == NULL) {
		perror("No se pudo crear el logger");
		exit(1);
	}

	return nuevoLogger;
}

t_log *cambiarNombre(t_log* logger, char *nuevoNombre) {
    t_log *nuevoLogger = logger;
	free(logger->program_name);
    nuevoLogger->program_name = strdup(nuevoNombre);
    return nuevoLogger;
}

char * duplicarNombre(t_log *logger) {
    return string_duplicate(logger->program_name);
}

void terminarPrograma() {
	log_destroy(logger);
    log_destroy(loggerError);
	config_destroy(config);
}

FILE * abrir(char *archivo, char *tipoDeArchivo) {
    FILE *codigo = fopen(archivo, tipoDeArchivo);
    if (codigo == NULL) error("No se pudo abrir el archivo %s", archivo);
    return codigo;
}

void debug (char *mensajeFormato, ...) {
    va_list argumentos;
    va_start (argumentos, mensajeFormato);

    char *mensajeCompleto = string_from_vformat (mensajeFormato, argumentos);

    log_debug (logger, "%s", mensajeCompleto);

    free (mensajeCompleto);

    va_end (argumentos);
}

void error (char *mensajeFormato, ...) {
    va_list argumentos;
    va_start (argumentos, mensajeFormato);

    char *mensajeCompleto = string_from_vformat (mensajeFormato, argumentos);

    log_error (loggerError, "%s", mensajeCompleto);

    free (mensajeCompleto);

    va_end (argumentos);
    exit(1);
}
