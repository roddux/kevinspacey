#include <linux/module.h>
#include <linux/random.h>
#include <linux/libata.h>
#include "fopskit.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("roddux");
MODULE_DESCRIPTION("kvsp");

// Most common commands
#define ATA_CMD_FLUSH 0xE7
#define ATA_CMD_FPDMA_READ 0x60
#define ATA_CMD_FPDMA_WRITE 0x61
#define ATA_CMD_PACKET 0xA0
#define ATA_CMD_READ 0xC8
#define ATA_CMD_WRITE 0xCA

void bitflip(void *, unsigned int);

// ata_qc_issue, in our case, calls ahci_qc_issue
// that's what we wanna look at for hardware operations
// also ahci_qc_prep, ahci_fill_cmd_slot ...

// amusingly, ahci_qc_issue takes exactly the same input as ata_qc_issue
// which means we can short-circuit ata_qc_issue
// just use our hook to fuzz the ata_queued_cmd struct, then
// return directly to ahci_qc_issue.

fopskit_hook_handler(ata_qc_issue) {
//	This hook runs before the main ata_qc_issue code does
//	Which means we can fuck with all it's arguments

	struct ata_queued_cmd *CMD = REGS_ARG1;
//	printk("ata_queued_cmd: %X", &CMD);
	struct ata_taskfile *TF = &CMD->tf;
//	printk("taskfile: %X", &TF);

	switch (TF->command) {
		case ATA_CMD_FLUSH:
			printk("kvsp: ata_qc_issue cmd %X (ATA_CMD_FLUSH)",TF->command);
			break;
		case ATA_CMD_FPDMA_READ:
			printk("kvsp: ata_qc_issue cmd %X (ATA_CMD_FPDMA_READ)",TF->command);
			break;
		case ATA_CMD_FPDMA_WRITE:
			printk("kvsp: ata_qc_issue cmd %X (ATA_CMD_FPDMA_WRITE)",TF->command);
			break;
		case ATA_CMD_PACKET:
			printk("kvsp: ata_qc_issue cmd %X (ATA_CMD_PACKET)",TF->command);
			break;
		case ATA_CMD_READ:
			printk("kvsp: ata_qc_issue cmd %X (ATA_CMD_READ)",TF->command);
			break;
		case ATA_CMD_WRITE:
			printk("kvsp: ata_qc_issue cmd %X (ATA_CMD_WRITE)",TF->command);
			break;
		default:
			printk("kvsp: ata_qc_issue cmd %X (not defined)",TF->command);
			break;
	}

	//struct ata_queued_cmd CMDBU;
	//CMDBU = *CMD;

	unsigned int *(*ahciqci_p)(struct ata_queued_cmd *qc);
	ahciqci_p = kallsyms_lookup_name("ahci_qc_issue");

	unsigned short X;
	get_random_bytes(&X, sizeof(X));
	if(X%3 == 0) {
		unsigned short i;
		//for(i=0; i<100; i++) {
			//bitflip(CMD, sizeof(struct ata_queued_cmd));
			bitflip(TF, sizeof(struct ata_taskfile));
			//ahciqci_p(CMD);
			printk("kvsp: issued fuzzed taskfile");
		//}
	}
	//unsigned short X;
	//get_random_bytes(&X, sizeof(X));
	//if(X%3 == 0) {
	//	printk("kvsp: flipping a bit");
	//	bitflip(CMD, sizeof(struct ata_queued_cmd));
	//}
	// TF->command = ATA_CMD_FPDMA_READ;
	// this kills the crab^H^H^H^Hvm
}

// TODO: Remove the fopskit helper functions and use ftrace directly (?)
struct fops_hook hook_ata_qc_issue = fops_hook_val(ata_qc_issue);

static int __init test_init(void) {
	printk("kvsp: hooking...");
	fopskit_sym_hook(&hook_ata_qc_issue);
	printk("kvsp: okay!");
	return 0;
}

static void __exit test_exit(void) {
	printk("kvsp: unhooking...");
	fopskit_sym_unhook(&hook_ata_qc_issue);
	printk("kvsp: bye!");
}

module_init(test_init);
module_exit(test_exit);
