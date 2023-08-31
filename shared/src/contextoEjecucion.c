#include "shared/include/contextoEjecucion.h"
t_contexto * contextoEjecucion = NULL;

// MANEJO DE CONTEXTO
void enviarContextoActualizado(int socket){ 
    t_paquete * paquete = crearPaquete();
    
    paquete->codigo_operacion = CONTEXTOEJECUCION;
   
    agregarAPaquete (paquete,(void *)&contextoEjecucion->pid, sizeof(contextoEjecucion->pid));
    agregarAPaquete (paquete,(void *)&contextoEjecucion->programCounter, sizeof(contextoEjecucion->programCounter));
    
    //debug ("%d %d", contextoEjecucion->pid, contextoEjecucion->programCounter);

    agregarInstruccionesAPaquete (paquete, contextoEjecucion->instrucciones);
    agregarRegistrosAPaquete(paquete, contextoEjecucion->registrosCPU);
    
    agregarTablaDeSegmentosAPaquete(paquete);
    agregarMotivoAPaquete(paquete, contextoEjecucion->motivoDesalojo);
    agregarAPaquete(paquete, (void *)&contextoEjecucion->rafagaCPUEjecutada, sizeof(contextoEjecucion->rafagaCPUEjecutada));

    enviarPaquete(paquete, socket);
	eliminarPaquete(paquete);
}


void agregarTablaDeSegmentosAPaquete(t_paquete* paquete){
    
    agregarAPaquete (paquete, &(contextoEjecucion->tablaDeSegmentosSize), sizeof contextoEjecucion->tablaDeSegmentosSize);
    
    //t_segmento * test = (t_segmento *) list_get (contextoEjecucion->tablaDeSegmentos, 0);

    //debug ("Test 2: %d %d %d", test->direccionBase, test->id, test->tamanio);

    uint32_t i;
    for(i=0;i<contextoEjecucion->tablaDeSegmentosSize;i++){
        agregarSegmentoAPaquete(paquete,list_get(contextoEjecucion->tablaDeSegmentos, i));
    }
}

void agregarSegmentoAPaquete(t_paquete* paquete, t_segmento* segmento){
	agregarAPaquete(paquete, &(segmento->id), sizeof(uint32_t));
	agregarAPaquete(paquete, &(segmento->direccionBase), sizeof(uint32_t));
	agregarAPaquete(paquete, &(segmento->tamanio), sizeof(uint32_t));
}

void agregarMotivoAPaquete (t_paquete* paquete, t_motivoDeDesalojo* motivoDesalojo){

    agregarAPaquete (paquete, (void *)&motivoDesalojo->comando, sizeof (motivoDesalojo->comando));
    agregarAPaquete (paquete, (void *)&motivoDesalojo->parametrosLength, sizeof (motivoDesalojo->parametrosLength));
    for (int i = 0; i < motivoDesalojo->parametrosLength; i++) 
        agregarAPaquete (paquete, (void *)motivoDesalojo->parametros[i], (strlen (motivoDesalojo->parametros[i]) + 1) * sizeof(char));

}

void agregarInstruccionesAPaquete(t_paquete* paquete, t_list* instrucciones){

    contextoEjecucion->instruccionesLength = list_size(instrucciones);

    agregarAPaquete(paquete, &contextoEjecucion->instruccionesLength, sizeof(uint32_t)); //primero envio la cantidad de elementos
    uint32_t i;
    for(i=0;i<contextoEjecucion->instruccionesLength;i++)
        agregarAPaquete (paquete, list_get(instrucciones, i), sizeof(char) * (strlen(list_get(instrucciones, i)) + 1 ));
    
}

void agregarRegistrosAPaquete(t_paquete* paquete, t_dictionary* registrosCPU){

    char name[3] = "AX", longName[4] = "EAX";

    for (int i = 0; i < 3; i++) {
        ssize_t tamanioActual = sizeof(char) * (4 * pow(2, i) + 1);
        for (int j = 0; j < 4; j++) {
            char* registroConCaracterTerminador = (char*) dictionary_get(registrosCPU, (i) ? longName : name); 
            //string_append(&registroConCaracterTerminador,"\0"); 
            
            //debug ("Agregando a paquete: %s", registroConCaracterTerminador);
            agregarAPaquete(paquete, (void*) registroConCaracterTerminador, tamanioActual);
            name[0]++, longName[1]++;
        }
        longName [1] = 'A', longName [0] = (i == 1) ? 'R' : 'E';
    }

}



