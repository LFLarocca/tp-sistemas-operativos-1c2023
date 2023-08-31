#ifndef CICLODEINSTRUCCION_H
#define CICLODEINSTRUCCION_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <stdint.h>
    #include <unistd.h>
    #include <commons/string.h>
    #include <commons/config.h>
    #include <commons/log.h>
    #include <commons/temporal.h>
    #include <commons/collections/list.h>
    #include "shared/include/global.h"
    #include "shared/include/configuraciones.h"
    #include "shared/include/utilsCliente.h"
    #include "shared/include/utilsServidor.h"
    #include "shared/include/contextoEjecucion.h"
    #include "conexionMemoria.h"

    #define obtenerTiempoEspera() config_get_int_value(config, "RETARDO_INSTRUCCION")
    #define obtenerTamanioMaxSeg() config_get_int_value(config, "TAM_MAX_SEGMENTO")

    extern char *listaComandos[];
    extern char* instruccionAEjecutar; 
    extern char** elementosInstruccion; 
    extern int instruccionActual; 
    extern int cantParametros;
    extern int tiempoEspera;
    extern int conexionAMemoria;

    extern t_contexto* contextoEjecucion;

    void cicloDeInstruccion();
    void fetch();
    void decode();
    void execute();
    void liberarMemoria();
    int buscar(char *elemento, char **lista); 

    void set_c(char*, char*);
    void yield_c(); 
    void exit_c(); 
    void mov_in(char*, char*);
    void mov_out(char*, char*);
    void io(char*);
    void f_open(char*);
    void f_close(char*);
    void f_seek(char*, char*);
    void f_read(char*, char*, char*);
    void f_write(char*, char*, char*);
    void f_truncate(char*, char* );
    void wait_c(char*);
    void signal_c(char* );
    void create_segment(char*, char*);
    void delete_segment(char*);

    uint32_t mmu(char* direccionLogica, int tamValor);
    char* recibirValor(int);
    void destruirTemporizador (t_temporal * temporizador);
    void modificarMotivoDesalojo (t_comando comando, int numParametros, char * parm1, char * parm2, char * parm3);
    int obtenerTamanioReg(char* registro);

#endif 