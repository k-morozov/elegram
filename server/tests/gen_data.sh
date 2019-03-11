#!/usr/bin/env bash

# generate schema
sudo -u postgres psql -U postgres -f gen_schema.sql
echo "gen_schema.sql    FINISHED"

# generate users and data
sudo -u postgres python3 gen_test_users.py
echo "gen_test_users.py FINISHED"
