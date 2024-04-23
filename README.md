# docky

Docky isolates linux processes 🐢

## Usage

Install dependencies

```sh
# To manage linux capabilities(7)
sudo apt install libcap-dev
```

```sh
make
sudo setcap cap_sys_admin+ep ./dist/docky
./dist/docky /bin/bash
```
