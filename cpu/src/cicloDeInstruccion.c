#include "cpu/include/cicloDeInstruccion.h"

char *listaComandos[] = {
    [SET] = "SET",
    [MOV_IN] = "MOV_IN",
    [MOV_OUT] = "MOV_OUT", 
    [IO] = "I/O",
    [F_OPEN] = "F_OPEN",
    [F_CLOSE] = "F_CLOSE", 
    [F_SEEK] = "F_SEEK",
    [F_READ] = "F_READ",
    [F_WRITE] = "F_WRITE", 
    [F_TRUNCATE] = "F_TRUNCATE",
    [F_CREATE] = "F_CREATE",
    [WAIT] = "WAIT",
    [SIGNAL] = "SIGNAL",
    [CREATE_SEGMENT] = "CREATE_SEGMENT",
    [DELETE_SEGMENT] = "DELETE_SEGMENT",
    [YIELD] = "YIELD",
    [EXIT] = "EXIT"
};
char* instruccionAEjecutar; 
char** elementosInstruccion; 
int instruccionActual; 
int cantParametros;
int tiempoEspera;
int nroSegmento;

t_temporal* rafagaCPU; 
int64_t rafagaCPUEjecutada; 

void cicloDeInstruccion(){
    fetch();//busca la próxima instruccion a ejecutar. Lista en pcb
    decode();//interpreta que instruccion va a ejecutar y si requiere traduccion logica o fisica
    execute();//ejecuta la instruccion 
    liberarMemoria();
}

// ------- Funciones del ciclo ------- //
void fetch() { 
    int numInstruccionABuscar = contextoEjecucion->programCounter;
    instruccionAEjecutar = list_get(contextoEjecucion->instrucciones,numInstruccionABuscar); 
    contextoEjecucion->programCounter += 1;
}

void decode(){
    elementosInstruccion = string_n_split(instruccionAEjecutar, 4, " ");
    cantParametros = string_array_size(elementosInstruccion) - 1;
    instruccionActual = buscar(elementosInstruccion[0], listaComandos);
}

int buscar(char *elemento, char **lista) {
    int i = 0;
    //for (; strcmp(lista[i], elemento) && i < string_array_size(lista); i++);
    while (i <= string_array_size (lista)) {
        if (i < string_array_size (lista))
            if (!strcmp (elemento, lista[i]))
                return i;
    //    debug ("%s", lista[i]);
        i++;
    }
    return (i > string_array_size(lista)) ? -1 : i;
}
 
void execute() {

    switch(cantParametros) {
        case 0:
            log_info(logger, "PID: <%d> - Ejecutando: <%s> ", contextoEjecucion->pid, listaComandos[instruccionActual]);
            break;
        case 1:
            log_info(logger, "PID: <%d> - Ejecutando: <%s> -  <%s>", contextoEjecucion->pid, listaComandos[instruccionActual], elementosInstruccion[1]);
            break;
        case 2:   
            log_info(logger, "PID: <%d> - Ejecutando: <%s> - <%s>, <%s>", contextoEjecucion->pid, listaComandos[instruccionActual], elementosInstruccion[1], elementosInstruccion[2]);
            break;
        case 3:
            log_info(logger, "PID: <%d> - Ejecutando: <%s> - <%s>, <%s>, <%s>", contextoEjecucion->pid, listaComandos[instruccionActual], elementosInstruccion[1], elementosInstruccion[2], elementosInstruccion[3]);
            break; 
    }
    switch(instruccionActual){
        case SET:
            set_c(elementosInstruccion[1], elementosInstruccion[2]);
            break;
        case MOV_IN:
            mov_in(elementosInstruccion[1], elementosInstruccion[2]);
            break;
        case MOV_OUT:
            mov_out(elementosInstruccion[1], elementosInstruccion[2]);
            break;
        case IO:
            io(elementosInstruccion[1]);
            break;
        case F_OPEN:
            f_open(elementosInstruccion[1]);
            break;
        case F_CLOSE:
            f_close(elementosInstruccion[1]);
            break;
        case F_SEEK:
            f_seek(elementosInstruccion[1], elementosInstruccion[2]);
            break;
        case F_READ:
            f_read(elementosInstruccion[1], elementosInstruccion[2], elementosInstruccion[3]);
            break;
        case F_WRITE:
            f_write(elementosInstruccion[1], elementosInstruccion[2], elementosInstruccion[3]);
            break;
        case F_TRUNCATE:
            f_truncate(elementosInstruccion[1], elementosInstruccion[2]);
            break;
        case WAIT:
            wait_c(elementosInstruccion[1]);
            break;
        case SIGNAL:
            signal_c(elementosInstruccion[1]);
            break;
        case CREATE_SEGMENT:
            create_segment(elementosInstruccion[1], elementosInstruccion[2]);
            break;
        case DELETE_SEGMENT:
            delete_segment(elementosInstruccion[1]);
            break;
        case YIELD: 
            yield_c();
            break;
        case EXIT:
            exit_c();
            break;
        default:
            //sleep(1);
            break;
    }
}

