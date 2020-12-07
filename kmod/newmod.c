#include <linux/module.h>
#include <linux/random.h>
#include <linux/libata.h>
#include "fopskit.h"
#include "ahci_l.h"
#include <scsi/scsi_cmnd.h>

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

#define ATA_XXX 0xEC
#define ATA_YYY 0xEF

void bitflip(void *, unsigned int);

// used to hijack with ftrace after we've done shit
unsigned int noret(struct ata_queued_cmd *qc){return 0;}

fopskit_hook_handler(ata_tf_to_fis) {
	const struct ata_taskfile *tf = (const struct ata_taskfile *) REGS_ARG1;
	unsigned char pmp = (unsigned char) REGS_ARG2;
	int is_cmd = (int) REGS_ARG3;
	unsigned char *fis = (unsigned char *) REGS_ARG4;

//	printk("kvsp: ata_tf_to_fis, fis is at %X? (%x)\n", &fis, fis[0]);

	// Get pointer to the original command, so we can run it afterwards
	unsigned int *(*atatf2fis_p) (
		const struct ata_taskfile *tf,
		unsigned char pmp,
		int is_cmd,
		unsigned char *fis
	);

	// Orig function pointer
	atatf2fis_p = kallsyms_lookup_name("ata_tf_to_fis");
//	printk("kvsp: running ata_tf_to_fis\n");
	// run it first, then fuzz data after
	atatf2fis_p(REGS_ARG1, REGS_ARG2, REGS_ARG3, REGS_ARG4);

	// Fuzz 20% of the time ¯\_(^-^)_/¯
	unsigned short X;
	get_random_bytes(&X, sizeof(X));
	if(X%5 == 0) {
		bitflip(fis, 20); // fis[0] to fis[19]
	}
	fis[0] = 0x27; // still a h2d fis

//	printk("kvsp: now fuzzing fis\n");
}

//fopskit_hook_handler(ata_qc_issue) {
fopskit_hook_handler(ahci_qc_issue) {
	int dontfuzz = 0;
	struct ata_queued_cmd *CMD = (struct ata_queued_cmd*)REGS_ARG1;
	struct ata_taskfile *TF = &CMD->tf;

#if 0
	switch (TF->command) {
		case ATA_CMD_FLUSH:
			printk("kvsp: ahci_qc_issue cmd %X (ATA_CMD_FLUSH)",TF->command);
			break;
		case ATA_CMD_FPDMA_READ:
			printk("kvsp: ahci_qc_issue cmd %X (ATA_CMD_FPDMA_READ)",TF->command);
			break;
		case ATA_CMD_FPDMA_WRITE:
			printk("kvsp: ahci_qc_issue cmd %X (ATA_CMD_FPDMA_WRITE)",TF->command);
			break;
		case ATA_CMD_PACKET:
			printk("kvsp: ahci_qc_issue cmd %X (ATA_CMD_PACKET)",TF->command);
			break;
		case ATA_CMD_READ:
			printk("kvsp: ahci_qc_issue cmd %X (ATA_CMD_READ)",TF->command);
			break;
		case ATA_CMD_WRITE:
			printk("kvsp: ahci_qc_issue cmd %X (ATA_CMD_WRITE)",TF->command);
			break;
		case ATA_XXX: // skip some known-breaking ones
		case ATA_YYY:
			printk("kvsp: ahci_qc_issue cmd %X (ATA_XXX/YYY)",TF->command);
			dontfuzz = 1;
			break;
		default:
			printk("kvsp: ahci_qc_issue cmd %X (not defined)",TF->command);
			break;
	}
#endif

	struct ata_queued_cmd CMDBU;
	CMDBU = *CMD;

	// hey lets try and fuzz ahci_fill_cmd_slot

	// Get pointer to the original command, so we can run it afterwards
	unsigned int *(*ahciqci_p)(struct ata_queued_cmd *qc);
	ahciqci_p = kallsyms_lookup_name("ahci_qc_issue");

	if (dontfuzz) {
		return ahciqci_p(CMD);
	}

	// Fuzz 30% of the time ¯\_(^-^)_/¯
	unsigned short X;
	get_random_bytes(&X, sizeof(X));
	if(X%3 == 0) {
		unsigned short i;
		for(i=0; i<(X%10); i++) { // Flip up to 10 bytes ¯\_(^-^)_/¯
			bitflip(TF,  sizeof(struct ata_taskfile));
		}
//		printk("kvsp: pre-issue fuzz taskfile");
		return ahciqci_p(CMD);
//		printk("kvsp: issued fuzzed taskfile");
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

// It's safe to hook these AHCI funcs, cause our VM boot disk is IDE 8^)
struct fops_hook hook_ahci_qc_issue = fops_hook_val(ahci_qc_issue);
struct fops_hook hook_ata_tf_to_fis = fops_hook_val(ata_tf_to_fis);

static int __init test_init(void) {
	printk("kvsp: hooking...");
	fopskit_sym_hook(&hook_ahci_qc_issue);
	fopskit_sym_hook(&hook_ata_tf_to_fis);
	printk("kvsp: okay!");
	return 0;
}

static void __exit test_exit(void) {
	printk("kvsp: unhooking...");
	fopskit_sym_unhook(&hook_ahci_qc_issue);
	fopskit_sym_unhook(&hook_ata_tf_to_fis);
	printk("kvsp: bye!");
}

module_init(test_init);
module_exit(test_exit);
