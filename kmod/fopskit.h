// Hacked together from tpe-lkm by github.com/cormander
#include <linux/ftrace.h>
#include <linux/stop_machine.h>
#include <linux/slab.h>

// Only support X86-64
#define REGS_ARG1 regs->di
#define REGS_ARG2 regs->si
#define REGS_ARG3 regs->dx

#if !defined(CONFIG_SECURITY) || !defined(CONFIG_FUNCTION_TRACER)
#error "This module requires both CONFIG_SECURITY and CONFIG_FUNCTION_TRACER kernel options to be enabled"
#endif

struct fops_hook {
	char *name;
	void *addr;
	bool found;
	bool hooked;
	struct ftrace_ops *fops;
};

int fopskit_sym_hook(struct fops_hook *);
int fopskit_sym_unhook(struct fops_hook *);
int fopskit_sym_int(char *);
char *fopskit_sym_str(char *);
void *fopskit_sym_ptr(char *);

#define fops_hook_val(val) {#val, NULL, false, false, &fops_##val}

#define fopskit_hook_handler(val) \
	static void notrace fopskit_##val(unsigned long, unsigned long, struct ftrace_ops *, struct pt_regs *); \
	static struct ftrace_ops fops_##val __read_mostly = { .func = fopskit_##val, .flags = FTRACE_OPS_FL_SAVE_REGS | FTRACE_OPS_FL_IPMODIFY }; \
	static void notrace fopskit_##val(unsigned long ip, unsigned long parent_ip, struct ftrace_ops *fops, struct pt_regs *regs)

#define fops_hook_error(func, ret, fops) printk("fopskit: %s() failed with return code %d for fops_hook { name => %s, addr => %lx, found => %d, hooked => %d } at %s() line %d\n", \
	func, ret, fops->name, (unsigned long)fops->addr, fops->found, fops->hooked, __FUNCTION__, __LINE__)
