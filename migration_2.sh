#!/bin/bash

nc localhost 5325 <<END
migrate -d tcp:localhost:5555
END