#!/bin/sh

mkdir rootfs
wget https://dl-cdn.alpinelinux.org/alpine/latest-stable/releases/x86_64/alpine-minirootfs-3.22.1-x86_64.tar.gz
cd rootfs
tar -xf ../alpine-minirootfs-3.22.1-x86_64.tar.gz
