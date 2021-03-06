#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <linux/delay.h>
unsigned char *buff;
static char* devname;
module_param(devname,charp,0644);//用户向内核传参数的函数
static int print(char *filename,char *buff){
	struct file* fp;
	mm_segment_t fs;
	loff_t pos;
	fp = filp_open(filename,O_RDWR|O_CREAT,0644);
	if(IS_ERR(fp)){
		printk("open fail");
		return -1;
	}
	fs = get_fs();
	set_fs(KERNEL_DS);
	pos = 0;
	fp->f_op->write(fp,buff,strlen(buff),&fp->f_pos);
	filp_close(fp,NULL);
	set_fs(fs);
	return 0;
}
static int __init
init_file(void)
{
	buff = (unsigned char* )kmalloc(1024*1024,GFP_KERNEL);
	if(buff != NULL)
	{
		printk("allocate 1M address space %p\n",buff);
	}
	else
		printk("error\n");
	memset(buff,'A',1024*1024);
	print(devname,buff);
	return 0;
}
static void __exit
exit_file(void)
{
	kfree(buff);
	printk("GoodBye kernel\n");
}
module_init(init_file);
module_exit(exit_file);
MODULE_LICENSE("GPL");

