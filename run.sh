#!/bin/bash
docker run -it --rm \
    --net host \
    -e LOCAL_UID=$(id -u $USER) \
    -e LOCAL_GID=$(id -g $USER) \
    -e USER=$USER \
    -e DISPLAY=$DISPLAY \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    --privileged \
    -v /sys:/sys:ro \
    -v /opt/intelFPGA_lite:/opt/intelFPGA_lite \
    -v /opt/nslcore/linux/x86_64:/opt/nslcore/linux/x86_64 \
    -v $(pwd)/core:/root/core \
    -v $(pwd)/software:/root/software \
    -v $(pwd)/docs:/root/docs \
	-v $(pwd)/integration:/root/integration \
	-v $(pwd)/simulation:/root/simulation \
    koyaman/rv32x_dev:1.0 $1

