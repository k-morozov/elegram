#!/bin/bash

# start docker container with postgres
#sudo docker run --name some-postgres -e POSTGRES_PASSWORD=mysecretpassword -d postgres

# generate schema
sudo -u postgres psql -U postgres -f gen_schema.sql
echo "gen_schema.sql    FINISHED"

# generate users and data
sudo -u postgres python3 gen_test_users.py
echo "gen_test_users.py FINISHED"
