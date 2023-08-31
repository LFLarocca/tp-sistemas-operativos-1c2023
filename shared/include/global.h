#ifndef GLOBAL_H_
#define GLOBAL_H_

    // Librerias de UNIX
    #include <signal.h>
    // Librerias de so-commons
    #include <commons/log.h>
    #include <commons/config.h>

    /**
     * @enum t_comando
     * @brief Lista de comandos enumerados.
     */

    #define NUM_COMANDOS 16
    typedef enum {
        SET, MOV_IN, MOV_OUT, IO,
        F_OPEN, F_CLOSE, F_SEEK, F_READ, F_WRITE, F_TRUNCATE, F_CREATE,
        WAIT, SIGNAL,
        CREATE_SEGMENT, DELETE_SEGMENT,
        YIELD, EXIT
    } t_comando; 

    /**
     * @struct t_instruccion
     * @brief Estructura que sostiene las instrucciones para enviar del Kernel a la CPU (O para que la CPU Parsee la informacion recibida.)
     */
    typedef struct {
        t_comando comando; 
        char* parametros[3]; 
    }t_instruccion; 

#endif
