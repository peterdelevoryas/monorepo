alias grep="ag --nobreak --noheading"

function qemurun() {
    machine_type="$1"
    flash_image="$2"
    shift
    shift
    rm -f /tmp/a.mtd /tmp/b.mtd
    cp $flash_image /tmp/a.mtd
    cp /tmp/a.mtd /tmp/b.mtd
    $HOME/local/openbmc-qemu/build/qemu-system-arm \
        -machine $machine_type \
        -drive file=/tmp/a.mtd,if=mtd,format=raw \
        -drive file=/tmp/b.mtd,if=mtd,format=raw \
        -serial stdio -display none \
        -serial pty -serial pty -serial pty -serial pty \
        -netdev user,id=mlx,mfr-id=0x8119,ncsi-mac=aa:cd:ef:ab:cd:ef,hostfwd=::2222-:22 \
        -net nic,model=ftgmac100,netdev=mlx \
        $@
}

function flash2mtd() {
  local IMAGE_PATH="$1"
  local MTD_PATH="$2"
  dd if=/dev/zero of=$MTD_PATH bs=1M count=128
  dd if=$IMAGE_PATH of=$MTD_PATH bs=1k conv=notrunc
}
