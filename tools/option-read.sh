#! /bin/sh

sudo stm8flash -c stlinkv2 -p stm8s103?3 -s opt -r /tmp/stm8s103-option.bin
