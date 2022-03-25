#!/bin/bash

for i in {0..1}
do
c=$(($i*2))
ssh root@localhost -p 2222 'cd /home/filippo ; 
	./fill_memory ;
	sleep 20 ;' &
echo "waiting for ssh" 
sleep 25
echo "waking up from sleep"
gnome-terminal -- sh -c './build_migration_1.sh'
sleep 1
gnome-terminal -- sh -c 'timeout 20 ./migration.sh'
sleep 25
#gnome-terminal -- sh -c 'timeout 3 nc localhost 5324 > result.txt <<END
#info migrate
#END'
timeout 3 nc localhost 5324 > result_$c.txt <<END
info migrate
END
sleep 2
gnome-terminal -- sh -c 'timeout 3 nc localhost 5324 <<END
q
END'
echo "done"
ssh root@localhost -p 2223 'cd /home/filippo ; 
	./fill_memory ;
	sleep 20 ;' &
echo "waiting for ssh on 2nd machine"
sleep 25
echo "waking up from sleep"
gnome-terminal -- sh -c './build_migration_2.sh'
sleep 1
gnome-terminal -- sh -c 'timeout 20 ./migration_2.sh'
sleep 25
c=$(($c+1))
timeout 3 nc localhost 5325 > result_$c.txt <<END
info migrate
END
sleep 2
gnome-terminal -- sh -c 'timeout 3 nc localhost 5325 <<END
q
END'
echo "done"
done
