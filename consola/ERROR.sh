#!/bin/bash

./consola.out ./tuki-pruebas/ERROR_1 ./consola.config &
sleep 0.2; ./consola.out ./tuki-pruebas/ERROR_2 ./consola.config &
sleep 0.3; ./consola.out ./tuki-pruebas/ERROR_3 ./consola.config &
sleep 0.4; ./consola.out ./tuki-pruebas/ERROR_4 ./consola.config;
echo "Se ejecuto la prueba ERROR, donde cuatro errores distintos terminan los procesos antes de tiempo.";
