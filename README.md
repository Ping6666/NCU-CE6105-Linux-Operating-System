# LinuxOS-Projects

110-01 NCU CE6105 Linux Operating System

[CE6105 Linux OS | NCU Fu-Hau Hsu](https://staff.csie.ncu.edu.tw/hsufh/COURSES/FALL2021/linuxos.html)

## Environment 

1. ubuntu 20.04: kernel version 5.11
2. ubuntu 18.04: kernel version 5.4

### Installation

```cmd=1
# Fully update the OS
sudo apt-get update && sudo apt-get upgrade -y

# just for the vm
sudo apt-get install open-vm-tools-desktop -y

# essential packages to compile kernels
sudo apt-get install build-essential libncurses-dev libssl-dev libelf-dev -y

# Clean up the installed packages
sudo apt-get clean && sudo apt-get autoremove
```

```cmd=1
# Download the source code of the Linux kernel
wget -P ~/ https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.14.252.tar.xz

# Unpack the tarball
tar -xvf ~/linux-4.14.252.tar.xz -C ~/
```

### make

```cmd=1
# configure the kernel
make menuconfig
## 1. use Tab to move between options
## 2. 5-level paging tables
## 3. CONFIG_SYSTEM_TRUSTED_KEYS

# make with parallel processing
## compile the kernel's source code
## prepare the modules
## install the kernel
sudo make ARCH=$(arch) -j$(nproc) && sudo make modules_install ARCH=$(arch) -j$(nproc) && sudo make install ARCH=$(arch) -j$(nproc)

# update the bootloader of the OS with the new kernel
sudo update-grub

# then, boot in recovery mode
```

#### make error: debian/canonical-certs.pem

> ./.config

```
// CONFIG_SYSTEM_TRUSTED_KEYS="debian/canonical-certs.pem"
CONFIG_SYSTEM_TRUSTED_KEYS=""
```

#### change grub setting

```cmd=1
sudo vi /etc/default/grub

## change below two lines ##
# GRUB_TIMEOUT_STYLE=hidden
# GRUB_TIMEOUT=0

sudo update-grub
```

#### system halted

> console print

```
This linux kernel as configured requires 5-level paging
This cpu does not support the required "cr4.la57" features.
Unable to boot
Please use a kernel appropiate for your cpu.

system halted
```

> solution

```cmd=1
make menuconfig

# GO TO "Processor type and features"
# TURN OFF "Enable 5-level page tables support"
```

### boot fail

#### root file system requires manual fsck

```cmd=1
# RUN fsck MANUALLY
fsck -yf /dev/sda{%i}

# Then, exit or reboot.
exit
reboot
```

## Project 1

[Project 1 README.md](./Project_1/README.md)

## Project 2

[Project 2 README.md](./Project_2/README.md)

## Project 3

[Project 3 README.md](./Project_3/README.md)

## License

plz follow MIT License \
within Linux need to follow its License (GPL-2.0)
