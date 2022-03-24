#!/bin/bash

gnome-terminal -- sh -c './buildubuntu.sh'
sleep 60
ssh root@localhost -p 2222 'cd /home/filippo ; 
	sleep 1 ; 
	./start.sh ; 
	umount shared ;
	sleep 5 ;
	./fill_memory ;
	sleep 20 ;' &
echo "waiting for ssh" 
sleep 50
echo "waking up from sleep"
gnome-terminal -- sh -c './build_migration_1.sh'
sleep 1
gnome-terminal -- sh -c 'timeout 20 ./migration.sh'
sleep 25
gnome-terminal -- sh -c 'timeout 3 nc localhost 5324 > result.txt <<END
info migrate
END'
sleep 2
gnome-terminal -- sh -c 'timeout 3 nc localhost 5324 <<END
q
END'
echo "done"
