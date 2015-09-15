#include <linux/module.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include<linux/semaphore.h>
#define DriverMain_MAJOR 234

static int debug_enable = 1;
module_param(debug_enable, int, 0);
MODULE_PARM_DESC(debug_enable, "Enable module debug mode.");
struct kmem_cache *drivermain_cache;
static int len;
char *dirname="examples";
struct proc_dir_entry *parent;
static struct node{
char data;
struct node* link;
};

struct semaphore sem_name;
static struct node* start;
static struct node* cur;
struct file_operations drivermain_fops;

static int drivermain_open(struct inode *inode, struct file *file)
{
    printk("drivermain_open: successful\n");
    return 0;
}

static int drivermain_release(struct inode *inode, struct file *file)
{
    printk("drivermain_release: successful\n");
    return 0;
}

static ssize_t drivermain_read(struct file *file, char *buffer, size_t count,
               loff_t *ptr)
{	int i = len-1;
	int p=len;
	down(&sem_name);
	
    if(start != NULL){
	struct node* n;
	n = cur;
	do{
		
		*(buffer+i) = n->data;
		cur = n;
		n = n->link;
		kmem_cache_free(drivermain_cache,cur);
		i--;
	}while(n != NULL);

	}else{
		printk("Buffer contains no element");
	}
	len = 0;
     start = NULL;
    cur = NULL;
    	up(&sem_name);
    return p;
}

int drivermain_readproc(char *buffer, char **start, off_t offset,int count,int eof, void *data)
{
    int r;
    r=sprintf(buffer+r,"drivermain: length of buffer = %i \n",len);
    return r;
}

static ssize_t drivermain_write(struct file *file, const char *buffer,
               size_t count, loff_t * ppos)
{	
	int i=0;
        down(&sem_name);
	for(i;i<count; i++){
	if(start == NULL)
        {
	len++;
	start = kmem_cache_alloc(drivermain_cache,GFP_KERNEL);
	start->link = NULL;
	start->data = *(buffer+i);
	cur = start;
	}
        else
        {
	
	struct node* new;
	new = kmem_cache_alloc(drivermain_cache,GFP_KERNEL);
        if(new != NULL)
        {
         len++;
	new->link = cur;
	new->data = *(buffer+i);
	cur = new;
	}
        new = NULL;
	}
}
	 up(&sem_name);
    return len;
}

static int drivermain_ioctl(struct file *file,
               unsigned int cmd, unsigned long arg)
{
    printk("drivermain_ioctl: cmd=%ld, arg=%ld\n", cmd, arg);
    return 0;
}

static int __init drivermain_init(void)
{
    sema_init(&sem_name, 1);
    int ret;
    printk("DriverMain Example Init - debug mode is %s\n",
           debug_enable ? "enabled" : "disabled");
    ret = register_chrdev(DriverMain_MAJOR, "drivermain", &drivermain_fops);
    drivermain_cache =  kmem_cache_create("DriverMain_CACHE",sizeof(struct node),SLAB_HWCACHE_ALIGN ,GFP_KERNEL,NULL);
        if (ret < 0) {
            printk("Error registering chrd device\n");
            goto drivermain_fail1;
        }
    printk("DriverMain: Module has registered successfully!!\n");
    /*Creating read proc entry*/
    parent=proc_mkdir(dirname,NULL);
    create_proc_read_entry("drivermain",0,parent,drivermain_readproc,NULL);

    /* Init processing here... */
    

    return 0;

drivermain_fail1:
    return ret;	
}

static void __exit drivermain_exit(void)
{
    kmem_cache_destroy (drivermain_cache);
    unregister_chrdev(DriverMain_MAJOR, "drivermain");
    remove_proc_entry("drivermain",NULL);
    printk("DriverMain Program Exit\n");
}

struct file_operations drivermain_fops = {
    owner:   THIS_MODULE,
    read:    drivermain_read,
    write:   drivermain_write,
unlocked_ioctl:   drivermain_ioctl,
    open:    drivermain_open,
    release: drivermain_release,
};

module_init(drivermain_init);
module_exit(drivermain_exit);

