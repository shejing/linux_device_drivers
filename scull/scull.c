/************************************************************************
    > File Name: scull.c
    > Author: shejing
    > Mail: 2016192403@qq.com 
    > Created Time: Wed 26 Aug 2020 11:38:56 PM CST
***********************************************************************/
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/moduleparam.h>

static dev_t scull_major = 0;
static dev_t scull_major = 0;
module_param(scull_major, dev_t, S_IRUGO);
module_param(scull_minor, dev_t. S_IRUGO);

struct file_operations scull_fops = {
    .owner = THIS_MODULE,
    .llseek = scull_llseek,
    .read   = scull_read,
    .write  = scull_write,
    .ioctl  = scull_ioctl,
    .open   = scull_open,
    .release= scull_release,
};

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
}

static void scull_setup_cdev(struct scull_dev *dev, int index)
{
    int err, devno = MKDEV(scull_major, scull_minor + index);
    cdev_init(&dev->cdev, &scull_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops   = &scull_fops;
    err = cdev_add(&dev->cdev, devno, 1);
    if (err)
        printk(KERN_NOTICE "Error %d adding scull%d", err, index);
}

int scull_trim(struct scull_dev *dev)
{
    struct scull_qset *next, *dptr;
    int qset = dev->qset;
    int i;
    for (dptr = dev->data; dptr; dptr = next) {
        if (dptr->data) {
            for (i = 0； i < qset; i++)
                kfree(dptr->data[i]);
            kfree(dptr->data);
            dptr->data = NULL;
        }
        next = dptr->next;
        kfree(dptr);
    }
    dev->size = 0;
    dev->quantum = scull_quantum;
    dev->qset    = scull_qset;
    dev->data    = NULL;
    return 0;
}







