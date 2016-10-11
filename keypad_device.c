#include <linux/version.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/ioport.h>
#include <linux/fs.h>
#include <asm/hardware.h>
#include <asm/irq.h>
#include <asm/arch/pxa-regs.h>
#include <linux/delay.h>
#include <linux/signal.h>
#include <asm/uaccess.h>
//#include <string.h>
#include <asm/io.h>
#include <linux/slab.h>
//#include <linux/irq.h>
#include <linux/interrupt.h>

#define KEY_BASE 0x14000000

unsigned long *keyport;
static unsigned char *buffer = NULL;
static unsigned char key_buf = 0;
int check=0;

void key_handler(int irq, void *dev_id, struct pt_regs *regs);

void key_handler(int irq, void *dev_id, struct pt_regs *regs){
  unsigned char key_value;

  keyport = (unsigned long *) ioremap_nocache(KEY_BASE, 0x10);
  key_value = (*(keyport))&0xf;
  printk("KEY INPUT :[%d] input!!!!!\n", key_value);
  key_buf = key_value;
  check = 1;
  iounmap(keyport);
}

int virtual_device_open(struct inode *inode, struct file *filp){
  printk(KERN_ALERT "virtual_device open\n");
  
  return 0;
}

int virtual_device_release(struct inode *inode, struct file *filp){
  printk(KERN_ALERT "virtual_device release\n");
  return 0;
}

ssize_t virtual_device_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos){
  printk(KERN_ALERT "virtual_device write\n");
  strcpy(buffer, buf);

  return count;
}

ssize_t virtual_device_read(struct file *filp, const char *buf, size_t count, loff_t *f_pos){
  //printk(KERN_ALERT "virtual_device read\n");
  
  /* int a = (GPLR1 &= 0x01);
  if(a==1){
    strcpy(buffer, "1");
  }else{
    strcpy(buffer, "0");
    }*/
  if(check == 1){
    if(copy_to_user(buf, &key_buf, count)<0){
      printk("error\n");
    }  
    check = 0;
    return count;
  }else{
    return 0;
  }
  
}

static struct file_operations vd_fops={
  .read = virtual_device_read,
  .write = virtual_device_write,
  .open = virtual_device_open,
  .release = virtual_device_release
};

int __init virtual_device_init(void){
  int return_val;

  if(register_chrdev(250, "keypad_device", &vd_fops)<0)
    printk(KERN_ALERT "driver init failed\n");
  else 
    printk(KERN_ALERT "driver init successful\n");

  buffer = (unsigned char*)kmalloc(16, GFP_KERNEL);
  if(buffer!=NULL)
    memset(buffer, 0, 16);

  GPDR0 &= ~(1<<0);

  set_GPIO_IRQ_edge(0, GPIO_FALLING_EDGE);

  return_val = request_irq(IRQ_GPIO(0), key_handler, SA_INTERRUPT, 
			   "KEY PAD INT", NULL);

  if(return_val<0){
    printk(KERN_ERR "pxa255_pro_key_init() : Can't request irq %#010x\n", 
	   IRQ_GPIO(0));
    return -1;
  }
  return 0;
}

void __exit virtual_device_exit(void){
  unregister_chrdev(250, "keypad_device");
  kfree(buffer);
  free_irq(IRQ_GPIO(0), NULL);
  printk(KERN_ALERT "driver exit successful\n");
}

module_init(virtual_device_init);
module_exit(virtual_device_exit);
MODULE_LICENSE("GPL");
