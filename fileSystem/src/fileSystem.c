#include "fileSystem/include/fileSystem.h"

int socketCliente, socketMemoria, fdBitmap, fdBloques;
int cantBloques, tamanioBitmap, tamanioBloques;
t_log * logger, * loggerError; 
t_config * config, * superbloque;
t_bitarray * bitmap;
char * ptrBloques, * ptrBitMap, ** bloques;
char * pathSuperBloque, * pathBloques, * pathBitmap, * pathFCBs;

int main () {
    logger = iniciarLogger("fileSys.log","File System");
    config = iniciarConfiguracion("filesys.config");
    loggerError = iniciarLogger("errores.log", "File System"); 
    
    atexit(terminarPrograma);

    conexionMemoria();
    atexit(cerrarConexion);

    // Se abre el archivo de super-bloque y se agarra la cantidad de bloques y el tamaño de cada bloque.

    pathSuperBloque = confGet("PATH_SUPERBLOQUE");
    pathBitmap = confGet("PATH_BITMAP");
    pathBloques = confGet("PATH_BLOQUES");
    pathFCBs = confGet("PATH_FCB");

    superbloque = config_create(pathSuperBloque);
    cantBloques = config_get_int_value(superbloque, "BLOCK_COUNT");
    tamanioBitmap = BIT_CHAR(cantBloques);
    tamanioBloques = config_get_int_value(superbloque, "BLOCK_SIZE");

    atexit(cerrarSuperBloque);

    // Se abre el archivo del bitmap de bloques, con las flags para crearla si no existe y escribir y leer en caso de ser necesario.
    // A su vez se crea con permisos para que el usuario pueda leerlas y modificarlas por si las dudas.

    fdBitmap = open(pathBitmap, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fdBitmap < 0)
        error("No se abrio correctamente el archivo %s; error: %s", pathBitmap, strerror(errno));
    atexit(cerrarArchivoBitmap);
    
    if (ftruncate(fdBitmap, tamanioBitmap) < 0)
        error("No se pudo expandir correctamente el archivo %s; error: %s", pathBitmap, strerror(errno));

    // mmap es un asco, es lo peor de la galaxia.
    // Se genera un mmap, vinculado directamente al archivo de bitmap, para poder modificarlo directamente.

    // Explicación tecnica: AAAAAAAAAAAAAAAAAAAAAAA
    // Mentira, el mmap, por lo que entiendo, es una función que aloca una cantidad de memoria, según el segundo parametro, y la puede vincular
    // a un archivo, en este caso el del bitmap, para poder modificarlo activamente en memoria.
    // Para hacerlo funcionar creo un bitarray que es una forma de manejar una cantidad de bits especificos, y lo vinculo con el string que 
    // maneja el archivo de bitmap.

    ptrBitMap = mmap(0, tamanioBitmap, PROT_WRITE | PROT_READ, MAP_SHARED, fdBitmap, 0);
    if (ptrBitMap == MAP_FAILED) 
        error("No se mapeo correctamente el bitmap; error: %s", strerror(errno));
    atexit(cerrarMMapBitmap);
    bitmap = bitarray_create_with_mode(ptrBitMap, cantBloques, MSB_FIRST);
    atexit(cerrarBitmap);

    fdBloques = open(pathBloques, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fdBloques < 0)
        error("No se abrio correctamente el archivo %s; error: %s", pathBloques, strerror(errno));
    atexit(cerrarArchivoBloques);

    if (ftruncate(fdBloques, cantBloques * tamanioBloques) < 0)
        error("No se pudo expandir correctamente el archivo %s; error: %s", pathBloques, strerror(errno));

    ptrBloques = mmap(0, tamanioBloques * cantBloques, PROT_WRITE | PROT_READ, MAP_SHARED, fdBloques, 0);
    if (ptrBloques == MAP_FAILED) 
        error("No se mapeo correctamente el bitmap; error: %s", strerror(errno));
    atexit(cerrarMMapBloques);
    
    bloques = malloc(cantBloques * sizeof (char *));
    atexit (limpiarBloques);

    for (int i = 0; i < cantBloques; i++) {
        *(bloques + i) = &ptrBloques[i * tamanioBloques];
    }

    if (mkdir (pathFCBs, S_IRUSR | S_IWUSR | S_IXUSR) == -1 && errno != EEXIST)
        error ("No se pudo crear o verificar que exista el directorio de FCBs, error: %s", strerror (errno));
    
    // Pruebas de Crear, abrir, y truncar archivo.
    /*
    int retCode = 0;
    //if (access("directorioFCB/prueba.fcb", F_OK)) 
        retCode = crearArchivo ("prueba");
    //if (retCode < 0) error ("Se creo archivo incorrectamente, codigo de error: %d, error: %s", retCode, strerror(errno));
    fcb_t * pruebaFCB = abrirArchivo ("prueba");
    if (pruebaFCB == NULL) error ("No se pudo abrir el archivo %s.", "prueba");
    //debug ("Test: %d %d %d", bloques[pruebaFCB->ptrIndirecto][3], bloques[pruebaFCB->ptrIndirecto][7], bloques[pruebaFCB->ptrIndirecto][11]);
    //debug ("Test: %d %d %d", direccionIndirectaAReal (pruebaFCB->ptrIndirecto, 0), direccionIndirectaAReal (pruebaFCB->ptrIndirecto, 1), direccionIndirectaAReal (pruebaFCB->ptrIndirecto, 2));
    
    //retCode = copiarABloque (0, string_repeat ('t', 64), 64);
    //retCode = copiarABloque (2, string_repeat ('e', 64), 64);
    //retCode = copiarABloque (3, string_repeat ('s', 64), 64);
    //retCode = copiarABloque (4, string_repeat ('t', 64), 64);
    //if (retCode < 0) error ("No se pudo copiar correctamente la informacion a los bloques");
    
    retCode = truncarArchivo (pruebaFCB, tamanioBloques * 8);
    if (retCode < 0) error ("No se pudo truncar correctamente el archivo %s, codigo %d.", pruebaFCB->nombre, retCode);
    
    escribirArchivo (pruebaFCB, string_repeat ('t', 64), 64, 0);
    escribirArchivo (pruebaFCB, string_repeat ('e', 64), 64, 64);
    escribirArchivo (pruebaFCB, string_repeat ('s', 64), 64, 128);
    escribirArchivo (pruebaFCB, string_repeat ('t', 64), 64, 192);
    leerArchivo (pruebaFCB, 0, tamanioBloques);
    leerArchivo (pruebaFCB, 10, 10);
    leerArchivo (pruebaFCB, tamanioBloques, 10);
    leerArchivo (pruebaFCB, 0, tamanioBloques * 4);
    leerArchivo (pruebaFCB, 5, tamanioBloques * 3);

    //retCode = truncarArchivo (pruebaFCB, 0);
    //if (retCode < 0) error ("No se pudo truncar correctamente el archivo %s, codigo %d.", pruebaFCB->nombre, retCode);
    
    //retCode = truncarArchivo (pruebaFCB, (MAX_BLOQUES) * tamanioBloques);
    //if (retCode < 0) error ("No se pudo truncar correctamente el archivo %s, codigo %d.", pruebaFCB->nombre, retCode);
    
    //crearArchivo ("prueba2");
    //fcb_t * prueba2 = abrirArchivo ("prueba2");

    //truncarArchivo (prueba2, 16 * tamanioBloques);
    //truncarArchivo (prueba2, 8 * tamanioBloques);
    msync(ptrBloques, tamanioBloques * cantBloques, MS_SYNC);
    msync(ptrBitMap, tamanioBitmap, MS_SYNC);
    */
    

    escucharAlKernel();
    exit(0);
}

void cerrarConexion () { close (socketMemoria); }
void cerrarSuperBloque () { config_destroy (superbloque); }
void cerrarBitmap () { bitarray_destroy(bitmap); }
void cerrarMMapBitmap () { munmap(ptrBitMap, tamanioBitmap); }
void cerrarArchivoBitmap () { close (fdBitmap); }
void limpiarBloques () { free (bloques); }
void cerrarMMapBloques () { munmap(ptrBloques, tamanioBloques); }
void cerrarArchivoBloques () { close (fdBloques); }