void recibirContextoActualizado (int socket) {

    if (contextoEjecucion != NULL) destroyContextoUnico ();
	iniciarContexto ();
	int size, desplazamiento = 0;
	void * buffer;

	buffer = recibirBuffer(socket, &size);

    // Desplazamiento: Tamaño de PID, PID, y tamaño de programCounter.
    desplazamiento += sizeof(int);
    memcpy(&(contextoEjecucion->pid), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(contextoEjecucion->pid) + sizeof(int);

    // Desplazamiento: programCounter y tamaño de instruccionesLength.
    memcpy(&(contextoEjecucion->programCounter), buffer+desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(contextoEjecucion->programCounter) + sizeof(int);

    //debug ("%d %d", contextoEjecucion->pid, contextoEjecucion->programCounter);

    deserializarInstrucciones (buffer, &desplazamiento);
    deserializarRegistros (buffer, &desplazamiento);

    deserializarTablaDeSegmentos(buffer, &desplazamiento);
    
    deserializarMotivoDesalojo (buffer, &desplazamiento);

    // Desplazamiento: Tamaño de la rafaga de CPU Ejecutada.
    desplazamiento += sizeof (int);
    memcpy(&(contextoEjecucion->rafagaCPUEjecutada), buffer + desplazamiento, sizeof (uint64_t));
		
	free(buffer);

}

void deserializarInstrucciones (void * buffer, int * desplazamiento) {

    int tamanio;
    list_clean_and_destroy_elements (contextoEjecucion->instrucciones, free);
    // Desplazamiento: instruccionesLength.
    memcpy(&(contextoEjecucion->instruccionesLength), buffer + (* desplazamiento), sizeof(uint32_t));
    (* desplazamiento) += sizeof(uint32_t);
    
    for (uint32_t i = 0; i < contextoEjecucion->instruccionesLength; i++) {

        // Desplazamiento: Tamaño de la linea de instruccion.
        memcpy (&tamanio, buffer + (* desplazamiento), sizeof (int));
        (* desplazamiento) += sizeof (int);
        char * valor = malloc (tamanio);

        //Desplazamiento: Linea de instruccion.
        memcpy(valor, buffer + (* desplazamiento), tamanio);
        //debug ("%s", valor);
        (* desplazamiento) += tamanio;
        list_add (contextoEjecucion->instrucciones, string_duplicate (valor));
        free (valor);
    }

    (* desplazamiento) += sizeof(int);

}

void deserializarRegistros (void * buffer, int * desplazamiento) {
    dictionary_clean_and_destroy_elements (contextoEjecucion->registrosCPU, free);

    char *temp, name [3] = "AX", longName [4] = "EAX";

    for (int i = 0; i < 3; i++) {
        ssize_t tamanioActual = sizeof(char) * (4 * pow(2, i) + 1);
        for (int j = 0; j < 4; j++) {
            temp = malloc (tamanioActual);

            // Desplazamiento: Registro actual y tamaño del proximo registro. 
            // (Para el ultimo registro pasa a ser el tamaño del comando de desalojo)
            memcpy (temp, buffer + (* desplazamiento), tamanioActual);
            (* desplazamiento) += tamanioActual + sizeof (int);
            //debug ("%s", temp);

            //char * auxiliar = 
                //string_substring(temp, 0, tamanioActual - 1); 
            dictionary_put (contextoEjecucion->registrosCPU, (i) ? longName : name, temp);
            //free (temp);
            name [0]++, longName [1]++;
        }
        longName [1] = 'A', longName [0] = (i == 1) ? 'R' : 'E';
    }
}

void deserializarTablaDeSegmentos (void * buffer, int * desplazamiento) {
    list_clean_and_destroy_elements (contextoEjecucion->tablaDeSegmentos, free);
    // Desplazamiento: tamaño de la lista de segmentos.
    memcpy(&(contextoEjecucion->tablaDeSegmentosSize), buffer + (* desplazamiento), sizeof(uint32_t));
    (* desplazamiento) += sizeof(uint32_t);
    
    for (uint32_t i = 0; i < contextoEjecucion->tablaDeSegmentosSize; i++) {

        t_segmento* segmento = deserializarSegmento(buffer, desplazamiento);
        list_add (contextoEjecucion->tablaDeSegmentos, segmento);
    }

    //debug ("Me llegaron %d segmentos.", contextoEjecucion->tablaDeSegmentosSize);
    (* desplazamiento) += sizeof(int);

}


t_segmento* deserializarSegmento(void* buffer, int* desplazamiento){
    t_segmento* segmento = malloc(sizeof(t_segmento));
    int tamanio;
    // id
    memcpy (&tamanio, buffer + (* desplazamiento), sizeof (int));
    (* desplazamiento) += sizeof (int);
    memcpy (&(segmento->id), buffer + (* desplazamiento), tamanio);
    (* desplazamiento) += sizeof (uint32_t);
    //direccion base
    memcpy (&tamanio, buffer + (* desplazamiento), sizeof (int));
    (* desplazamiento) += sizeof (int);
    memcpy (&(segmento->direccionBase), buffer + (* desplazamiento), tamanio);
    (* desplazamiento) += sizeof (uint32_t);
    //tamanio
    memcpy (&tamanio, buffer + (* desplazamiento), sizeof (int));
    (* desplazamiento) += sizeof (int);
    memcpy (&(segmento->tamanio), buffer + (* desplazamiento), tamanio);
    (* desplazamiento) += sizeof (uint32_t);

    //debug ("Segmento: %d, %d, %d", segmento->id, segmento->direccionBase, segmento->tamanio);

    return segmento;
}


void deserializarMotivoDesalojo (void * buffer, int * desplazamiento) {

    // Desplazamiento: Comando de desalojo y tamaño de parametrosLength.
    memcpy (&(contextoEjecucion->motivoDesalojo->comando), buffer + (* desplazamiento), sizeof (t_comando));
    (* desplazamiento) += sizeof (t_comando) + sizeof (int);

    // Desplazamiento: parametrosLength.
    memcpy (&(contextoEjecucion->motivoDesalojo->parametrosLength), buffer + (* desplazamiento), sizeof (uint32_t));
    (* desplazamiento) += sizeof (contextoEjecucion->motivoDesalojo->parametrosLength);

    //debug ("Comando: %d, parametros: %d", contextoEjecucion->motivoDesalojo->comando, contextoEjecucion->motivoDesalojo->parametrosLength);

    for (int i = 0; i < contextoEjecucion->motivoDesalojo->parametrosLength; i++) {
        int tamanioParametro;

        // Desplazamiento: Tamaño de Parametro.
        memcpy (&tamanioParametro, buffer + (* desplazamiento), sizeof (int));
        (* desplazamiento) += sizeof(int);

        // Desplazamiento: Parametro.
        char *temp = malloc (sizeof(char) * tamanioParametro);
        memcpy(temp, buffer + (* desplazamiento), sizeof (char) * tamanioParametro);
        contextoEjecucion->motivoDesalojo->parametros[i] = temp;
        (* desplazamiento) += tamanioParametro;
    }

}

void iniciarContexto(){

    contextoEjecucion = malloc(sizeof(t_contexto));
	contextoEjecucion->instrucciones = list_create();
	contextoEjecucion->instruccionesLength = 0;
	contextoEjecucion->pid = 0;
	contextoEjecucion->programCounter = 0;
	contextoEjecucion->registrosCPU = dictionary_create();
	contextoEjecucion->tablaDeSegmentos = list_create();
	contextoEjecucion->tablaDeSegmentosSize = 0;
    contextoEjecucion->rafagaCPUEjecutada = 0;
    contextoEjecucion->motivoDesalojo = (t_motivoDeDesalojo *)malloc(sizeof(t_motivoDeDesalojo));
    contextoEjecucion->motivoDesalojo->parametros[0] = "";
    contextoEjecucion->motivoDesalojo->parametros[1] = "";
    contextoEjecucion->motivoDesalojo->parametros[2] = "";
    contextoEjecucion->motivoDesalojo->parametrosLength = 0;
    contextoEjecucion->motivoDesalojo->comando = 0;
	
}

void destroyContexto() {
    list_destroy_and_destroy_elements(contextoEjecucion->instrucciones, free);
    dictionary_destroy_and_destroy_elements(contextoEjecucion->registrosCPU, free);
    list_destroy_and_destroy_elements(contextoEjecucion->tablaDeSegmentos, free);
    for (int i = 0; i < contextoEjecucion->motivoDesalojo->parametrosLength; i++) 
        if (strcmp(contextoEjecucion->motivoDesalojo->parametros[i], "")) free(contextoEjecucion->motivoDesalojo->parametros[i]);
    free(contextoEjecucion->motivoDesalojo);
    free(contextoEjecucion);
    contextoEjecucion = NULL;
}

void destroyContextoUnico () {
    list_destroy(contextoEjecucion->instrucciones);
    dictionary_destroy_and_destroy_elements(contextoEjecucion->registrosCPU, free);
    list_destroy(contextoEjecucion->tablaDeSegmentos);
    for (int i = 0; i < contextoEjecucion->motivoDesalojo->parametrosLength; i++) 
        if (strcmp(contextoEjecucion->motivoDesalojo->parametros[i], "")) free(contextoEjecucion->motivoDesalojo->parametros[i]);
    free(contextoEjecucion->motivoDesalojo);
    free(contextoEjecucion);
    contextoEjecucion = NULL;
}