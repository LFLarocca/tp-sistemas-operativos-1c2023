#!/bin/bash

./consola.out ./tuki-pruebas/BASE_1 ./consola.config &
sleep 1; ./consola.out ./tuki-pruebas/BASE_2 ./consola.config &
sleep 1; ./consola.out ./tuki-pruebas/BASE_2 ./consola.config;
echo "Prueba BASE terminada: Se valido la ejecucion correcta del Kernel, CPU y la consola misma.";
