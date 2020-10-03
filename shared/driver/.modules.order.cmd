cmd_/home/giacomo/shared/driver/modules.order := {   echo /home/giacomo/shared/driver/driver.ko; :; } | awk '!x[$$0]++' - > /home/giacomo/shared/driver/modules.order
