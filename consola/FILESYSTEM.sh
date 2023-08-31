#!/bin/bash

./consola.out ./tuki-pruebas/FS_1 ./consola.config &
sleep 0.2; ./consola.out ./tuki-pruebas/FS_2 ./consola.config;
echo "Se ejecuto la prueba FILESYSTEM donde se validan las operaciones de File System y Memoria, y ademas se comprueba el proceso de compactacion de Memoria.";


