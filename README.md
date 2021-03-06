# pintos

# Usage
```sh
git clone https://github.com/lizhijian-cn/pintos.git

cd pintos

# add pintos to $PATH
echo "export PINTOS_HOME=$PWD" >> ~/.zshrc
echo "export PATH=\$PATH:\$PINTOS_HOME/src/utils" >> ~/.zshrc
source ~/.zshrc
```

## check project 1 threads, 27 tests
```
git checkout threads
cd $PINTOS_HOME/src/threads
make check
```

## check project 2 userprog, 80 tests
```
git checkout userprog
cd $PINTOS_HOME/src/userprog
make check
```
![](https://raw.githubusercontent.com/lizhijian-cn/static/master/img/20201213023210.png)

## check project 3 vm, 
```
git checkout vm
cd $PINTOS_HOME/src/vm
make check
```

# Environment
* ubuntu 20.04 (wsl2)
* bochs 2.6.10 
* qemu (if you want to debug in gdb)

## Install bochs
``` sh
wget https://sourceforge.net/projects/bochs/files/bochs/2.6.10/bochs-2.6.10.tar.gz
tar xzf bochs-2.6.10.tar.gz
cd bochs-2.6.10

# make and install bochs
./.conf.linux --enable-gdb-stub --with-nogui
make
sudo make install
```

**If you got an `Could not open wave output device` panic, you can modify the bochs source code and remake it**

~~I dont know why, but it can run :)~~

```c
// bochs-2.6.10/iodev/sound/soundmod.cc
// in function void bx_soundmod_ctl_c::init()

    ret = waveout->openwaveoutput(pwaveout);

    ret = BX_SOUNDLOW_OK; // add this line
    
    if (ret != BX_SOUNDLOW_OK) {
      BX_PANIC(("Could not open wave output device"));
    }
```

## Install qemu
```
# install qemu if you want to debug in gdb
sudo apt install qemu-system

# debug pintos just like this
# add --qemu --gdb
cd $PINTOS_HOME/src/threads/
make
pintos -v -k -T 60 --qemu --gdb  -- -q  run alarm-single
```

# PS
由于使用的Ubuntu20.04移除了一些32位库和某些不知情的原因，一直无法成功编译安装bochs和pintos。感谢这位同学提供的移植到20.04的pintos，我也做了一些摸索，删除了pintos中的第564行
```
user_shortcut: keys=ctrlaltdel
```
现在已经可以编译运行了，但**无法直接在bochs下调试**，必须安装`qemu-system`用qemu进行调试。
