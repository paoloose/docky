# docky

Docky isolates linux processes 🐢

## Usage

Install dependencies

```sh
# To manage linux capabilities(7)
sudo apt install libcap-dev
```

Pull the root filesystem image (currently using Alpine's minirootfs)

```sh
./pull_rootfs.sh
```

And run the container

```sh
make
./dist/docky /bin/sh
```

## Specific implementation details

- Exclusively uses cgroups v2 and does not support cgroups v1.
  Linux kernel >= 5.8 is
  [needed](https://kubernetes.io/docs/concepts/architecture/cgroups/#requirements).
- It depends on an external filesystem to be downloaded and mounted.
  See `pull_rootfs.sh`

## Acknowledgements

- Containers From Scratch • Liz Rice • GOTO 2018 <https://www.youtube.com/watch?v=8fi7uSYlOdc>
- Linux containers in 500 lines of code <https://blog.lizzie.io/linux-containers-in-500-loc.html>
- Alpine minirootfs <https://dl-cdn.alpinelinux.org/alpine/latest-stable/releases/x86_64/>