// Instrucciones

void set_c(char* registro, char* valor){ 
    tiempoEspera = obtenerTiempoEspera();
    usleep(tiempoEspera * 1000); 
    dictionary_remove_and_destroy(contextoEjecucion->registrosCPU, registro, free); 
    dictionary_put(contextoEjecucion->registrosCPU, registro, string_duplicate(valor));
}

void io(char* tiempo){
    destruirTemporizador(rafagaCPU);
    modificarMotivoDesalojo (IO, 1, tiempo, "", "");
    enviarContextoActualizado(socketCliente);
}

void wait_c(char* recurso){
    destruirTemporizador(rafagaCPU);
    modificarMotivoDesalojo (WAIT, 1, recurso, "", "");
    enviarContextoActualizado(socketCliente);
}

void signal_c(char* recurso){
    destruirTemporizador(rafagaCPU);
    modificarMotivoDesalojo (SIGNAL, 1, recurso, "", "");
    enviarContextoActualizado(socketCliente);
}

void yield_c(){ 
    destruirTemporizador(rafagaCPU);
    modificarMotivoDesalojo (YIELD, 0, "", "", "");
    enviarContextoActualizado(socketCliente);
}

void exit_c () {
    destruirTemporizador(rafagaCPU);
    char * terminado = string_duplicate ("SUCCESS");
    modificarMotivoDesalojo (EXIT, 1, terminado, "", "");
    enviarContextoActualizado(socketCliente);
    free (terminado);
}

void f_open(char* nombre){
    destruirTemporizador(rafagaCPU);
    modificarMotivoDesalojo (F_OPEN, 1, nombre, "", "");
    enviarContextoActualizado(socketCliente);
};

void f_close(char* nombre){
    destruirTemporizador(rafagaCPU);
    modificarMotivoDesalojo (F_CLOSE, 1, nombre, "", "");
    enviarContextoActualizado(socketCliente);
};

void f_seek(char* nombre, char* puntero){
    destruirTemporizador(rafagaCPU);
    modificarMotivoDesalojo (F_SEEK, 2, nombre, puntero, "");
    enviarContextoActualizado(socketCliente);
};

void f_read(char* nombre, char* direccionLogica, char* cantBytes){
    int tamanioLeer = atoi(cantBytes);
    int dirFisica = mmu (direccionLogica, tamanioLeer);
    char* direccionFisica = string_itoa(dirFisica);
    destruirTemporizador(rafagaCPU);
    modificarMotivoDesalojo (F_READ, 3, nombre, direccionFisica, cantBytes);
    enviarContextoActualizado(socketCliente);
    free (direccionFisica);
};

void f_write(char* nombre, char* direccionLogica, char* cantBytes){
    int tamanioEscribir = atoi(cantBytes);
    int dirFisica = mmu (direccionLogica, tamanioEscribir);
    char* direccionFisica = string_itoa(dirFisica);
    destruirTemporizador(rafagaCPU);
    modificarMotivoDesalojo (F_WRITE, 3, nombre, direccionFisica, cantBytes);
    enviarContextoActualizado(socketCliente);
    free (direccionFisica);
};

void f_truncate(char* nombre, char* tamanio){
    destruirTemporizador(rafagaCPU);
    modificarMotivoDesalojo (F_TRUNCATE, 2, nombre, tamanio, "");
    enviarContextoActualizado(socketCliente);
};

void create_segment(char* idSegmento, char* tamanio){
    destruirTemporizador(rafagaCPU);
    modificarMotivoDesalojo (CREATE_SEGMENT, 2, idSegmento, tamanio, "");
    enviarContextoActualizado(socketCliente);
};

void delete_segment(char* idSegmento){
    destruirTemporizador(rafagaCPU);
    modificarMotivoDesalojo (DELETE_SEGMENT, 1, idSegmento, "", "");
    enviarContextoActualizado(socketCliente);
};

void mov_in(char* registro, char* direccionLogica){

    char* valorAInsertar;
    uint32_t tamRegistro = (uint32_t)obtenerTamanioReg(registro);
    uint32_t dirFisica = UINT32_MAX;
    dirFisica = mmu(direccionLogica, tamRegistro);

    if(dirFisica!=UINT32_MAX){
    t_paquete* peticion = crearPaquete();
    peticion->codigo_operacion = READ;
    agregarAPaquete(peticion,&contextoEjecucion->pid, sizeof(uint32_t));
    agregarAPaquete(peticion,&dirFisica, sizeof(uint32_t));
    agregarAPaquete(peticion,&tamRegistro,sizeof(uint32_t));
    enviarPaquete(peticion, conexionAMemoria);    
    eliminarPaquete (peticion);

    recibirOperacion(conexionAMemoria);
    valorAInsertar = recibirMensaje(conexionAMemoria);

    dictionary_remove_and_destroy(contextoEjecucion->registrosCPU, registro, free); 
    dictionary_put(contextoEjecucion->registrosCPU, registro, string_duplicate(valorAInsertar));
    
    log_info(logger, "PID: <%d> - Accion: <%s> - Segmento: <%d> - Direccion Fisica: <%d> - Valor: <%s>", contextoEjecucion->pid, "LEER", nroSegmento, dirFisica, valorAInsertar);
    free (valorAInsertar);
    }
};

