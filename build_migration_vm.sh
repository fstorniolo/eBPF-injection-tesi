\#!/usr/bin/env bash

set -eux

# Parameters.
id=ubuntu-20.04.3-live-server-amd64 #ubuntu-18.04.1-desktop-amd64
disk_img="${id}.img.qcow2"
disk_img_snapshot="${id}.snapshot.qcow2"
iso="${id}.iso"  

# Start VM

/home/filippo/Desktop/Tesi/qemu/build/x86_64-softmmu/qemu-system-x86_64 \
  -drive "file=${disk_img},format=qcow2" \
  -enable-kvm \
  -m 2G \
  -smp 2 \
  -device virtio-net-pci,netdev=ssh \
  -netdev user,id=ssh,hostfwd=tcp::2223-:22 \
  -fsdev local,id=test_dev,path=shared,security_model=none \
  -device virtio-9p-pci,fsdev=test_dev,mount_tag=shared \
  -device newdev \
  -incoming tcp:0:4444 \
  -serial stdio \
  "$@" \
;

  # -device virtio-net-pci,netdev=ssh \
  # -netdev user,id=ssh,hostfwd=tcp::2222-:22 \
  # -fsdev local,id=test_dev,path=shared,security_model=none \
  # -device virtio-9p-pci,fsdev=test_dev,mount_tag=shared \
  # -device newdev \
