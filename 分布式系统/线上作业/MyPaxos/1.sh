#!/bin/bash


 cd Paxos
 
 make clean   >/dev/null 
 make    2>&1    >/dev/null 
 
 cd bin

 rm -rf /root/out.txt
 
 ./Paxos_Test  > /root/out.txt
 

  yy=$(tail -n 2 /root/out.txt  | awk '{print $5}')
 
 
 echo ${yy:18:20}  

