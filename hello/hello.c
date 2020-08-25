/************************************************************************
    > File Name: hello.c
    > Author: shejing
    > Mail: 2016192403@qq.com 
    > Created Time: Tue 25 Aug 2020 09:11:09 AM CST
***********************************************************************/
#include<linux/init.h>
#include<linux/module.h>
MODULE_LICENSE("Dual BSD/GPL");

static int hello_init(void)
{
    printk(KERN_ALERT "Hello, world\n");
    return 0;
}

static void hello_exit(void)
{
    printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);
