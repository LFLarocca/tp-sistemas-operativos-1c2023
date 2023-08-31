UNAME:=$(shell uname)

ifeq ($(UNAME),Linux)
	FILE_EXT:=.out
else
	FILE_EXT:=.exe
endif

SHARED         := shared/src
CONFIG         := $(SHARED)/configuraciones.c
CONTEXTO_EJECUCION := $(SHARED)/contextoEjecucion.c
UTILS_CLIENTE  := $(SHARED)/utilsCliente.c
UTILS_SERVIDOR := $(SHARED)/utilsServidor.c


ARCHIVOS_CONSOLA := consola/src/consola.c \
                    $(CONFIG) $(UTILS_CLIENTE) $(UTILS_SERVIDOR) 

CPU_SRC := cpu/src/
ARCHIVOS_CPU     := $(CPU_SRC)cpu.c	\
					$(CPU_SRC)servidorKernel.c $(CPU_SRC)cicloDeInstruccion.c $(CPU_SRC)conexionMemoria.c \
				    $(CONFIG) $(CONTEXTO_EJECUCION) $(UTILS_CLIENTE) $(UTILS_SERVIDOR) 

FILESYS_SRC := fileSystem/src/
ARCHIVOS_FILESYS := $(FILESYS_SRC)fileSystem.c $(FILESYS_SRC)servidorKernel.c $(FILESYS_SRC)conexionMemoria.c \
					$(FILESYS_SRC)operaciones.c $(FILESYS_SRC)bloques.c \
				    $(CONFIG) $(UTILS_CLIENTE) $(UTILS_SERVIDOR) 

KERNEL_CONEXIONES := kernel/src/conexiones
KERNEL_PETICIONES := kernel/src/peticiones
KERNEL_PLANIFIACION := kernel/src/planificacion
ARCHIVOS_KERNEL  := kernel/src/kernel.c \
					$(KERNEL_CONEXIONES)/*.c \
					$(KERNEL_PETICIONES)/manejoRecursos.c $(KERNEL_PETICIONES)/manejoSegmentos.c $(KERNEL_PETICIONES)/manejoArchivos.c $(KERNEL_PETICIONES)/pcb.c $(KERNEL_PETICIONES)/syscalls.c \
					$(KERNEL_PLANIFIACION)/*.c \
                    $(CONFIG) $(UTILS_CLIENTE) $(UTILS_SERVIDOR) $(CONTEXTO_EJECUCION)

ARCHIVOS_MEMORIA := memoria/src/memoria.c memoria/src/conexionCPU.c memoria/src/conexionFS.c memoria/src/conexionKernel.c memoria/src/manejoSegmentacion.c memoria/src/algoritmosAsignacion.c memoria/src/peticiones.c memoria/src/compactacion.c\
                    $(CONFIG) $(UTILS_CLIENTE)  $(UTILS_SERVIDOR) 

CC := gcc
CFLAGS := -ggdb -I ./ -lcommons -Wall -Wextra

trap:
	@echo "Si estas aca te confundiste de ondis, pone \'make Modulo\' con el modulo que queres compilar o pone make en tu carpeta."

all: Consola Cpu Filesys Kernel Memoria
	@echo "Estan locas ratas cocain*manas, exitos."

Consola:
	@echo "Compilando consola... "
	@$(CC) $(ARCHIVOS_CONSOLA) -o consola/consola$(FILE_EXT)     $(CFLAGS)

Cpu:
	@echo "Compilando CPU... "
	@$(CC) $(ARCHIVOS_CPU)     -o cpu/cpu$(FILE_EXT)             $(CFLAGS) -lm

Filesys:
	@echo "Compilando File System... "
	@$(CC) $(ARCHIVOS_FILESYS) -o fileSystem/filesys$(FILE_EXT) $(CFLAGS)

Kernel:
	@echo "Compilando Kernel... "
	@$(CC) $(ARCHIVOS_KERNEL)  -o kernel/kernel$(FILE_EXT)       $(CFLAGS) -lpthread -lm

Memoria:
	@echo "Compilando Memoria... "
	@$(CC) $(ARCHIVOS_MEMORIA) -o memoria/memoria$(FILE_EXT)     $(CFLAGS) -lpthread
