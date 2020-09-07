#ifndef _SCULL_H_
#define _SCULL_H_

#include<linux/ioctl.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/moduleparam.h>
#include<linux/kernel.h>
#include<linux/slab.h>
#include<asm/uaccess.h>
#include<asm/switch_to.h>
#include<linux/semaphore.h>
#include<linux/errno.h>

struct scull_qset {
    void **data;
    struct scull_q  *next;
};

struct scull_dev {
    struct scull_qset *data;
    int    quantum;
    int    qset;
    unsigned long size;
    unsigned int  access_key;
    struct semaphore sem;
    struct cdev   cdev;
};


int scull_trim(struct scull_dev *dev);
ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
ssize_t scull_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);



#endif
