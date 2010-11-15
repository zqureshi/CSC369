#!/usr/bin/env bash

echo -e "VM Page Fault Stress Test\n"

for num_ops in 10000 50000 100000 500000
do
  i=0
  while [ ${i} -lt 10 ]
  do
    echo -e "${num_ops} ops, run ${i}:\n"

    echo 'testheap:'
    ./testheap ${num_ops}
    echo ''

    echo 'testbh:'
    ./testbh ${num_ops}
    echo ''

    i=$(( ${i} + 1 ))
  done
done
