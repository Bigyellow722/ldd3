#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <linux/slab.h>
#include <asm/uaccess.h>


#include "scull.h"

MODULE_LICENSE("Daul BSD/GPL");


/*  */
int scull_major = SCULL_MAJOR;
int scull_minor = 0;
int scull_nr_devs = SCULL_DEVS;
int scull_quantum = SCULL_QUANTUM;
int scull_qset = SCULL_QSET;

struct scull_dev *scull_devs;


static loff_t scull_llseek(struct file *filp, loff_t offset, int size)
{
	loff_t res;
	return res;
}

static ssize_t scull_read(struct file *filp, char __user *buf, size_t size, loff_t *f_pos)
{
	struct scull_dev *dev = filp->private_data;
	struct scull_qset *dptr;
	int quantum = dev->quantum, qset = dev->qset;
	int itemsize = quantum * qset;
	int item, s_pos, q_pos, rest;
	ssize_t retval = 0;
	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;
	if (*f_pos >= dev->size)
		goto out;
out:
	up(&dev->sem);
	return retval;
}

static ssize_t scull_write(struct file *filp, const char __user *writebuf, size_t size, loff_t *offset)
{
	ssize_t res;
	return res;
}

static long scull_ioctl(struct file *filp, unsigned int value, unsigned long cmd)
{
	int res;
	return res;
}

static int scull_open(struct inode *node, struct file *filp)
{
	int res;
	return res;
}

static int scull_release(struct inode *node, struct file *filp)
{
	int res;
	return res;
}

struct file_operations scull_fops = {
	.owner = THIS_MODULE,
	.llseek = scull_llseek,
	.read = scull_read,
	.write = scull_write,
	.unlocked_ioctl = scull_ioctl,
	.open = scull_open,
	.release = scull_release,
};

static int scull_trim(struct scull_dev *dev)
{
	struct scull_qset *next, *dptr;
	int qset = dev->qset;
	int i;

	for (dptr = dev->data; dptr; dptr = next) {
		if (dptr->data) {
			for (i = 0; i < qset; i++) {
				kfree(dptr->data[i]);
			}
			kfree(dptr->data);
			dptr->data = NULL;
		}
		next = dptr->next;
		kfree(dptr);
	}
	dev->size = 0;
	dev->quantum = scull_quantum;
	dev->qset = scull_qset;
	dev->data = NULL;
	return 0;
}

static void scull_setup_cdev(struct scull_dev *dev, int index)
{
	int err, devno = MKDEV(scull_major, scull_minor);

	cdev_init(&dev->cdev, &scull_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &scull_fops;
	err = cdev_add(&dev->cdev, devno, 1);
	if (err)
		printk(KERN_NOTICE "Error %d adding scull%d", err, index);
}

void scull_cleanup(void)
{
	int i = 0;
	dev_t devno = MKDEV(scull_major, scull_minor);

	if (scull_devs) {
		for (i = 0; i < scull_nr_devs; i++) {
			scull_trim(scull_devs + i);
			cdev_del(&scull_devs[i].cdev);
		}
		kfree(scull_devs);
	}
}

static int scull_init(void)
{
	dev_t dev = 0;
	int res, i;
	int err, devno;
	printk(KERN_ALERT "scull init\n");

	/* alloc device number */
	if (scull_major) {
		dev = MKDEV(scull_major, scull_minor);
		res = register_chrdev_region(dev, scull_nr_devs, "scull");
	} else {
		res = alloc_chrdev_region(&dev, scull_minor, scull_nr_devs, "scull");
		scull_major = MAJOR(dev);
	}

	if (res < 0) {
		printk(KERN_WARNING "scull: can't get major %d\n", scull_major);
		return res;
	}
		
	/* alloc scull devices */
	scull_devs = kmalloc(scull_nr_devs * sizeof(struct scull_dev), GFP_KERNEL);
	if (!scull_devs) {
		res = -ENOMEM;
		goto fail;
	}
	memset(scull_devs, 0, scull_nr_devs * sizeof(struct scull_dev));

	/* initialize each scull device*/
	for (i = 0; i < scull_nr_devs; i++) {
		scull_devs[i].quantum = scull_quantum;
		scull_devs[i].qset = scull_qset;
		scull_setup_cdev(&scull_devs[i], i);
	}

	return 0;
fail:
	/*clean up*/
	return res;
}

static void scull_exit(void)
{
	printk(KERN_ALERT "scull exit\n");
}

module_init(scull_init);
module_exit(scull_exit);
