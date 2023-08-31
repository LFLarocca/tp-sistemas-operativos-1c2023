#include "memoria/include/manejoSegmentacion.h"

t_segmento * segmento0; 
void* espacioDeUsuario; 
t_list* huecosLibres; 
op_code resultado; 

// Creación estructuras
void creacionEspacio(){
    espacioDeUsuario = malloc (config_get_int_value(config,"TAM_MEMORIA")); 
	if (espacioDeUsuario == NULL) error ("No se pudo alocar memoria al espacio de usuario.");
	atexit (liberarEspacioDeUsuario);
	segmento0 = crearSegmentoCompartido (); 
	atexit (liberarSegmentoCompartido);
}

void creacionListaHuecosLibres() {

    huecosLibres = list_create(); 
	atexit (liberarListaDeHuecos);
    
    t_hueco_libre* primerHuecoLibre = malloc(sizeof(t_hueco_libre)); 

    primerHuecoLibre->direccionBase = config_get_int_value(config,"TAM_SEGMENTO_0"); 
    primerHuecoLibre->tamanioHueco = config_get_int_value(config,"TAM_MEMORIA") - primerHuecoLibre->direccionBase; 

	list_add(huecosLibres,(void*)primerHuecoLibre); 
}

t_segmento* crearSegmentoCompartido(){

	segmento0 = malloc(sizeof(t_segmento)); 
	if (segmento0 == NULL) error ("No se pudo alocar memoria al segmento 0.");
    segmento0->id=0; 
    segmento0->pid=UINT32_MAX; 
    segmento0->direccionBase=0; 
    segmento0->tamanio = config_get_int_value(config,"TAM_SEGMENTO_0"); 

	return segmento0; 
}



void liberarEspacioDeUsuario() {
	free (espacioDeUsuario);
}

void liberarSegmentoCompartido() {
	free (segmento0);
}

void liberarListaDeHuecos () {
	list_destroy_and_destroy_elements (huecosLibres, free);
}