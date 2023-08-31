#include "memoria/include/compactacion.h"

t_list* todosLosSegmentos; 

void compactar(){
    
    log_info(logger,"Solicitud de Compactación"); 
    usleep(config_get_int_value(config,"RETARDO_COMPACTACION") * 1000); 

    reubicacionDeSegmentos(); 
}

void reubicacionDeSegmentos() {

    todosLosSegmentos = list_create(); 

    unificarSegmentos(); 

    t_segmento* segmento; 

    uint32_t nuevaBase = segmento0->tamanio; 
    for (int i = 0; i < list_size(todosLosSegmentos); i++) {
        //Aca iria tambien la parte de escribir donde corresponda las cosas.
        segmento = list_get(todosLosSegmentos, i);
        //debug ("%d", segmento->pid);
        if (segmento->tamanio) {
            actualizarDirecciones(nuevaBase, segmento->direccionBase, segmento->tamanio);
            segmento->direccionBase = nuevaBase; 
            nuevaBase += segmento->tamanio;

	        log_info(logger, "PID: <%d> - Segmento: <%d> - Base: <%d> - Tamaño: <%d>", segmento->pid, segmento->id, segmento->direccionBase, segmento->tamanio);

            //actualizarSegmento(segmento->pid,segmento->id, segmento); 
        }
    }

    list_clean_and_destroy_elements(huecosLibres,free); 

    crearUnicoHuecoLibre(nuevaBase); 

    list_destroy(todosLosSegmentos); 
}

void crearUnicoHuecoLibre(uint32_t direccionFinal) {

    uint32_t tamanioTotalMemoria = config_get_int_value(config,"TAM_MEMORIA");
    
    uint32_t tamanioHuecoRestante = tamanioTotalMemoria - direccionFinal;

    t_hueco_libre* huecoLibre = malloc(sizeof(t_hueco_libre));

    huecoLibre->direccionBase = direccionFinal;
    huecoLibre->tamanioHueco = tamanioHuecoRestante;

    list_add_sorted(huecosLibres, huecoLibre, direccionMasBaja);
}
   

void unificarSegmentos() {
    
    t_proceso* temp1; 
    t_segmento* temp2; 

    for (int i=0;i<list_size(tablaDeTablasDeSegmentos);i++){

        temp1 = (t_proceso*)list_get(tablaDeTablasDeSegmentos,i); 

        for(int j=1; j<list_size(temp1->tablaDeSegmentosAsociada);j++) {
            
            temp2 = (t_segmento*)list_get(temp1->tablaDeSegmentosAsociada,j); 
            list_add_sorted(todosLosSegmentos, (void*)temp2, direccionBaseMenor); 
            
        }   
    }
}

void actualizarSegmento(uint32_t pid, uint32_t segmentID, t_segmento* segmentoActualizado) {

    debug ("%d %d", pid, segmentID);
    t_list* tablaDeSegmentos = (buscarProcesoSegun(pid))->tablaDeSegmentosAsociada;

    list_replace_and_destroy_element(tablaDeSegmentos,segmentID,(void*)segmentoActualizado,free); 
}

void actualizarDirecciones(uint32_t base, uint32_t baseAnterior, uint32_t tamanio){
    memcpy(espacioDeUsuario+base, espacioDeUsuario+baseAnterior, tamanio);
}

bool direccionBaseMenor (void * unSegmento, void * otroSegmento) {
    t_segmento *segmentoUno = (t_segmento *) unSegmento;
	t_segmento *segmentoDos = (t_segmento *) otroSegmento;
	return (segmentoUno->direccionBase < segmentoDos->direccionBase); 
}

bool ordenadoPorID (void * unSegmento, void * otroSegmento) {
    t_segmento *segmentoUno = (t_segmento *) unSegmento;
	t_segmento *segmentoDos = (t_segmento *) otroSegmento;
	return (segmentoUno->id < segmentoDos->id); 
}

bool direccionMasBaja (void * huecoLibreUno, void * huecoLibreDos) {
	t_hueco_libre * huecoUno = (t_hueco_libre * ) huecoLibreUno;
	t_hueco_libre * huecoDos = (t_hueco_libre * ) huecoLibreDos;
	return (huecoUno->direccionBase < huecoDos->direccionBase); 
}

void enviarTablasActualizadas() {

    t_proceso* proceso; 

    for(int i=0;i<list_size(tablaDeTablasDeSegmentos);i++) {

        proceso = buscarProcesoSegun(i); 
        enviarCodOp(i, sockets[2]);
        enviarTablaSegmentos(proceso); 
        
    }
}


