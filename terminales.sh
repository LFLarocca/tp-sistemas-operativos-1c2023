#!/bin/bash

t_kitty="kitty -d"
t_xterm="xterm -hold -e "

modulos=('memoria' 'cpu' 'fileSystem' 'kernel' 'consola')

moveToTp() {
    echo $(ls -R ~ |grep tp-2023-1c-toTheEnd|grep /home/|grep End:|sed 's/.$//')
}

for i in {0..4};
    do ${t_kitty} "$(moveToTp)/${modulos[$i]}"&
done