#include "memoria/include/peticiones.h"

//serializar tabla de Segmentos

void enviarTablaSegmentos(t_proceso* procesoEnMemoria){ 
    t_paquete* paquete = crearPaquete();
    
    paquete->codigo_operacion = TABLADESEGMENTOS;

    uint32_t tablaDeSegmentosSize = list_size(procesoEnMemoria->tablaDeSegmentosAsociada);

	agregarAPaquete(paquete,(void*)&procesoEnMemoria->pid,sizeof(uint32_t)); 
    agregarAPaquete(paquete,(void*)&tablaDeSegmentosSize,sizeof(uint32_t)); 
   
    uint32_t i;
    for(i=0;i<tablaDeSegmentosSize;i++){
        agregarSegmentoAPaquete(paquete,list_get(procesoEnMemoria->tablaDeSegmentosAsociada, i));
    }

    enviarPaquete(paquete, sockets[2]);

	eliminarPaquete(paquete);
}

void agregarSegmentoAPaquete(t_paquete* paquete, t_segmento* segmento){
	agregarAPaquete(paquete, (void*)&segmento->id, sizeof(uint32_t));
	agregarAPaquete(paquete, (void*)&segmento->direccionBase, sizeof(uint32_t));
	agregarAPaquete(paquete, (void*)&segmento->tamanio, sizeof(uint32_t));
}

uint32_t recibirPID(int socketCliente) {

	int size, desplazamiento=0; 
	uint32_t pid; 

	void* buffer = recibirBuffer(socketCliente, &size);
	desplazamiento += sizeof(int);
	memcpy(&(pid), buffer + desplazamiento, sizeof(uint32_t));

	free (buffer);
	return pid; 

}

t_peticion* recibirPeticionCreacionDeSegmento(int socketCliente) {

	int size, desplazamiento = 0; 

	t_peticion* peticion = malloc(sizeof(t_peticion)); 
	t_segmento* segmentoPedido = malloc(sizeof(t_segmento)); 

	peticion->segmento = segmentoPedido; 
	peticion->segmento->direccionBase = UINT32_MAX; 

	void* buffer = recibirBuffer(socketCliente, &size); 
	desplazamiento += sizeof(int); 
	memcpy(&(peticion->pid), buffer + desplazamiento, sizeof(uint32_t));
	peticion->segmento->pid = peticion->pid;
	desplazamiento += sizeof(uint32_t) + sizeof (int); 

	memcpy(&(peticion->segmento->id),buffer+desplazamiento,sizeof(uint32_t)); 
	desplazamiento += sizeof(uint32_t) + sizeof (int); 
	memcpy(&(peticion->segmento->tamanio), buffer+desplazamiento,sizeof(uint32_t)); 

	free (buffer);
	debug ("Recibida peticion de PID %d, para la id %d y de tamaño %d", peticion->pid, peticion->segmento->id, peticion->segmento->tamanio);
	return peticion; 

}

void recibirYProcesarPeticionEliminacionSegmento(int socketCliente) {

	int size, desplazamiento = 0; 
	uint32_t pid; 
	uint32_t segmentId; 

	void* buffer = recibirBuffer(socketCliente, &size); 

	desplazamiento += sizeof(int); 
	memcpy(&pid, buffer + desplazamiento, sizeof(uint32_t));

	desplazamiento += sizeof(uint32_t) + sizeof (int); 
	memcpy(&segmentId,buffer+desplazamiento,sizeof(uint32_t)); 

	free (buffer);
	deleteSegment(pid, segmentId); 
}