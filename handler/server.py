# hug -f ./server.py
# waits for start_fuzz?fuzz_id=FUZID&seed=A
# if the same fuzz_id starts a new fuzz, the prior has crashed -- we log the seed
# if not, just scrap the previous seed and continue

# live reload development:
# while true; do inotifywait server.py; kill `pgrep -f "hug -f ./server.py"`; hug -f ./server.py& done
import hug

fuzzers = {}
crashes = []

@hug.get()
def start_fuzz(fuzz_id, seed):
	if fuzz_id in fuzzers:
		crashes.append(fuzzers[fuzz_id]["seed"])
	# we ignore duplicate seeds because it's incredibly unlikely
	fuzzers[fuzz_id] = {"seed":seed}
	return {"message":f"start_fuzz({fuzz_id},{seed})"}

@hug.get()
def end_fuzz(fuzz_id):
	del fuzzers[fuzz_id]
	return {"message":f"end_fuzz({fuzz_id})"}

@hug.get()
def get_fuzzers():
	return fuzzers

@hug.get()
def get_crashes():
	return crashes
