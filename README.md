-----------------------------------------------------------
This setup is configured for windows
-----------------------------------------------------------
Clone this repo in the local machine:
-git submodule init and update (all the dependency) 
-pip install --upgrade protobuf grpcio-tools
-pip install protobuf-compiler
-python ./libs/nanopb/generator/nanopb_generator.py proto/message.proto (this will generate message.pb files)
Install Docker Desktop - Container (windows x64):
Install correct version for WSL (normally done automatically when opening the docker for the first time):
- docker-compose build
- docker-compose up
Open the workspace in Dev-container:
- Edit server.cpp
- Edit client.cpp
- Save
- docker compose up --build
- See output in terminal
For building and running use makefile