void mov_out(char* direccionLogica, char* registro){

    void * valor = dictionary_get(contextoEjecucion->registrosCPU, registro);
    int tamRegistro = obtenerTamanioReg(registro);

    uint32_t dirFisica = UINT32_MAX;
    dirFisica = mmu(direccionLogica, tamRegistro);

    if(dirFisica != UINT32_MAX){    
    t_paquete* peticion = crearPaquete();
    peticion->codigo_operacion = WRITE;

    agregarAPaquete(peticion, &(contextoEjecucion->pid), sizeof(uint32_t));
    agregarAPaquete(peticion, &(dirFisica), sizeof(int));
    agregarAPaquete(peticion, valor, sizeof(char) * tamRegistro + 1); 

    enviarPaquete(peticion, conexionAMemoria);
    eliminarPaquete (peticion);

    recibirOperacion(conexionAMemoria);
    char * respuesta = recibirMensaje(conexionAMemoria);
    //debug ("%s", respuesta);
    free (respuesta);

    log_info(logger, "PID: <%d> - Accion: <%s> - Segmento: <%d> - Direccion Fisica: <%d> - Valor: <%s>", contextoEjecucion->pid, "WRITE", nroSegmento, dirFisica, (char *)valor);
    }
};

// Funciones grales

void destruirTemporizador (t_temporal * temporizador) {
    temporal_stop(temporizador);
    contextoEjecucion->rafagaCPUEjecutada = temporal_gettime(temporizador);  
    //temporal_destroy(temporizador); 
}

void modificarMotivoDesalojo (t_comando comando, int numParametros, char * parm1, char * parm2, char * parm3) {
    char * (parametros[3]) = { parm1, parm2, parm3 };
    contextoEjecucion->motivoDesalojo->comando = comando;
    for (int i = 0; i < numParametros; i++)
        contextoEjecucion->motivoDesalojo->parametros[i] = string_duplicate(parametros[i]);
    contextoEjecucion->motivoDesalojo->parametrosLength = numParametros;
}

void liberarMemoria() {
    for (int i = 0; i <= cantParametros; i++) free(elementosInstruccion[i]);
    free(elementosInstruccion);
}

char* recibirValor(int socket) {
    char* valor;
    int tamanio; 
	int size, desplazamiento = 0;
	void * buffer;

	buffer = recibirBuffer(socket, &size);

    memcpy(&(tamanio), buffer, sizeof(int32_t));
    desplazamiento += sizeof(int32_t);

    memcpy(&(valor),buffer+desplazamiento,sizeof(char)*tamanio); 
    desplazamiento+=sizeof(char)*tamanio; 

	free(buffer);

    return valor;
}

int obtenerTamanioReg(char* registro){

    if(string_starts_with(registro, "E")) return 8;
    else if(string_starts_with(registro, "R")) return 16;
    else return 4;

}

//mmu

uint32_t mmu(char* direccionLogica, int tamValor){
    int dirFisica;
    int dirLogica = atoi(direccionLogica);
    int tamMaxSegmento = obtenerTamanioMaxSeg();

    nroSegmento = floor(dirLogica/tamMaxSegmento);
    uint32_t desplazamiento = dirLogica % tamMaxSegmento;

    log_debug(logger, "nrosegmento: %d", nroSegmento);
    log_debug(logger, "desplazamiento: %d", desplazamiento);
    log_debug(logger, "tamvalor: %d", tamValor);

    t_segmento* segmento = (t_segmento*)list_get(contextoEjecucion->tablaDeSegmentos, nroSegmento);
    
    uint32_t base = segmento->direccionBase;
    
    if((desplazamiento + tamValor) < (segmento->tamanio)){
        dirFisica = base + desplazamiento;
        return dirFisica;
    }
    
    else{
        log_info(logger, "PID: <%d> - Error SEG_FAULT - Segmento: <%d> - Offset: <%d> - Tamaño: <%d>", contextoEjecucion->pid, nroSegmento, desplazamiento, tamValor);
        char * terminado = string_duplicate ("SEG_FAULT");
        destruirTemporizador(rafagaCPU);
        modificarMotivoDesalojo (EXIT, 1, terminado, "", "");
        enviarContextoActualizado(socketCliente);
        contextoEjecucion->programCounter = contextoEjecucion->instruccionesLength;
        free (terminado);
        return UINT32_MAX; 
    }
}
