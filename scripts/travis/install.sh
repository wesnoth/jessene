#!/bin/bash
echo "FROM wesnoth/jessene:1804-master" > docker/Dockerfile-travis-1804-master
echo "COPY ./ /jessene-travis/" >> docker/Dockerfile-travis-1804-master
echo "WORKDIR /jessene-travis" >> docker/Dockerfile-travis-1804-master

docker build -t wesnoth/jessene-repo:1804-master -f docker/Dockerfile-travis-1804-master .
