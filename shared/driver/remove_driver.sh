#!/bin/sh

set -ex

# Teardown.
sudo rmmod driver
sudo rm -f /dev/newdev
