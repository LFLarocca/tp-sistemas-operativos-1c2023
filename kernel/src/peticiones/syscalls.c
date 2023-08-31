#include "kernel/include/peticiones/syscalls.h"

t_list *recursos;
char **nombresRecursos;
char* invalidResource = "INVALID_RESOURCE";
char* outOfMemory = "OUT_OF_MEMORY";
estadoProceso estadoAnterior; 
int tiempoIO;
bool hayOpFS;

void retornoContexto(t_pcb *proceso, t_contexto *contextoEjecucion){
    switch (contextoEjecucion->motivoDesalojo->comando){
        case IO:
            io_s(proceso, contextoEjecucion->motivoDesalojo->parametros);
            break;
        case WAIT:
            wait_s(proceso, contextoEjecucion->motivoDesalojo->parametros);
            break;
        case SIGNAL:
            signal_s(proceso, contextoEjecucion->motivoDesalojo->parametros);
            break;
        case YIELD:
            yield_s(proceso);
            break;
        case EXIT:
            exit_s(proceso, contextoEjecucion->motivoDesalojo->parametros);
            break;
        case F_OPEN:
            fopen_s(proceso, contextoEjecucion->motivoDesalojo->parametros);
            break;
        case F_CLOSE:
            fclose_s(proceso, contextoEjecucion->motivoDesalojo->parametros);
            break;
        case F_SEEK:
            fseek_s(proceso, contextoEjecucion->motivoDesalojo->parametros);
            break;
        case F_READ:
            fread_s(proceso, contextoEjecucion->motivoDesalojo->parametros);
            break;
        case F_WRITE:
            fwrite_s(proceso, contextoEjecucion->motivoDesalojo->parametros);
            break;
        case F_TRUNCATE:
            ftruncate_s(proceso, contextoEjecucion->motivoDesalojo->parametros);
            break;
        case CREATE_SEGMENT:
            createSegment_s(proceso, contextoEjecucion->motivoDesalojo->parametros);
            break;
        case DELETE_SEGMENT:
            deleteSegment_s(proceso, contextoEjecucion->motivoDesalojo->parametros);
            break; 
    default:
        log_error(loggerError, "Comando incorrecto, ejecutando Yield para continuar");
        yield_s(proceso);
        break;
    }
}

void volverACPU(t_pcb* proceso) {
    contextoEjecucion = procesarPCB(proceso);
    rafagaCPU = contextoEjecucion->rafagaCPUEjecutada; 
    retornoContexto(proceso, contextoEjecucion); 
    
}

void wait_s(t_pcb *proceso, char **parametros){

    char *recurso = parametros[0];

    int indexRecurso = indiceRecurso(recurso);

    if (indexRecurso == -1)
    {
        exit_s(proceso, &invalidResource); 
        return;
    }

    int instancRecurso = instanciasRecursos[indexRecurso];
    instancRecurso--;
    instanciasRecursos[indexRecurso]=instancRecurso;
    

    log_info(logger,"PID: <%d> - Wait: <%s> - Instancias: <%d>",proceso->pid, recurso, instancRecurso); 


    if(instancRecurso < 0){
        
        t_list *colaBloqueadosRecurso = (t_list *)list_get(recursos, indexRecurso);

        estadoAnterior = proceso->estado; 
        proceso->estado = BLOCK;

        list_add(colaBloqueadosRecurso, (void *)proceso);

        loggearCambioDeEstado(proceso->pid, estadoAnterior, proceso->estado);
        loggearBloqueoDeProcesos(proceso, recurso); 
        
    } 
    else {
        list_add(proceso->recursosAsignados, (void*)string_duplicate (recurso));
       
    
        volverACPU(proceso);
    }
}


void signal_s(t_pcb *proceso, char **parametros){

    char *recurso = parametros[0];

    int indexRecurso = indiceRecurso(recurso);

    if (indexRecurso == -1){
        exit_s(proceso, &invalidResource); 
        return;
    }


    int instancRecurso = instanciasRecursos[indexRecurso];
    instancRecurso++;

    log_info(logger,"PID: <%d> - Signal: <%s> - Instancias: <%d>",proceso->pid, recurso, instancRecurso); 

    eliminarRecursoLista(proceso->recursosAsignados,recurso); 

    instanciasRecursos[indexRecurso]=instancRecurso;

    if(instancRecurso <= 0){
        
        t_list *colaBloqueadosRecurso = (t_list *)list_get(recursos, indexRecurso);

        t_pcb* pcbDesbloqueado = desencolar(colaBloqueadosRecurso);

        list_add(pcbDesbloqueado->recursosAsignados, (void*)string_duplicate (recurso));

        estimacionNuevaRafaga(pcbDesbloqueado); 

        estadoAnterior = pcbDesbloqueado->estado;
        pcbDesbloqueado->estado = READY;

        loggearCambioDeEstado(pcbDesbloqueado->pid,estadoAnterior,pcbDesbloqueado->estado); 
        ingresarAReady(pcbDesbloqueado); 
    
    }
    

    if (strncmp (parametros[2], "EXIT", 4)) volverACPU(proceso);    
}


