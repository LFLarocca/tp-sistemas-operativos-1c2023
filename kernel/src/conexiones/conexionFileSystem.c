/* KERNEL- cliente | FYLESYSTEM - sevidor*/
#include "kernel/include/conexiones/conexionFileSystem.h"

int conexionAFS; 

 void conexionFileSystem(){
    char * nombreAnterior = duplicarNombre (logger);
    logger = cambiarNombre(logger,"Kernel-FS");
    char * nombreAnteriorErrores = duplicarNombre (loggerError);
    loggerError = cambiarNombre(loggerError,"Errores Kernel-FS");

    conexionAFS = conexion("FILESYSTEM");
    logger = cambiarNombre(logger, nombreAnterior);
    loggerError = cambiarNombre(loggerError, nombreAnteriorErrores);

    free (nombreAnterior);
    free (nombreAnteriorErrores);
    return;
}
