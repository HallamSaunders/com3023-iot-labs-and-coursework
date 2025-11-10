# To run QEMU with the virtual IO setup

```bash
qemu-system-x86_64 \
  -enable-kvm -m 4G \
  -drive file=Downloads/EEEM048.qcow2,format=qcow2 \
  -virtfs local,id=shared_folder,path=Documents/qemu-virt/,security_model=none,mount_tag=hostshare
```

# To mount the drive (run inside QEMU)

```bash
sudo mount -t 9p -o trans=virtio hostshare /mnt
```
