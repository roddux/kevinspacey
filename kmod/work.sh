echo "[>] Starting fuzzer"

function readfuzz {
	echo "[+] Reading from device"
	for _ in {1..10}; do
		cat /dev/sdb &>/dev/null
	done
}

function writefuzz {
	echo "[+] Writing to device"
	for _ in {1..10}; do
		shred /dev/sdb
	done
}

function miscfuzz {
	echo "[+] Running buttpussy"
	#TODO: hardcoded
	/mnt/buttpussy/handler/fuzzer-newest /dev/sdb $RANDOM
}

export -f readfuzz
export -f writefuzz
export -f miscfuzz

while :; do
	echo "[+] Waiting for device to show"
	while sleep 1; do
		echo -n "."
		if [ -b /dev/sdb ]; then
			echo "ok"
			break
		else
			echo "waiting"
		fi
	done

	echo "[+] Device is back"

	let RC=$RANDOM%3;
	if [ $RC -eq 0 ]; then
		echo "[+] Readfuzz"
		timeout 5s bash -c readfuzz
	elif [ $RC -eq 1 ]; then
		echo "[+] Miscfuzz"
		timeout 5s bash -c miscfuzz
	else 
		echo "[+] Writefuzz"
		timeout 5s bash -c writefuzz 
	fi
done
