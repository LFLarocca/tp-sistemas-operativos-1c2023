#include "kernel/include/planificacion/algoritmosCortoPlazo.h"

void detenerYDestruirCronometro(t_temporal *cronometroReady){

    temporal_stop(cronometroReady);
    temporal_destroy(cronometroReady);
}

void planificarACortoPlazoSegunAlgoritmo(){
    char *algoritmoPlanificador = obtenerAlgoritmoPlanificacion();

    if (!strcmp(algoritmoPlanificador, "FIFO"))
    {
        planificarACortoPlazo(proximoAEjecutarFIFO);
    }
    else if (!strcmp(algoritmoPlanificador, "HRRN"))
    {
        planificarACortoPlazo(proximoAEjecutarHRRN);
    }
    else
    {
        log_error(loggerError, "Algoritmo invalido. Debe ingresar FIFO o HRRN");
        abort();
    }
}

t_pcb *proximoAEjecutarFIFO(){
    return desencolar(pcbsREADY);
}

void *mayorRR(void *unPCB, void *otroPCB){
    return (calcularRR(unPCB) >= calcularRR(otroPCB)) ? unPCB : otroPCB;
}

void estimacionNuevaRafaga(t_pcb *pcb){

    if (!strcmp(obtenerAlgoritmoPlanificacion(), "HRRN"))
    {
        calcularEstimadoProximaRafaga(pcb, rafagaCPU);
    }
}

void calcularEstimadoProximaRafaga(t_pcb *pcb, int64_t rafagaReal){

    double alfa = obtenerAlfaEstimacion();
    double estimadoRafaga = alfa * rafagaReal + (1 - alfa) * pcb->estimadoProximaRafaga;
    pcb->estimadoProximaRafaga = estimadoRafaga;
}

double calcularRR(void *elem){

    t_pcb *pcb = (t_pcb *)elem;

    temporal_stop(pcb->tiempoEnReady);

    double waitTime = temporal_gettime(pcb->tiempoEnReady);

    temporal_resume(pcb->tiempoEnReady);

    double estimatedServiceTime = pcb->estimadoProximaRafaga;

    //debug ("%lf %lf", (waitTime + estimatedServiceTime) / estimatedServiceTime, estimatedServiceTime);

    return (waitTime + estimatedServiceTime) / estimatedServiceTime;
}

t_pcb *proximoAEjecutarHRRN(){
    t_pcb * pcbMaximo = list_get_maximum(pcbsREADY, mayorRR);
    for (int i = 0; i < list_size(pcbsREADY); i++)
        if (pcbMaximo->pid == ((t_pcb *)(list_get(pcbsREADY, i)))->pid) 
            return list_remove(pcbsREADY, i);
    return list_remove(pcbsREADY, 0);
}