import requests, uuid, os

# this wrapper runs 'iof' or some other fuzzer
# the fuzzer must take a seed
# the fuzzer should perform ~1000 rounds or so
# the fuzzer must exit cleanly

serverURL = "http://localhost:8000"
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

def start_fuzz():
	# take first 32 bits of UUID as a seed
	seed = uuid.uuid4().int & 0xFFFFFFFF 
	fuzz_id = get_fuzz_id()
	print(f"Seed:{seed}\nFuzz:{fuzz_id}\n")
	#requests.get(f"{serverURL}/start_fuzz?fuzz_id={fuzz_id}&seed={seed}")
	# we don't care about return value. if we crash, vm dies
	#subprocess.run("./iof.c ")
	#end_fuzz()

# def end_fuzz():
#	requests.get(f"{serverURL}/end_fuzz?fuzz_id=<>")
