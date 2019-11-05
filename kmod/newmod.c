#include <linux/module.h>
#include "fopskit.h"

#include <linux/libata.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("roddux");
MODULE_DESCRIPTION("hackmod");

#define ATA_CMD_FLUSH 0xE7
#define ATA_CMD_FPDMA_READ 0x60
#define ATA_CMD_FPDMA_WRITE 0x61
#define ATA_CMD_PACKET 0xA0

fopskit_hook_handler(ata_qc_issue) {
//	printk("ftr: ata_qc_issue");

	struct ata_queued_cmd *CMD = REGS_ARG1;
//	printk("ata_queued_cmd: %X", &CMD);
	struct ata_taskfile *TF = &CMD->tf;
//	printk("taskfile: %X", &TF);

	switch (TF->command) {
		case ATA_CMD_FLUSH:
			printk("ftr: ata_qc_issue cmd %X (ATA_CMD_FLUSH)",TF->command);
			break;
		case ATA_CMD_FPDMA_WRITE:
			printk("ftr: ata_qc_issue cmd %X (ATA_CMD_FPDMA_WRITE)",TF->command);
			break;
		case ATA_CMD_PACKET:
			printk("ftr: ata_qc_issue cmd %X (ATA_CMD_PACKET)",TF->command);
			break;
		default:
			printk("ftr: ata_qc_issue cmd %X (not defined)",TF->command);
			break;
	}

}


struct fops_hook hook_ata_qc_issue = fops_hook_val(ata_qc_issue);

static int __init test_init(void) {
	printk("ftr: hooking...");
	fopskit_sym_hook(&hook_ata_qc_issue);
	printk("ftr: okay!");
	return 0;
}

static void __exit test_exit(void) {
	printk("ftr: unhooking...");
	fopskit_sym_unhook(&hook_ata_qc_issue);
	printk("ftr: bye!");
}

module_init(test_init);
module_exit(test_exit);
