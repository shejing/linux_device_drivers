/************************************************************************
    > File Name: scull.c
    > Author: shejing
    > Mail: 2016192403@qq.com 
    > Created Time: Wed 26 Aug 2020 11:38:56 PM CST
***********************************************************************/
#include "scull.h"

#define SCULL_QUANTUM 1000
#define SCULL_SET     4000

static dev_t scull_major = 0;
static dev_t scull_minor = 0;

module_param(scull_major, dev_t, S_IRUGO);
module_param(scull_minor, dev_t, S_IRUGO);

struct file_operations scull_fops = {
    .owner = THIS_MODULE,
//    .llseek = scull_llseek,
    .read   = scull_read,
    .write  = scull_write,
//    .ioctl  = scull_ioctl,
    .open   = scull_open,
    .release= scull_release,
};


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
            for (i = 0; i < qset; i++)
                kfree(dptr->data[i]);
            kfree(dptr->data);
            dptr->data = NULL;
        }
        next = dptr->next;
        kfree(dptr);
    }
    dev->size = 0;
    dev->quantum = SCULL_QUANTUM;
    dev->qset    = SCULL_SET;
    dev->data    = NULL;
    return 0;
}

int scull_open(struct inode *inode, struct file *filp)
{
   struct scull_dev *dev;

   dev = container_of(inode->i_cdev, struct scull_dev, cdev);
   filp->private_data = dev;

   if ( (filp->f_flags & O_ACCMODE) == O_WRONLY) {
    scull_trim(dev);
   }
   return 0;
}

int scull_release(struct inode *inode, struct file *filp)
{
    return 0;
}

ssize_t scull_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
    struct scull_dev = filp->private_data;
    struct scull_qset *dptr;
    int quantum      = dev->quantum;
    int qset         = dev->qset;
    int itemsize     = quantum * qset;
    int item, s_pos, q_pos, rest;
    ssize_t retval   = 0;

    if (down_interruptible(&dev->sem)) //如果信号量小于0,则down_interruptible使得进程进入可中断睡眠
        return -ERESTARTSYS;

    //临界区
    //偏移大于保存在设备中的数据总量
    if (*f_pos >= dev->size)
        goto out;
    if (*f_pos + count > dev->size)
        count = dev->size - *f_pos;

    //链表项
    item = (long)*f_pos / itemsize;
    rest = (long)*f_pos % itemsize;
    //量子项
    s_pos = rest / quantum;
    //量子中的偏移
    q_pos = rest % quantum;

    dptr = scull_follow(dev, item);

    if(dptr == NULL || !dptr->data || dptr->data[s_pos])
        goto out;

    //每次只处理一个量子，超过了这部分的不处理
    if (count > quantum - q_pos)
        count = quantum - q_pos;

    if (copy_to_user(buf, dptr->data[s_pos] + q_pos, count)) {
        retval = -EFAULT;
        goto out;
    }
    *f_pos += count;
    retval  = count;

 out:
    up(&dev->sem);
    return retval;
    
}

ssize_t scull_write(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
    struct scull_dev = filp->private_data;
    struct scull_qset *dptr;
    int quantum      = dev->quantum;
    int qset         = dev->qset;
    int itemsize     = quantum * qset;
    int item, s_pos, q_pos, rest;
    ssize_t retval   = -ENOMEM;

    if (down_interruptible(&dev->sem)) //如果信号量小于0,则down_interruptible使得进程进入可中断睡眠
        return -ERESTARTSYS;

    //临界区

    //链表项
    item = (long)*f_pos / itemsize;
    rest = (long)*f_pos % itemsize;
    //量子项
    s_pos = rest / quantum;
    //量子中的偏移
    q_pos = rest % quantum;

    dptr = scull_follow(dev, item);

    if(dptr == NULL)
        goto out;

    if(!dptr->data){
        dptr->data = kmalloc(qset * sizeof(char *), GFP_KERNEL);
        if (!dptr->data)
            goto out;
        memset(dptr->data, 0, qset * sizeof(char *));
    }

    if(!dptr->data[s_pos]){
        dptr->data[s_pos] = kmalloc(quantum, GFP_KERNEL);
        if(!dptr->data[s_pos])
            goto out;
    }

    //每次只处理一个量子，超过了这部分的不处理
    if (count > quantum - q_pos)
        count = quantum - q_pos;

    if (copy_from_user(dptr->data[s_pos] + q_pos, buf, count)) {
        retval = -EFAULT;
        goto out;
    }
    *f_pos += count;
    retval  = count;

    //更新文件的大小
    if (dev->size < *f_pos)
        dev->size = *f_pos;

 out:
    up(&dev->sem);
    return retval;
    
}





