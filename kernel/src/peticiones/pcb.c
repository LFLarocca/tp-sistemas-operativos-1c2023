#include "kernel/include/peticiones/pcb.h"

char* pidsInvolucrados; 

//Básicos PCB

t_pcb *crearPCB(){

    t_pcb *nuevoPCB = malloc(sizeof(t_pcb));
    nuevoPCB->estado = NEW;
    nuevoPCB->pid = procesosCreados;
    nuevoPCB->programCounter = 0;
    nuevoPCB->instrucciones = list_create();
    nuevoPCB->estimadoProximaRafaga = obtenerEstimacionInicial();
    nuevoPCB->tablaDeArchivos = list_create();
    nuevoPCB->tablaDeSegmentos = list_create();
    nuevoPCB->recursosAsignados = list_create();
    nuevoPCB->registrosCPU = crearDiccionarioDeRegistros();

    procesosCreados++; // para el nuevo pid
    

    return nuevoPCB;
}

void destruirPCB(t_pcb *pcb){
    list_destroy_and_destroy_elements(pcb->instrucciones, free);
    list_destroy_and_destroy_elements(pcb->tablaDeSegmentos, free);
    list_destroy_and_destroy_elements(pcb->tablaDeArchivos, free);
    list_destroy_and_destroy_elements(pcb->recursosAsignados, free);
    dictionary_destroy_and_destroy_elements(pcb->registrosCPU, free);
    free(pcb);
}

t_dictionary *crearDiccionarioDeRegistros(){

    t_dictionary *registros = dictionary_create();

    char name[3] = "AX", longName[4] = "EAX";
    for (int i = 0; i < 4; i++) {
        dictionary_put(registros, name, string_repeat('0', 4));
        longName[0] = 'E';
        dictionary_put(registros, longName, string_repeat('0', 8));
        longName[0] = 'R';
        dictionary_put(registros, longName, string_repeat('0', 16));
        name[0]++, longName[1]++;
    }

    return registros;
}


// Lista de PCBs

void inicializarListasPCBs(){
    pcbsNEW = list_create();
    pcbsREADY = list_create();
    pcbsEnMemoria = list_create();
}

void destruirListasPCBs () {
    destruirListaPCB(pcbsNEW);
    destruirListaPCB(pcbsREADY);
    destruirListaPCB(pcbsEnMemoria);
}

void destruirListaPCB(t_list *pcbs){
    list_destroy_and_destroy_elements(pcbs, (void *)destruirPCB);
}

void encolar(t_list *pcbs, t_pcb *pcb){
    list_add(pcbs, (void *)pcb);
}

t_pcb *desencolar(t_list *pcbs){
    return (t_pcb *)list_remove(pcbs, 0);
}

void agregarPID(void *value){
    t_pcb *pcb = (t_pcb *)value;
    char *pid = string_itoa(pcb->pid);
    string_append_with_format(&pidsInvolucrados, " %s ", pid);
    free (pid);
}

void listarPIDS(t_list *pcbs) {
    list_iterate(pcbs, agregarPID);
}

t_pcb* buscarPID(t_list* listaPCBs, uint32_t pid){
    int cantProcesos = list_size(listaPCBs); 

    t_pcb* pcb;
    for(int i=0;i<cantProcesos;i++){
        
        pcb = list_get(listaPCBs, i);
        if(pcb->pid == pid) return pcb;
    }

    return NULL;
}









