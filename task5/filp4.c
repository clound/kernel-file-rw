#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <asm/div64.h>
unsigned char *buff;
static char* devname;
struct timeval tv;
struct timeval start_time,end_time;
module_param(devname,charp,0644);//用户向内核传参数的函数

struct timeval my_function(void){//计算时间
	printk("Hello World!\n");
	do_gettimeofday(&tv);
	return tv;	
}
static int fileread(const char *filename)
{	
	struct file *fp;
	mm_segment_t fs;
	loff_t pos;
	long count=0;
	int fsize=1024*64;
	printk("<1>start...\n");
	fp = filp_open(filename,O_RDONLY,0);//打开文件
	buff = (char *)kmalloc(fsize,GFP_ATOMIC);
	fs = get_fs();
	set_fs(KERNEL_DS);//获得内核态
	pos = 0;
	for(count = 0;count < 32*1024;count++)
		vfs_read(fp,buff,fsize,&pos);//读取文件
		//fp->f_op->read(fp,buff,fsize,&(fp->f_pos));//第二种方法，也是vfs_read实际会调用的函数
	set_fs(fs);//恢复用户态
	buff[fsize] = '\0';
	printk("<1>buffer is %s\n",buff);
	filp_close(fp,NULL);//关文件
	return 0;
}	
static int __init
init_file(void)
{
	long minus,mul=0;	
	start_time = my_function();
	fileread(devname);
	end_time = my_function();
	if(start_time.tv_sec == end_time.tv_sec)//判断超出秒数
		minus = (long)(end_time.tv_usec - start_time.tv_usec);
	else{
		minus = (long)((end_time.tv_sec - start_time.tv_sec)*1000000 + end_time.tv_usec - start_time.tv_usec);		
	}		
	mul = 2*1024*1024;
	do_div(mul,(minus/1000));
	printk("The rate is %ld kb/ms,minus %ld\n",mul,minus);//计算得到速率
	return 0;
}
static void __exit
exit_file(void)
{
	kfree(buff);//释放buff空间
	printk("GoodBye kernel\n");
}
module_init(init_file);
module_exit(exit_file);
MODULE_LICENSE("GPL");

