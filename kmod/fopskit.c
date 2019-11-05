// Hacked together from tpe-lkm by github.com/cormander
#include "fopskit.h"
#define IS_ERROR(x) (x<0)

/* callback for fopskit_find_sym_addr */
static int __init fopskit_find_sym_callback(struct fops_hook *hook, const char *name, struct module *mod, unsigned long addr) {
	if (hook->found) return 1;

	/* this symbol was found. the next callback will be the address of the next symbol */
	if (name && hook->name && !strcmp(name, hook->name)) {
		hook->addr = (unsigned long *)addr;
		hook->found = true;
	}

	return 0;
}

/* find this symbol */
static int __init fopskit_find_sym_addr(struct fops_hook *hook) {
	hook->found = false;

	if (!kallsyms_on_each_symbol((void *)fopskit_find_sym_callback, hook)) {
		fops_hook_error("fopskit_find_sym_addr", -EFAULT, hook);
		return -EFAULT;
	}

	return 0;
}

/* hook this symbol */
int __init fopskit_sym_hook(struct fops_hook *hook) {
	int ret;
	ret = fopskit_find_sym_addr(hook);
	if (IS_ERROR(ret)) return ret;

	preempt_disable_notrace();

	ret = ftrace_set_filter_ip(hook->fops, (unsigned long) hook->addr, 0, 0);
	if (IS_ERROR(ret)) {
		fops_hook_error("ftrace_set_filter_ip", ret, hook);
		return ret;
	}

	ret = register_ftrace_function(hook->fops);
	if (IS_ERROR(ret)) {
		fops_hook_error("register_ftrace_function", ret, hook);
		return ret;
	}

	hook->hooked = true;

	preempt_enable_notrace();

	return 0;
}

/* unhook this symbol */
int fopskit_sym_unhook(struct fops_hook *hook) {
	int ret;

	if (false == hook->hooked) {
		printk("fopskit: unhook called for non-hooked function");
		return 0;
	}

	// preempt_disable_notrace(); #YOLO
	ret = unregister_ftrace_function(hook->fops);
	if (IS_ERROR(ret)) {
		fops_hook_error("unregister_ftrace_function", ret, hook);
		return ret;
	}
	ret = ftrace_set_filter_ip(hook->fops, (unsigned long) hook->addr, 1, 0);
	if (IS_ERROR(ret)) {
		fops_hook_error("ftrace_set_filter_ip", ret, hook);
		return ret;
	}
	// preempt_enable_notrace(); #You Only YOLO Once

	hook->hooked = false;

	return 0;
}

/* find int value of this symbol */
int __init fopskit_sym_int(char *name) {
	static struct ftrace_ops fops_int;
	struct fops_hook hook_int = {name, NULL, false, false, &fops_int};
	int ret;

	ret = fopskit_find_sym_addr(&hook_int);
	if (IS_ERROR(ret)) return -EFAULT;

	return *((int *)hook_int.addr);
}

/* find string value of this symbol */
char __init *fopskit_sym_str(char *name) {
	static struct ftrace_ops fops_str;
	struct fops_hook hook_str = {name, NULL, false, false, &fops_str};
	int ret;

	ret = fopskit_find_sym_addr(&hook_str);
	if (IS_ERROR(ret)) return '\0';

	return (char *)hook_str.addr;
}

/* find the address of this symbol */
void __init *fopskit_sym_ptr(char *name) {
	static struct ftrace_ops fops_ptr;
	struct fops_hook hook_ptr = {name, NULL, false, false, &fops_ptr};
	int ret;

	ret = fopskit_find_sym_addr(&hook_ptr);
	if (IS_ERROR(ret)) return NULL;

	return hook_ptr.addr;
}
