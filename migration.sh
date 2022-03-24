#!/bin/bash

nc localhost 5324 <<END
migrate -d tcp:localhost:4444
END