cmd_/home/jing/workspace/linux_device_drivers/hello/modules.order := {   echo /home/jing/workspace/linux_device_drivers/hello/hello.ko; :; } | awk '!x[$$0]++' - > /home/jing/workspace/linux_device_drivers/hello/modules.order