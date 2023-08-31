#include <fileSystem/include/bloques.h>


uint32_t proximoBloqueLibre () {
    for (uint32_t i = 0; i < (uint32_t) cantBloques; i++) {
        bool noDisponible = bitarray_test_bit (bitmap, i);
        log_info (logger, "Acceso a Bitmap - Bloque: <%d> - Estado: <%d>", i, noDisponible);
        if (!noDisponible) {
            bitarray_set_bit (bitmap, i);
            msync (bitmap->bitarray, tamanioBitmap, MS_SYNC);
            return i;
        }
    }
    return UINT32_MAX;
}


void eliminarBloque (uint32_t ptr) {
    log_info (logger, "Acceso a Bitmap - Bloque: <%d> - Estado: <%d>", ptr, bitarray_test_bit (bitmap, ptr));
    bitarray_clean_bit (bitmap, ptr);
    for (int i = 0; i < tamanioBloques; i++)
        bloques[ptr][i] = '\0';
}

int copiarABloque (uint32_t numBloque, char * texto, uint32_t longDeTexto) {
    if (longDeTexto > (uint32_t) tamanioBloques)
        return -1;
    for (uint32_t i = 0; i < longDeTexto; i++) 
        bloques[numBloque][i] = texto[i];

    return 0;
}

uint32_t direccionIndirectaAReal (uint32_t ptrIndirecto, uint32_t bloqueABuscar) {
    uint8_t direccionDeconstruida[TAMANIO_PUNTERO];
    //debug ("Variables: %d %d %d %d %d", bloqueABuscar * TAMANIO_PUNTERO, bloques[ptrIndirecto][bloqueABuscar * TAMANIO_PUNTERO], bloques[ptrIndirecto][bloqueABuscar * TAMANIO_PUNTERO + 1], bloques[ptrIndirecto][bloqueABuscar * TAMANIO_PUNTERO + 2], bloques[ptrIndirecto][bloqueABuscar * TAMANIO_PUNTERO + 3]);
    for (int i = bloqueABuscar * TAMANIO_PUNTERO; i < (int) (bloqueABuscar + 1) * TAMANIO_PUNTERO; i++)
        direccionDeconstruida[i - (bloqueABuscar * TAMANIO_PUNTERO)] = bloques[ptrIndirecto][i];
    uint32_t direccion = 0;
    //debug ("Puntero indirecto: %d | Bloque del puntero %d", ptrIndirecto, bloqueABuscar);
    //debug ("Direccion deconstruida: %d %d %d %d", direccionDeconstruida[0], direccionDeconstruida[1], direccionDeconstruida[2], direccionDeconstruida[3]);
    for (int i = 1; i <= TAMANIO_PUNTERO; i++)
        direccion += (direccionDeconstruida[i - 1] << ((TAMANIO_PUNTERO - i) * 8));
    return direccion;
}