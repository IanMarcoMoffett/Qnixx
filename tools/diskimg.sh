#!/bin/bash

set -e

# Create the image.
qemu-img create disk.img 10G

# Create the filesystem.
mkfs.ext2 disk.img

# Copy the base syste, onto it.
mkdir -p disk_mnt
sudo mount disk.img disk_mnt
sudo cp -r base/* disk_mnt

# Cleanup.
sudo umount disk_mnt
rm -rf disk_mnt
