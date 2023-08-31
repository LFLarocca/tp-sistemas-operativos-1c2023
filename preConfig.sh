#!/bin/bash
tp="$(pwd)"

cd;

if [[ ! -f /usr/include/commons/error.h ]]; then

  if [[ -d ./so-commons-library/ ]]; then
    echo "Ya existe carpeta commons, eliminandola";
    rm -rf ./so-commons-library/
  fi;

  git clone https://github.com/sisoputnfrba/so-commons-library && 
    cd ./so-commons-library; make install; cd ..; rm -rf ./so-commons-library/

else echo "Ya esta instalado";

fi;

cd "${tp}";
