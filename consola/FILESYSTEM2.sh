#!/bin/bash

./consola.out ./tuki-pruebas/FS_3 ./consola.config &
sleep 0.2; ./consola.out ./tuki-pruebas/FS_3 ./consola.config &
sleep 0.3; ./consola.out ./tuki-pruebas/FS_3 ./consola.config &
sleep 0.4; ./consola.out ./tuki-pruebas/FS_3 ./consola.config;
echo "Se ejecuto la prueba FILESYSTEM2 para comprobar la persistencia de los archivos generados por el File System.";