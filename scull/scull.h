#ifndef __SCULL_H
#define __SCULL_H

#define SCULL_MAJOR	0
#define SCULL_DEVS	4

#define SCULL_QUANTUM	4000
#define	SCULL_QSET	500


/* scull device */
struct scull_qset {
	void **data;
	struct scull_qset *next;
};

struct scull_dev {
	struct scull_qset *data;
	int quantum;			/*the number of quantum in each qset*/
	int qset;			/*the number of qsets*/
	unsigned long size;
	unsigned int access_key;
	struct semaphore sem;
	struct cdev cdev;
};


#endif
