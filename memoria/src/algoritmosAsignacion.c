#include "memoria/include/algoritmosAsignacion.h"

t_list* tablaDeTablasDeSegmentos; 

// Esquema de memoria

op_code ubicarSegmentosEnEspaciosLibres(t_peticion* peticion){
    
    char* algoritmoAUtilizar = config_get_string_value(config, "ALGORITMO_ASIGNACION");
    op_code resultado; 
	if (!strcmp(algoritmoAUtilizar, "FIRST"))
    {
        resultado = ubicarSegmentosPorFirst(peticion);
        return resultado; 
    }
    else if (!strcmp(algoritmoAUtilizar, "BEST"))
    {
        resultado = ubicarSegmentosPorBest(peticion);
        return resultado; 
        
    }
    else if (!strcmp(algoritmoAUtilizar, "WORST"))
    {
        resultado = ubicarSegmentosPorWorst(peticion); 
        return resultado; 
        
    }

    error("ERROR EN ALGORITMO DE ASIGNACION");
    return 0;
}

//Implementacion First
op_code ubicarSegmentosPorFirst(t_peticion* peticion){
	
    uint32_t tamanioSegmento = peticion->segmento->tamanio; 

    t_hueco_libre* huecoLibre; 

    for (int i=0;i<list_size(huecosLibres);i++) {
        
        huecoLibre = ((t_hueco_libre*)list_get(huecosLibres,i)); 
      
        if(huecoLibre->tamanioHueco >= tamanioSegmento) {
			
            peticion->segmento->direccionBase = huecoLibre->direccionBase;

            //debug ("Se ha encontrado un espacio para el segmento");
            loggearCreacionDeSegmento(peticion); 
            agregarSegmentoATablaDeSegmentosPCB(peticion); 
			reducirHuecosLibres(peticion->segmento, i);
            return SUCCESS; 
        }
    }

    
    if (corroborarPosibilidadDeCompactacion(peticion)){
        free(peticion->segmento);
        return COMPACTACION;
    }

    free(peticion->segmento);
    return OUTOFMEMORY; 
}


void loggearCreacionDeSegmento(t_peticion* peticion) {
    log_info(logger,"PID: <%d> - Crear Segmento: <%d> - Base: <%d> - Tamaño: <%d>",peticion->pid,peticion->segmento->id,peticion->segmento->direccionBase,peticion->segmento->tamanio); 
}



void agregarSegmentoATablaDeSegmentosPCB(t_peticion* peticion){
	
    uint32_t pidProceso = peticion -> pid;
	t_segmento* segmentoAAgregar = peticion->segmento;
	t_proceso* proceso = buscarProcesoSegun(pidProceso);
    debug ("%d, %d, %d, %d", peticion->segmento->pid, peticion->segmento->id, peticion->segmento->direccionBase, peticion->segmento->tamanio);
    list_replace_and_destroy_element(proceso->tablaDeSegmentosAsociada,peticion->segmento->id,(void*)segmentoAAgregar,free); 
}

void reducirHuecosLibres(t_segmento* segmento, int indiceHueco) {
    t_hueco_libre* aModificar = list_get(huecosLibres,indiceHueco); 
    aModificar->tamanioHueco -= segmento->tamanio; 

    if(aModificar->tamanioHueco == 0) {
    	list_remove_and_destroy_element(huecosLibres,indiceHueco,free); 
    }
    else {
        aModificar->direccionBase = segmento->direccionBase + segmento->tamanio; 
    }
     
}

t_proceso *buscarProcesoSegun(uint32_t pid)
{
    //debug ("Tamaño de lista: %d", list_size (tablaDeTablasDeSegmentos));
	for (int i = 0; i < list_size(tablaDeTablasDeSegmentos); i++)
		if (((t_proceso *)list_get(tablaDeTablasDeSegmentos, i))->pid == pid)
			return (t_proceso *)list_get(tablaDeTablasDeSegmentos, i);
	return NULL;
}

//Implementacion Best
op_code ubicarSegmentosPorBest(t_peticion* peticion){
    uint32_t tamanioSegmento = peticion->segmento->tamanio; 
    t_hueco_libre* huecoLibre = NULL; 
    t_hueco_libre* huecoAAsignar = NULL;
    int indiceHueco;
    bool encontrado = false; 
    int32_t tamanioHuecoMenor = __INT32_MAX__;

    for (int i=0;i<list_size(huecosLibres);i++) {
        huecoLibre = ((t_hueco_libre*)list_get(huecosLibres,i));

        if((huecoLibre->tamanioHueco >= tamanioSegmento) && ((int32_t)huecoLibre->tamanioHueco < tamanioHuecoMenor)){
            huecoAAsignar = huecoLibre;
            tamanioHuecoMenor = huecoAAsignar->tamanioHueco;
            indiceHueco = i;
            encontrado=true;
        }
    }
    if (encontrado) {

        peticion->segmento->direccionBase = huecoAAsignar->direccionBase;
        //debug ("Se ha encontrado un espacio para el segmento");
        loggearCreacionDeSegmento(peticion); 
        agregarSegmentoATablaDeSegmentosPCB(peticion); 
	    reducirHuecosLibres(peticion->segmento, indiceHueco);
        return SUCCESS;

    }
   
    else {

        if (corroborarPosibilidadDeCompactacion(peticion)){
            free(peticion->segmento);
            return COMPACTACION; 
        }

        free(peticion->segmento);
        return OUTOFMEMORY;
    }
    
}

//Implementacion Worst
op_code ubicarSegmentosPorWorst(t_peticion* peticion){

    uint32_t tamanioSegmento = peticion->segmento->tamanio; 
    t_hueco_libre* huecoLibre; 
    t_hueco_libre* huecoAAsignar = NULL;
    int indiceHueco;
    uint32_t tamanioHuecoMayor = 0;

    for (int i=0;i<list_size(huecosLibres);i++) {
        huecoLibre = ((t_hueco_libre*)list_get(huecosLibres,i));

        debug ("Encontrado el hueco libre en la dirección %d y el tamaño %d.", huecoLibre->direccionBase, huecoLibre->tamanioHueco);

        if(huecoLibre->tamanioHueco > tamanioHuecoMayor){
            huecoAAsignar = huecoLibre;
            tamanioHuecoMayor = huecoAAsignar->tamanioHueco;
            indiceHueco = i;
        }
    }

    if(huecoAAsignar != NULL && huecoAAsignar->tamanioHueco >= tamanioSegmento) {
        peticion->segmento->direccionBase = huecoAAsignar->direccionBase;
        loggearCreacionDeSegmento(peticion); 

        agregarSegmentoATablaDeSegmentosPCB(peticion); 
		reducirHuecosLibres(peticion->segmento, indiceHueco);
        return SUCCESS; 
    }
    else
    {   
         
        if(corroborarPosibilidadDeCompactacion(peticion)){
            free(peticion->segmento); 
            return COMPACTACION;  
        }
        free(peticion->segmento);   
        return OUTOFMEMORY; 
    }
}


bool corroborarPosibilidadDeCompactacion(t_peticion* peticion) {

    uint32_t sumaTamanios = sumaTamaniosHuecosLibres(); 

    return (sumaTamanios >= peticion->segmento->tamanio);  

}

uint32_t sumaTamaniosHuecosLibres() {

    int i; 
    t_hueco_libre* temp; 
    uint32_t suma = 0; 
    for (i=0;i<list_size(huecosLibres);i++) {
        temp=(t_hueco_libre*)list_get(huecosLibres,i); 
        suma+=temp->tamanioHueco; 

    }

    return suma; 
}