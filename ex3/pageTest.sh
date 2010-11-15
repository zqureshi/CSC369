#!/usr/bin/env bash

echo -e "VM Page Fault Stress Test\n"

for data_size in 8 16 32 64 128 256
do
  echo "DATASIZE: ${data_size}"
  export DATASIZE=${data_size}
  make -s clean
  make -e
  echo ''

  for num_ops in 10000 50000 100000 500000
  do
    i=0
    while [ ${i} -lt 3 ]
    do
      echo -e "DATASIZE: ${data_size}, ${num_ops} ops, run $(( ${i} + 1 )):"

      echo 'testheap:'
      ./testheap ${num_ops}
      echo ''

      echo 'testbh:'
      ./testbh ${num_ops}
      echo '----------------------------------------------------------------'
      echo ''

      i=$(( ${i} + 1 ))
    done
  done
done
