#!/usr/bin/env bash

# get postgres image
sudo docker pull postgres

# start docker container with postgres
sudo docker run --name some-postgres -e POSTGRES_PASSWORD=mysecretpassword -d postgres
