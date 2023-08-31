#ifndef CONFIGURACIONES_H
#define CONFIGURACIONES_H

    // Librerias estandar
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    // Librerias de UNIX
    #include <unistd.h>
    #include <sys/socket.h>
    // Librerias de so-commons
    #include <commons/log.h>
    #include <commons/config.h>
    #include <commons/string.h>

    #define confGet(key) config_get_string_value(config, key)

    extern t_log *logger;
    extern t_log *loggerError;
    extern t_config *config;

    extern bool imprimirLogs;
    extern t_log_level nivelDeLog;

    // Funciones de config
    /**
     * @fn t_config* iniciarConfiguracion(char* ruta)
     * @brief Abre un archivo de configuración o retorna nulo.
     * @param ruta Ruta/nombre del archivo de configuración.
     * @return Retorna un archivo de configuración, o en caso de fallar un NULL.
     */
    t_config* iniciarConfiguracion(char* ruta);

    /**
     * @fn t_log* iniciarLogger(char* nombreArchivo, char* nombreLogger)
     * @brief Crea un logger con los parametros indicados.
     * @param nombreArchivo Archivo donde se guardan los logs.
     * @param nombreLogger Nombre del logger.
     * @return Retorna el logger creado.
     */
    t_log* iniciarLogger(char* nombreArchivo, char* nombreLogger);

    /**
     * @fn t_log *cambiarNombre(char *nuevoNombre)
     * @brief Se agarra un t_log, se le modifica el nombre y se envia el logger modificado commo resultado (El anterior nombre se elimina).
     * @param nuevoNombre El nombre que va a reemplazar el anterior.
     * @return t_log* Se retorna el logger modificado.
     */
    t_log *cambiarNombre(t_log* logger, char *nuevoNombre);

    char * duplicarNombre(t_log *logger);

    /**
     * @fn void terminarPrograma()
     * @brief Programa utilizado para eliminar tanto el Config global como el Logger global en terminacion del programa.
     */
    void terminarPrograma();

    /**
     * @fn FILE *abrir (char *archivo, char *tipoDeArchivo)
     * Funcion hecha para abrir archivos y comprobar su apertura.
     * @param archivo Nombre del archivo a recibir, tiene que estar en la carpeta directa donde se corre el programa.
     * @param tipoDeArchivo Linea de texto del tipo de archivo para abrir, para referencia ver fopen(3).
     * @return FILE* Retorna el archivo abierto.
     */
    FILE *abrir (char *archivo, char *tipoDeArchivo);

    void debug (char * mensajeFormato, ...);

    /**
     * @fn void error (char *mensajeFormato, ...)
     * Funcion de error para terminar el programa en caso de fallo.
     * @param mensajeFormato Formato del mensaje como un printf para mandarlo por log_error().
     * @param ... Resto de los parametros en caso de mandarse.
     */

    void error (char *mensajeFormato, ...); 

#endif /* CONFIGURACIONES_H_ */