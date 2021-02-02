# SoboRequest
SoboRequest is a simple library created in order to facilitate making of HTTPS requests in C.
It makes an extensive use of libcurl; while libcurl is one of the most amazing libraries ever created, which I truly and seriously admire, it is also quite complex to use because of multitude of options to be set and paths to be taken. Therefore, my goal was to create something SIMPLE, allowing to make requests easily and quickly.


# Copyright
© Copyright by Krzysztof Sobolewski 2020-2021.
SoboRequest is Open Source software, licensed under the MIT license available here:
https://opensource.org/licenses/mit-license.php

# Contact
You can contact the author using the email:
<krzysztof.sobolewski@gmail.com>
I'm looking for cooperation in the further development of this software.

# Compilation and Installation
You may install SoboRequest either with tests or without them. 
## Build SoboRequest without tests:
```make``` or ```make all```
In the root directory of the project. 
The binaries will be built into ./bin. 
The binaries are:
```soborequest.o```

## Build SoboRequest with tests:
```make alltest```
In the same directory will do the same + build tests. 
The binaries will be built into ./bin.
The binaries are:
```soborequest.o```
```test_request```

## Install in the common paths:
After ```make```, ```make all```  or ```make alltest``` has been done, call:
```make install```
in the root directory of the project.
**WARNING: requires the sudo access**.

The library (soborequest.a) and the header file will be put in the target common directories, defined into the makefile in the variables:
```SOBOREQUEST_INCLUDE_PATH ?= '/usr/local/share/soborequest'```
```SOBOREQUEST_LIB_PATH     ?= '/usr/local/lib/soborequest'```

You may change those paths to conform with your system configuration. 


## Server-side application for testing
Also, you need the server-side application in order to test SoboRequest. 
For *test_request*, there is a server-side counterpart named *api.py*. It's  
located in the *python* directory in our project tree.

It's a simple application written in Python 3.9 using Flask and Flask-Restful:
    https://flask-restful.readthedocs.io/en/latest/
Its installation is also very simple - having Python 3.x (preferably 3.9) installed, please
select a directory where you would like to put the environment settings for Python + Flask.
From this directory, please call:
    ```python -m venv api_fl```
Then:
    ``` source api_fl/bin/activate```

Bang! You're into the api_fl environment. The last thing to do is to call:
    ```pip install flask-restful```

From now, you can start the python application *api.py* in the activated Python environment.
Go to the "python" directory in our project and type: 
    ```python api.py```
It will start the application server on the port 5000. 
From now, you can make requests to this server, ie: 
    ```http://127.0.0.1:5000```


# Usage in your projects
TODO. Will be written ASAP.
For now, please just take a look at the code starting with src/test/test_request.c 

# Tested and tests passing on:
```ArchLinux 5.10.7-arch1-1```
```MacOS Catalina 10.15.6```


# TODOs
    - PUT method - it's a bit tricky but I've found out how to handle it
    - authentication
    - uploading/downloading of files
    - handling forms in some easy way
