#!/bin/bash

archivos=($(find ./ -name '*.config'))

modulos=(MEMORIA CPU FILESYSTEM KERNEL)

echo "IP de la Memoria: "
read IP[0]

echo "IP del CPU: "
read IP[1]

echo "IP del FileSystem: "
read IP[2]

echo "IP del Kernel: "
read IP[3]

for i in ${archivos[@]};
    do for j in {0..4};
        do sed -i "s/IP_${modulos[$j]}=[a-z0-9\.]*/IP_${modulos[$j]}=${IP[$j]}/" "$i";
    done;
done;