/* KERNEL- cliente | MEMORIA - servidor*/
#include "kernel/include/conexiones/conexionMemoria.h"

int conexionAMemoria;

void conexionMemoria() {
    char * nombreAnterior = duplicarNombre (logger);
    logger = cambiarNombre (logger,"Kernel-Memoria");
    char * nombreAnteriorErrores = duplicarNombre (loggerError);
    loggerError = cambiarNombre (loggerError, "Errores Kernel-Memoria");

    conexionAMemoria = conexion("MEMORIA");
    logger = cambiarNombre(logger, nombreAnterior);
    loggerError = cambiarNombre(loggerError, nombreAnteriorErrores);
    free (nombreAnterior);
    free (nombreAnteriorErrores);
    return;
}

void recibirEstructurasInicialesMemoria(t_pcb* pcb) {
    
    char * nombreAnterior = duplicarNombre(logger);
    logger = cambiarNombre(logger,"Kernel-Memoria");
    
    t_paquete* peticion = crearPaquete(); 
    peticion->codigo_operacion = NEWPCB; 
    agregarAPaquete(peticion,(void*)&pcb->pid, sizeof(uint32_t));
    enviarPaquete(peticion, conexionAMemoria); 
    eliminarPaquete (peticion);
    debug ("PID <%d>: Se esta solicitando estructuras iniciales de memoria.", pcb->pid);
    recibirOperacion (conexionAMemoria);
    recibirTablaDeSegmentosActualizada (pcb);
    logger = cambiarNombre(logger, nombreAnterior);
    free (nombreAnterior);
}


void liberarMemoriaPCB(t_pcb* proceso){
    
    char * nombreAnterior = duplicarNombre(logger);
    logger = cambiarNombre(logger,"Kernel-Memoria");

    log_debug(logger, "PID <%d>: Se envia señal para eliminar estructuras en memoria.", proceso->pid);
    logger = cambiarNombre(logger, nombreAnterior);
    free (nombreAnterior);

    t_paquete* peticion = crearPaquete(); 
    peticion->codigo_operacion = ENDPCB; 
    agregarAPaquete(peticion,(void*)&proceso->pid, sizeof(uint32_t));
    enviarPaquete(peticion, conexionAMemoria); 
    eliminarPaquete (peticion);
}