void io_s(t_pcb *proceso, char **parametros){   
    
    estadoAnterior = proceso->estado;
    proceso->estado = BLOCK; 

    loggearBloqueoDeProcesos(proceso, "IO"); 
    loggearCambioDeEstado(proceso->pid, estadoAnterior, proceso->estado);
    
    tiempoIO = atoi(parametros[0]);
    log_info(logger,"PID: <%d> - Ejecuta IO: <%d>",proceso->pid,tiempoIO); 
  
    pthread_t pcb_bloqueado;

    if (!pthread_create(&pcb_bloqueado, NULL, (void *)bloquearIO, (void *)proceso)){
        pthread_detach(pcb_bloqueado);
    } else {
        error("Error en la creacion de hilo para realizar I/O, Abort");
    } 
     
}

void bloquearIO(t_pcb * proceso){  
       
    sleep(tiempoIO);
    estimacionNuevaRafaga(proceso); 
    estadoAnterior = proceso->estado;
    proceso->estado = READY;
    loggearCambioDeEstado(proceso->pid, estadoAnterior, proceso->estado);
    ingresarAReady(proceso);
    

}

void yield_s(t_pcb *proceso){  

    estimacionNuevaRafaga(proceso); 
    estadoAnterior = proceso->estado;
    proceso->estado = READY;
    loggearCambioDeEstado(proceso->pid, estadoAnterior, proceso->estado);
    ingresarAReady(proceso); 
}

void exit_s(t_pcb* proceso, char **parametros){   
    
    estadoAnterior = proceso->estado; 
    proceso->estado = SALIDA; 

    loggearCambioDeEstado(proceso->pid, estadoAnterior, proceso->estado); 
    loggearSalidaDeProceso(proceso, parametros[0]); 
    
    enviarMensaje("Terminado", proceso->socketPCB);

    if(!list_is_empty(proceso->recursosAsignados)) {
        
        liberarRecursosAsignados(proceso);
    }

    liberarArchivosAsignados(proceso);
    liberarMemoriaPCB(proceso); 

    list_remove_element(pcbsEnMemoria, proceso);
    destruirPCB(proceso); 
    destroyContextoUnico();
    sem_post(&semGradoMultiprogramacion); 
}


void fopen_s(t_pcb *proceso, char **parametros){

    t_archivo* archivo;
    char* nombreArchivo = parametros[0];

    log_info(logger, "PID: <%d> - Abrir Archivo: <%s>",contextoEjecucion->pid,nombreArchivo);

    //primero veo si esta en la tabla global
    if(estaEnLaTablaGlobal(nombreArchivo)){
            // si esta en la tabla alguien ya lo esta usando y tengo que ponerlo en block
            archivo = obtenerArchivoDeTG(nombreArchivo);
            debug ("Lo encontre!");
            estadoAnterior = proceso->estado;
            proceso->estado = BLOCK;

            debug("Ahora la lista de bloqueados tiene %d",list_size(archivo->colaBloqueados)); 
            list_add (archivo->colaBloqueados, proceso);
            loggearCambioDeEstado(proceso->pid, estadoAnterior, proceso->estado);
            loggearBloqueoDeProcesos(proceso, nombreArchivo);
    }

    else{ // si no esta en la tabla, se lo tengo que solicitar al FS y lo asigno porque nadie lo esta usando
        debug ("tengo que pedirselo al fs");
        archivo = solicitarArchivoFS(nombreArchivo);
        t_archivoProceso* nuevoArchivo = malloc(sizeof(t_archivoProceso));
        nuevoArchivo->fcb = archivo->fcb;
        nuevoArchivo->punteroArch = 0;
        list_add(proceso->tablaDeArchivos,(void*)nuevoArchivo);
        debug ("asignado");
        volverACPU(proceso);
    }
    
}

