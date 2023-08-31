#include "consola/include/consola.h"

int socketKernel;
t_log *logger;
t_log *loggerError;
t_config *config;
FILE *codigo;

int main (int, char *archivos[]) {
    logger = iniciarLogger ("consola.log", "Consola");
    loggerError = iniciarLogger ("consolaErrores.log", "Consola (Errores)");
    config = iniciarConfiguracion (archivos[2]);
    atexit (terminarPrograma);

    socketKernel = conexion ("KERNEL");
    atexit (cerrarConexion);
    if (!strcmp (archivos[1], "matar")) {
        enviarCodOp (TERMINAR_KERNEL, socketKernel);
        log_info (logger, "Terminando Kernel, porque harias esto?");
        exit(0);
    }

    codigo = abrir (archivos[1], "r");
    atexit (cerrarArchivo);
    size_t cantChars = MAX_CHARS;

    t_paquete *paquete = crearPaquete ();

    while(!feof(codigo)) {
        char *linea = (char *) malloc (sizeof (char) * cantChars);
        if (linea == NULL) error ("No se pudo alocar memoria");

        getline(&linea, &cantChars, codigo);
        char *lineaCodigo = string_replace(linea, "\n\0", "\0");

        agregarAPaquete (paquete, lineaCodigo, sizeof (char) * (strlen (lineaCodigo) + 1));
        free (lineaCodigo);
        free (linea);
    }

    enviarPaquete (paquete, socketKernel);
    eliminarPaquete (paquete);

    enviarMensaje ("Fin de instrucciones", socketKernel);
    esperarPID (archivos[1]);
    esperarFinalizacion ();
    
    exit (0);
}

void esperarPID(char * archivo) {
    recibirOperacion (socketKernel);
    char * pidString = recibirMensaje (socketKernel);
    char * nuevoNombre = string_from_format ("Consola - <%s> - <%s>", archivo, pidString);
    free (pidString);
    logger = cambiarNombre (logger, nuevoNombre);
    free (nuevoNombre);
    return;
}

void esperarFinalizacion () {
    recibirOperacion (socketKernel);
	char * mensaje = recibirMensaje (socketKernel);
    log_info(logger, "%s", mensaje);
    free(mensaje);
    return;
}

void cerrarConexion () {
    close (socketKernel);
    return;
}

void cerrarArchivo () {
    fclose (codigo);
    return;
}