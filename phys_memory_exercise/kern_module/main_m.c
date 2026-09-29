#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/cdev.h>

#include "solution.h"


static dev_t first;
static unsigned int count = 1;
static int max_chr_drv_major = 500;
static int max_chr_drv_minor = 0;
static struct cdev *maxs_cdev;

#define MAX_DEV_NAME "maxchrdev"
#define KBUF_SIZE (size_t) 10*PAGE_SIZE

static struct class *maxs_class;


static int max_chr_drv_open(struct inode *inode, struct file *file) {
	char *kbuf = kmalloc(KBUF_SIZE, GFP_KERNEL);
	file->private_data = kbuf;

	printk( KERN_INFO "Opening device: %s \n", MAX_DEV_NAME);

	return 0;
}

static int max_chr_drv_release(struct inode *inode, struct file *file) {
	char *kbuf = file->private_data;
	printk( KERN_INFO "Releasing device: %s \n", MAX_DEV_NAME);
	if (kbuf) kfree(kbuf);
	kbuf = NULL;
	file->private_data = NULL;

	return 0;
}

static ssize_t max_chr_drv_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos) {
	char *kbuf = file->private_data;
	int nbytes = lbuf - copy_to_user(buf, kbuf + *ppos, lbuf);

	*ppos += nbytes;
	printk( KERN_INFO "Reading device: %s, bytes readed = %d, ppos = %d\n", MAX_DEV_NAME, nbytes, (int) *ppos );
	return nbytes;
}

static unsigned int pid_to_cr3(int pid)
{
    struct task_struct *task;
    struct mm_struct *mm;
    void *cr3_virt;
    unsigned int cr3_phys;

    task = pid_task(find_vpid(pid), PIDTYPE_PID);

    if (task == NULL) return 0; // pid has no task_struct

    mm = task->mm;

    // mm can be NULL in some rare cases (e.g. kthreads)
    // when this happens, we should check active_mm
    if (mm == NULL) mm = task->active_mm;

    if (mm == NULL) return 0;

    cr3_virt = (void *) mm->pgd;
    cr3_phys = virt_to_phys(cr3_virt);

    return cr3_phys;
}

void print_page_table_defines(void) {
	printk( KERN_INFO "CONFIG_PGTABLE_LEVELS: %d\n", CONFIG_PGTABLE_LEVELS);
	printk( KERN_INFO "PAGE_SHIFT: %d  PAGE_SIZE: %ld  PAGE_MASK: 0x%lX\n", PAGE_SHIFT, PAGE_SIZE, PAGE_MASK);
	printk( KERN_INFO "PMD_SHIFT: %d  PMD_SIZE: %ld  PMD_MASK: 0x%lX\n", PMD_SHIFT, PMD_SIZE, PMD_MASK);
	printk( KERN_INFO "PUD_SHIFT: %d  PUD_SIZE: %ld  PUD_MASK: 0x%lX\n", PUD_SHIFT, PUD_SIZE, PUD_MASK);
	printk( KERN_INFO "PGDIR_SHIFT: %d  PGDIR_SIZE: %ld  PGDIR_MASK: 0x%lX\n", PGDIR_SHIFT, PGDIR_SIZE, PGDIR_MASK);
	printk( KERN_INFO "PTRS_PER_PTE: %d  PTRS_PER_PMD: %d  PTRS_PER_PUD: %d PTRS_PER_PGD: %d\n", PTRS_PER_PTE, PTRS_PER_PMD, PTRS_PER_PUD, PTRS_PER_PGD);
}

unsigned int read_physical_data(void *buf, const unsigned int size, const unsigned int physical_addr) {
	return 0;
}

static ssize_t max_chr_drv_write(struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos) {
	char *kbuf = file->private_data;
	int nbytes = lbuf - copy_from_user(kbuf + *ppos, buf, lbuf);
    
    unsigned int proc_id;
    unsigned int logic_addr;
    unsigned int cr3_contents;
    unsigned int phys_addr;
    int tr_res;

	*ppos += nbytes;

	printk( KERN_INFO "Writing device: %s, bytes writed = %d\n", MAX_DEV_NAME, nbytes);
	printk( KERN_INFO " * * * * * * * * * * * * * * * * * * * * * * * * * * * * \n" );

	if ( nbytes >= 8 ) {
		proc_id = *((unsigned int *) kbuf);
		logic_addr = *((unsigned int *)(kbuf + (sizeof(unsigned int))));

		print_page_table_defines();

		printk( KERN_INFO "pid : %d, logic_addr: 0x%0*X\n", proc_id, 8, logic_addr);

		cr3_contents = pid_to_cr3(proc_id);
		if (cr3_contents) {
			printk( KERN_INFO "cr3_contents: 0x%0*X\n", 8, cr3_contents);
			tr_res = va2pa(logic_addr, CONFIG_PGTABLE_LEVELS, cr3_contents, read_physical_data, &phys_addr);
			printk( KERN_INFO "va2pa result: %d, phys_addr: 0x%X\n", tr_res, phys_addr);
		} else printk ( KERN_INFO "pid_to_cr3 error\n");
    }

	return nbytes;
}

static loff_t max_chr_drv_lseek (struct file *file, loff_t offset, int orig) {
    loff_t testpos;

    switch (orig) {
        case SEEK_SET :
            testpos = offset;
            break;
        case SEEK_CUR : 
            testpos = file->f_pos + offset;
            break;
        case SEEK_END : 
            testpos = KBUF_SIZE + offset;
            break;
        default:
            return -EINVAL;
            break;
    }

    if (testpos >= KBUF_SIZE) testpos = KBUF_SIZE - 1;
    if (testpos < 0) testpos = 0;
    file->f_pos = testpos;

    printk ( KERN_INFO "Seeking to %ld position\n", (long)testpos);

    return testpos;
}

static const struct file_operations maxs_cdev_fops = {
	.owner = THIS_MODULE,
	.read = max_chr_drv_read,
	.write = max_chr_drv_write,
	.open = max_chr_drv_open,
	.release = max_chr_drv_release,
    .llseek = max_chr_drv_lseek
};

static int __init init_module_chrdrv(void) {

	printk( KERN_INFO "Hello, solution module loaded!\n" );

	first = MKDEV (max_chr_drv_major, max_chr_drv_minor);
	register_chrdev_region (first, count, MAX_DEV_NAME);

	maxs_cdev = cdev_alloc();

	cdev_init (maxs_cdev, &maxs_cdev_fops);
	cdev_add (maxs_cdev, first, count);

    maxs_class = class_create (THIS_MODULE, "maxs_class");
    device_create (maxs_class, NULL, first, "%s", "chrdrv");

    printk ( KERN_INFO "Created device class :%s\n", MAX_DEV_NAME);

	return 0;
}

static void __exit exit_module_chrdrv(void) {

    device_destroy (maxs_class, first);
    class_destroy (maxs_class);

	if (maxs_cdev)
		cdev_del (maxs_cdev);
	unregister_chrdev_region(first, count);
	printk( KERN_INFO "Hello, solution module leaved!\n" );
}

module_init(init_module_chrdrv);
module_exit(exit_module_chrdrv);


MODULE_LICENSE("GPL");