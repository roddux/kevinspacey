# KevinSpacey : IO port fuzzer
Virtual(Box): https://youtu.be/INtzPvAmNZ8

## Running
`$ make; ./fuzz.sh`

## Speedy VM guide
- `$ VBoxManage setextradata Arch GUI/GuruMeditationHandler PowerOff`
- `$ VBoxManage modifyvm Arch --bioslogodisplaytime 1`
- `$ VBoxManage modifyvm Arch --bioslogofadein off`
- `$ VBoxManage modifyvm Arch --bioslogofadeout off`
- Have your distro auto-login
	- modify `/etc/systemd/system/getty.target.wants/getty\@tty1.service`
	- `ExecStart=-/sbin/agetty --autologin root --noclear %I $TERM`
- Edit `.bashrc` to auto-start the fuzzer on boot, after auto-login
- Remove unneeded services from startup
- Remove GRUB boot timeout
	- modify `/etc/default/grub`
	- `GRUB_DEFAULT=0, GRUB_TIMEOUT=0, GRUB_HIDDEN_TIMEOUT=1, GRUB_HIDDEN_TIMEOUT_QUIET=true`
	- regenerate grub.cfg

## TODO
Extending the fuzzer

- Create a network service that:
	- logs the seed and port ranges for each run
	- detects crashes
- instrument the fuzzer to interact with the service
- have the host continually starting the fuzzer in an infinite loop
- investigate kernel drivers and DMA

### crash detection
#### fuzzer modifications:
- fuzzing runs will be about 1000 iterations
- fuzzing port/s will be static for each run
- fuzzer will tell network service the start and end of each run
#### network service:
- log all fuzzer runs
- if a run is started, then another run is started -- it means the first run did not finish, indicating a crash
- save the ports and data for said run

#### modifications/setup needed
- the network service machine needs to be setup to use a static IP
- fuzzer machine needs to setup network on boot or before fuzzing
- `dhcpcd enp3s0 -4 -A in fuzz.sh?`

#### network service using hug.rest:
- `/api?action=start_run&seed=RANDOM_SEED&ports=0x0f1,0x0f3,0x03f,0x0d1`
- `/api?action=end_run&seed=RANDOM_SEED`

RANDOM_SEED can act as an identifier for each run, because we probably won't collide them

##### fuzzer modifications
- use getopt+argc to take an optional RANDOM_SEED and a list of port/s to fuzz
- a python wrapper, using the requests library
- wrapper can chose the ports to fuzz
- fuzz special port values from https://github.com/chipsec/chipsec/blob/master/chipsec/modules/tools/vmm/iofuzz.py

----
### future:
#### auto-poc generator
- fuzzer-wrapper will be modified to generate PoCs based on a RANDOM_SEED supplied
- the network service will task fuzzer-wrappers to generate PoCs for identified crashes
- fuzzer wrapper will be modified to ask the server if it should verify a POC, or fuzz
	- `/api?action=get_task`
		- `task=verify_poc&seed=RANDOM_SEED`
		- `task=fuzz`
	- fuzz just fuzzes normally
- verify_poc will then generate, compile and run a PoC for the given RANDOM_SEED
	- `/api?action=verify_poc_start&seed=RANDOM_SEED`
	- again we use RANDOM_SEED as the unique identifier
- fuzzer then runs the PoC
- if the PoC successfully crashes the VM, the onus is on the network service to detect this
- if the PoC does not crash, the fuzzer then calls `/api?action=verify_poc_fail&seed=RANDOM_SEED`
- network service can detect crashes with the lack of this response

