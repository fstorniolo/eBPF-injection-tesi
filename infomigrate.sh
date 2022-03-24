#!/bin/bash

nc localhost 5324 > result.txt <<END
info migrate
END
