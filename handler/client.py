import subprocess, requests, uuid, os

# this wrapper runs 'iof' or some other fuzzer
# the fuzzer must take a seed
# the fuzzer should perform ~1000 rounds (so this handler isn't a bottleneck)
# the fuzzer must exit cleanly (don't ruin system state, or break how this works)

serverURL = "http://192.168.0.18:8000"
fuzzerBinary = "/mnt/kevinspacey/ioports/iof"
fuzzIdFilePath = "fuzz.id"

def get_fuzz_id():
	if not os.path.isfile(fuzzIdFilePath):
		newFuzzId = f"FUZZER_{str(uuid.uuid4())}"
		with open(fuzzIdFilePath, "w") as fuzzIdFile:
			fuzzIdFile.write(newFuzzId)
		return newFuzzId
	else:
		with open(fuzzIdFilePath, "r") as fuzzIdFile:
			return fuzzIdFile.read()

def start_fuzz(fuzz_id):
	print("[+] Generating seed")
	seed = uuid.uuid4().int & 0xFFFFFFFFFFFFFFFF
	print(f"[+] Seed: {seed}\n[+] Fuzz: {fuzz_id}")
	requests.get(f"{serverURL}/start_fuzz?fuzz_id={fuzz_id}&seed={seed}")
	# we don't care about return value. if we crash, vm dies
	subprocess.run(["./iof", str(seed)])
	end_fuzz(fuzz_id)

def end_fuzz(fuzz_id):
	requests.get(f"{serverURL}/end_fuzz?fuzz_id={fuzz_id}")

def fuzz_loop():
	print("[+] Getting fuzzer id")
	# use first 64 bits of a UUID as a seed, so it fits in uint64_t
	fuzz_id = get_fuzz_id()
	print(f"[+] Fuzzer id: {fuzz_id}")
	print("[+] Fuzz seed: {seed}")
	print("[+] Entering main fuzz loop")
	while True:
		start_fuzz()
		# only way outta here is to PANIC

if __name__ == "__main__":
	print("[>] Starting fuzzer")
	fuzz_loop()
