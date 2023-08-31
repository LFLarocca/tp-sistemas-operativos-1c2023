#!/bin/bash

./consola.out ./tuki-pruebas/MEMORIA_1 ./consola.config &
sleep 0.2; ./consola.out ./tuki-pruebas/MEMORIA_2 ./consola.config &
sleep 0.3; ./consola.out ./tuki-pruebas/MEMORIA_3 ./consola.config;
echo "Se ejecuto la prueba MEMORIA, donde se validan los distintos metodos de asignacion de segmentos de Memoria.";