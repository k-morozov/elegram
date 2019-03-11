# Elegram
[![Build Status](https://travis-ci.com/mkvdv/elegram.svg?branch=master)](https://travis-ci.com/mkvdv/elegram)
[![Download](https://api.bintray.com/packages/mkvdv/elegram/elegram/images/download.svg?version=elegram_server) ](https://bintray.com/mkvdv/elegram/elegram/elegram_server/link)
[![Download](https://api.bintray.com/packages/mkvdv/elegram/elegram/images/download.svg?version=elegram_client) ](https://bintray.com/mkvdv/elegram/elegram/elegram_client/link)


A toy instant message service, both server and client (builded using QT) applications.

## Requirements
* Server require this libraries:
   * [Boost](https://www.boost.org/) library [version 1.67](https://packages.ubuntu.com/cosmic/libboost-all-dev) 
   for ASIO, program_options and logging
   * OpenSSL for hashing functions
   * PostgreSQL server (use docker image in examples for it)
   * [pqxx](http://pqxx.org/development/libpqxx/) library for easy connections PostgreSQL server through pq library
   * [Google Protobuf](https://developers.google.com/protocol-buffers/) for message serialization/deserialization
   
   
* Client require:
    * [QT 5](https://www.qt.io/)
    * [Google Protobuf](https://developers.google.com/protocol-buffers/)
   
## Documentation
* Some notes about server [here](docs/server.md)
* Some notes about client [here](docs/client.md)