void fclose_s(t_pcb *proceso, char **parametros){

    char* nombreArchivo = parametros[0];

    log_info(logger, "PID: <%d> - Cerrar Archivo: <%s>",proceso->pid, nombreArchivo);

    quitarArchivo(proceso, nombreArchivo);

    t_archivo* archivo = obtenerArchivoDeTG(nombreArchivo);

    debug ("Tamaño de la lista del archivo: %d", list_size (archivo->colaBloqueados));
    if(list_is_empty(archivo->colaBloqueados)){
        //si no hay procesos esperando por el archivo, lo elimino
        debug("saco archivo de tg");
        quitarArchivoTG(nombreArchivo);
    }
    else{
        // si hay procesos esperando, desbloqueo uno y le asigno el archivo
        t_archivoProceso* archivoAAsignar = malloc(sizeof(t_archivoProceso));
        archivoAAsignar->fcb = archivo->fcb;
        archivoAAsignar->punteroArch = 0;

        debug("La lista de bloqueados de este archivo tiene %d",list_size(archivo->colaBloqueados)); 

        t_pcb* procesoADesbloquear = (t_pcb*)list_remove(archivo->colaBloqueados, 0);
;
        debug("El proceso que desbloquee tiene pid %d",procesoADesbloquear->pid); 

        estadoAnterior = procesoADesbloquear->estado;
        procesoADesbloquear->estado = READY;
        list_add(procesoADesbloquear->tablaDeArchivos, archivoAAsignar);
        loggearCambioDeEstado(procesoADesbloquear->pid, estadoAnterior,procesoADesbloquear->estado);
        ingresarAReady(procesoADesbloquear);
    }
    if (proceso->estado != SALIDA)
        volverACPU(proceso);
}

void fseek_s(t_pcb *proceso, char **parametros){
    char* nombreArchivo = parametros[0];
    int puntero = atoi(parametros[1]);
    

    t_archivoProceso* archivo = obtenerArchivoDeProceso(proceso, nombreArchivo);

    if(archivo == NULL){
            debug ("Este proceso no puede realizar operaciones sobre este archivo");
            exit_s(proceso, &invalidResource);
    }
    else archivo->punteroArch = (uint32_t)puntero;

    log_info(logger, "PID: <%d> - Actualizar puntero Archivo: <%s> - Puntero <%d>",proceso->pid, nombreArchivo, archivo->punteroArch);

    volverACPU(proceso);

}

void ftruncate_s(t_pcb *proceso, char **parametros){
    char* nombreArchivo = parametros[0];
    int tamanio = atoi(parametros[1]);
    t_paquete* peticion;
    debug("accesso a tg");
    t_archivo* archivo = obtenerArchivoDeTG(nombreArchivo);

    debug("%s", archivo->fcb->nombre);

    log_info(logger, "PID: <%d> - Archivo: <%s> - Tamaño: <%d>",proceso->pid, archivo->fcb->nombre, tamanio);

    if(estaAsignadoAlProceso(nombreArchivo, proceso)){

        peticion = crearPeticionDeTruncadoDeArchivo(archivo->fcb, tamanio);

        //bloqueo al proceso
        estadoAnterior = proceso->estado;
        proceso->estado = BLOCK;

        loggearCambioDeEstado(proceso->pid, estadoAnterior, proceso->estado);
        loggearBloqueoDeProcesos(proceso, nombreArchivo);

        peticionConBloqueoAFS(peticion, proceso);
    
    }

    else{
        log_error(logger, "El proceso no puede realizar esta peticion ya que no tiene permiso sobre el archivo");
    }
    
}


void fread_s(t_pcb *proceso, char **parametros){

    char* nombreArchivo = parametros[0];
    uint32_t dirFisica = (uint32_t)atoi(parametros[1]);
    uint32_t bytes = (uint32_t)atoi(parametros[2]);
    t_paquete* peticion;
    t_archivo* archivo = obtenerArchivoDeTG(nombreArchivo); //este lo necesito por la cola de bloqueo
    t_archivoProceso* archivoProceso = obtenerArchivoDeProceso(proceso, nombreArchivo);

    if(archivoProceso == NULL){
            debug ("Este proceso no puede realizar operaciones sobre este archivo");
            exit_s(proceso, &invalidResource);
    }
    else{

        log_info(logger, "PID: <%d> - Leer Archivo: <%s> - Puntero <%d> - Dirección Memoria <%d> - Tamaño <%d>",proceso->pid, nombreArchivo, archivoProceso->punteroArch, dirFisica, archivo->fcb->tamanio);

        peticion = crearPeticionDeLecturaDeArchivo(archivoProceso, dirFisica, bytes);
        agregarAPaquete(peticion, &(proceso->pid), sizeof(uint32_t));
        
        //bloqueo al proceso, agregandolo a la lista de bloqueados correspondiente a ese archivo
        estadoAnterior = proceso->estado;
        proceso->estado = BLOCK;

        loggearCambioDeEstado(proceso->pid, estadoAnterior, proceso->estado);
        loggearBloqueoDeProcesos(proceso, nombreArchivo);

        pthread_mutex_lock(&mutexCompactacion);
        hayOpFS = true;
        peticionConBloqueoAFS(peticion, proceso);

    }
}

