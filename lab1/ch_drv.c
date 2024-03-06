#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>

#define BUF_SIZE 256

static dev_t first;
static struct cdev c_dev;
static struct class * cl;

char ibuf[BUF_SIZE];
char rbuf[BUF_SIZE];
int rbuf_off = 0;

// 0 -> OK; 1 -> END of Buffer; 2 -> Div by zero; 3 -> Invalid op;
int calculate(int a, int b, char operation){
  int answer = 0;
  if(rbuf_off > BUF_SIZE - 3){
    printk(KERN_ERR "UNLUCK HAPPENED!!!! BUFFER IS FULL!\n");
    return 1;
  }
  switch(operation){
    case '+':
      answer = a + b;
      break;
    case '-':
      answer = a - b;
      break;
    case '*':
      answer = a * b;
      break;
    case '/':
      if (b==0){
        printk(KERN_ERR "Division by Zero");
        return 2;
      }
      answer = a / b;
      break;
    default:
      printk(KERN_ERR "Invalid operation!");
      return 3;
  }

  rbuf[rbuf_off] = answer + '0';
  rbuf_off++;
  rbuf[rbuf_off] = ' ';
  rbuf_off++;
  rbuf[rbuf_off] = '\0';
  printk(KERN_ERR "OFFSET IS %d\n", rbuf_off);
  return 0;
}

static int my_open(struct inode *i, struct file *f)
{
  printk(KERN_INFO "Driver: open()\n");
  return 0;
}

static int my_close(struct inode *i, struct file *f)
{
  printk(KERN_INFO "Driver: close()\n");
  return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{

  int count = strlen(rbuf);
  printk(KERN_INFO "Driver: read()\n");

  if (*off > 0 || len < count) {
      return 0;
  }

  if (copy_to_user(buf, rbuf, count) != 0) {
      return -EFAULT;
  }

  *off = count;

  return count;
}

static ssize_t my_write(struct file *f, const char __user *buf,  size_t len, loff_t *off)
{
  printk(KERN_INFO "Driver: write()\n");

  if(len > BUF_SIZE)
      return 0;

  if (copy_from_user(ibuf, buf, len) != 0) {
      return -EFAULT;
  }


  int a;
  int b;
  char op;
  if(sscanf(ibuf, "%d%c%d", &a, &op, &b) !=3){
        printk(KERN_ERR "Invalid format from input!");
        return -EINVAL;
  }

  retry:
  int state_res = calculate(a, b, op);

  switch(state_res){
  case 0:
    break;
  case 1:
    int i = 0;
    while(i < BUF_SIZE){
      rbuf[i] = '\0';
      i++;
    }
    rbuf_off = 0;
    goto retry;
  case 2:
    return -EINVAL;
  case 3:
    return -EINVAL;
  }

  printk(KERN_INFO "Buf is  %s\n", rbuf);
  printk(KERN_INFO "Buf Size is %d\n", rbuf_off);

  return len;
}

static struct file_operations mychdev_fops =
{
  .owner = THIS_MODULE,
  .open = my_open,
  .release = my_close,
  .read = my_read,
  .write = my_write
};

static int __init ch_drv_init(void)
{
    printk(KERN_INFO "Hello!\n");
    if (alloc_chrdev_region(&first, 0, 1, "ch_dev") < 0)
    {
    return -1;
    }
    if ((cl = class_create(THIS_MODULE, "chardrv")) == NULL)
    {
    unregister_chrdev_region(first, 1);
    return -1;
    }

      if (device_create(cl, NULL, first, NULL, "var2") == NULL)
      {
          class_destroy(cl);
          unregister_chrdev_region(first, 1);
          return -1;
      }


    cdev_init(&c_dev, &mychdev_fops);
    if (cdev_add(&c_dev, first, 1) == -1)
    {
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    return -1;
    }
    return 0;
}

static void __exit ch_drv_exit(void)
{
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    printk(KERN_INFO "Bye!!!\n");
}

module_init(ch_drv_init);
module_exit(ch_drv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Author");
MODULE_DESCRIPTION("The first kernel module");
