#!/bin/bash

docker build --progress=plain -t rv32x_dev:v1.0 . 2>&1 | tee build.log