void fwrite_s(t_pcb *proceso, char **parametros){
    char* nombreArchivo = parametros[0];
    uint32_t dirFisica = (uint32_t)atoi(parametros[1]);
    uint32_t bytes = (uint32_t)atoi(parametros[2]);
    t_paquete* peticion;
    t_archivoProceso* archivoProceso = obtenerArchivoDeProceso(proceso, nombreArchivo);

    if(archivoProceso == NULL){
            debug ("Este proceso no puede realizar operaciones sobre este archivo");
            exit_s(proceso, &invalidResource);
    }
    else{

        log_info(logger, "PID: <%d> - Escribir Archivo: <%s> - Puntero <%d> - Dirección Memoria <%d> - Tamaño <%d>",proceso->pid, nombreArchivo, archivoProceso->punteroArch, dirFisica, bytes);

        peticion = crearPeticionDeEscrituraDeArchivo(archivoProceso, dirFisica, bytes);
        agregarAPaquete(peticion, &(proceso->pid), sizeof(uint32_t));

        //bloqueo al proceso, agregandolo a la lista de bloqueados correspondiente a ese archivo
        estadoAnterior = proceso->estado;
        proceso->estado = BLOCK;

        loggearCambioDeEstado(proceso->pid, estadoAnterior, proceso->estado);
        loggearBloqueoDeProcesos(proceso, nombreArchivo);

        pthread_mutex_lock(&mutexCompactacion);
         hayOpFS = true;
        peticionConBloqueoAFS(peticion, proceso);

    }
}


void createSegment_s(t_pcb *proceso, char **parametros){

    int idSegmento = atoi(parametros[0]);
    int tamanio = atoi(parametros[1]);
    
    t_paquete* peticion = crearPaquete();
    peticion->codigo_operacion = CREATE_SEGMENT_OP;

    agregarAPaquete(peticion, &(contextoEjecucion->pid), sizeof(uint32_t));
    agregarAPaquete(peticion, &(idSegmento), sizeof(int));
    agregarAPaquete(peticion, &(tamanio), sizeof(uint32_t));

    enviarPaquete(peticion, conexionAMemoria);

    int rdoPeticion = recibirOperacion(conexionAMemoria);

    switch(rdoPeticion){
        case SUCCESS:
                log_info(logger, "PID: <%d> - Crear Segmento - Id: <%d> - Tamaño: <%d>", proceso->pid, idSegmento, tamanio);                
                recibirOperacion (conexionAMemoria);
                recibirTablaDeSegmentosActualizada(proceso);
                eliminarPaquete (peticion);
                volverACPU(proceso);
                break;

        case OUTOFMEMORY:
                exit_s(proceso, &outOfMemory);
                eliminarPaquete (peticion);
                break;
        
        case COMPACTACION:
            
                log_info(logger, "Compactacion: Se solicito compactacion ");

                if(hayOpFS) log_info(logger, "Compactacion: Esperando Fin de Operaciones de FS");

                pthread_mutex_lock(&mutexCompactacion);

                enviarMensaje("Compactacion OK", conexionAMemoria); 
                int cantidadDeProcesosAActualizar = recibirOperacion(conexionAMemoria); 

                t_pcb* procesoAActualizar;
                uint32_t pid;
                for(int i=0;i<cantidadDeProcesosAActualizar; i++) {
                    pid = recibirOperacion(conexionAMemoria);
                    debug ("%d", pid);
                    recibirOperacion (conexionAMemoria);
                    procesoAActualizar = buscarPID(pcbsEnMemoria, pid);
                    recibirTablaDeSegmentosActualizada(procesoAActualizar);
                }

                pthread_mutex_unlock(&mutexCompactacion);

                log_info(logger,  "Se finalizo el proceso de compactacion");
                
                eliminarPaquete (peticion);
                createSegment_s (proceso, parametros);
                break;
    }
}


void deleteSegment_s(t_pcb *proceso, char **parametros){
    
    uint32_t idSegmento = (uint32_t) atoi(parametros[0]);

    log_info(logger, "PID: <%d> - Eliminar Segmento - Id: <%d>", proceso->pid, idSegmento);

    t_paquete* peticion = crearPaquete();
    peticion->codigo_operacion = DELETE_SEGMENT_OP;

    agregarAPaquete(peticion, (void*)&contextoEjecucion->pid, sizeof(uint32_t));
    agregarAPaquete(peticion, (void*)&idSegmento, sizeof(uint32_t));
    enviarPaquete(peticion, conexionAMemoria);
    
    eliminarPaquete (peticion);

    recibirOperacion(conexionAMemoria);
    recibirTablaDeSegmentosActualizada(proceso);

    volverACPU(proceso);

}


void loggearBloqueoDeProcesos(t_pcb* proceso, char* motivo) {
    log_info(logger,"PID: <%d> - Bloqueado por: %s", proceso->pid, motivo); 
}

void loggearSalidaDeProceso(t_pcb* proceso, char* motivo) {
    log_info(logger,"Finaliza el proceso <%d> - Motivo: <%s>", proceso->pid, motivo); 
}
