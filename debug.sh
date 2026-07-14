make clean
make
make qemu &

xfce4-terminal --working-directory="$HOME/hrishikos" --command="bash -ic 'sleep 0.2; gdb build/os/os.elf'"
