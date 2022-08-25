#!/bin/bash

echo 123446789 >out.txt
exec 3<> out.txt
read -n 4 <&3
echo -n 5 >&3
exec 3>&-
