#!/bin/sh

echo_and_eval() {
    echo ${1}
    eval ${1}
}

main() {
    
    docker="/c/Program Files/Docker/Docker/resources/bin/docker.exe"
    image_name="vsc_docker_cc_gdb"

    echo_and_eval "'${docker}' build -f ./Dockerfile -t ${image_name} ."
    # docker-compose down
    # docker-compose up web-windows
    # echo "If the docker-compose up failed, ensure that all modules are present."
}

main
