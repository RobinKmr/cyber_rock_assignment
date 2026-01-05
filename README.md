-----------------------------------------------------------
This setup is configured for Linux
-----------------------------------------------------------
For this repo in the local machine:
`install git`
`sudo apt update`
`sudo apt install git`
`sudo apt install git-all`
`git --version - check git version`
`git config --global user.name "Your Name" `- set full name of the user 
`git config --global user.email "your.email@example.com" `- set email for the user
`git config --list - verify configuration`

`mkdir Git` - Create new folder name
`cd Git` - make Git current directory

Generate ssh key to get the repo from the GitHub as it is private
`ssh-keygen -t ed25519 -C "your_email@example.com" ` this will create the key, use your own email and after executing this command select the location where you want to save the key and use password for extra security
`eval "$(ssh-agent -s)"` - this command start the ssh key on the machine
`ssh-add ~/.ssh/id_ed25519` - add private key to the machine agent then no need to start the key after every restart
`cat ~/.ssh/id_ed25519.pub` - copy the key and use it on the GitHub to clone the repo

`git clone https://github.com/RobinKmr/cyber_rock_assignment.git` this is the main working repo

`git submodule init` - initialize the submodules
`git submodule update` - update the submodules

Get gcc and other libraries
`sudo apt update`
`sudo apt install -y build-essential cmake make g++ libssl-dev`

For installing the boost submodules
`sudo apt update`
`sudo apt install build-essential python3 libbz2-dev libz-dev libicu-dev`
`sudo apt install libboost-all-dev`

`git submodule init`
`git submodule update`
`./bootstrap.sh`
`./b2`
`./b2 install`

For installing nanopb
`sudo apt install protobuf-compiler python3 - installing compiler`

`apt install python3.13-venv` - installing venv for simplicity
`python3 -m venv venv` - create venv
`source venv/bin/activate` - activating venv

`pip install --upgrade protobuf grpcio-tools` - some google tool used

`mkdir generated` - create folder where the nanopb code will be generated
`python libs/nanopb/generator/nanopb_generator.py proto/sensor.proto --output-dir=generated`