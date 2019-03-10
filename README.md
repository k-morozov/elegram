# Elegram
[![Build Status](https://travis-ci.com/mkvdv/elegram.svg?branch=master)](https://travis-ci.com/mkvdv/elegram)

A toy instant message service, both server and client (builded using QT) applications.

## Requirements
* Server require this libraries:
   * [Boost](https://www.boost.org/) library [version 1.67.0.0](https://packages.ubuntu.com/cosmic/libboost-all-dev) 
   for ASIO, program_options and logging
   * OpenSSL for hashing functions
   * PostgreSQL server (use docker image in examples for it)
   * [pqxx](http://pqxx.org/development/libpqxx/) library for easy connections PostgreSQL server through pq library
   * [Google Protobuf](https://developers.google.com/protocol-buffers/) for message serialization/deserialization
   
   
* Client require:
    * QT 5
    * [Google Protobuf](https://developers.google.com/protocol-buffers/)
   
## Documentation
* Some notes about serer [here](docs/server.md)
