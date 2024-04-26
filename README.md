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

## Acknowledgements

- Containers From Scratch • Liz Rice • GOTO 2018 <https://www.youtube.com/watch?v=8fi7uSYlOdc>
- <https://blog.lizzie.io/linux-containers-in-500-loc.html>
- Alpine minirootfs <https://dl-cdn.alpinelinux.org/alpine/latest-stable/releases/x86_64/>
