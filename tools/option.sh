#! /bin/sh

echo "00 00 ff 00 ff 00 ff 00 ff 00 ff" | xxd -r -p > /tmp/stm8s103-option.bin
sudo stm8flash -c stlinkv2 -p stm8s103?3 -s opt -w /tmp/stm8s103-option.bin
