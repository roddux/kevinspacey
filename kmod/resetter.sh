echo "[>] Starting fuzzer"

while :; do

echo "[+] Waiting for device to show"
cont=0
while [ $cont -eq 0 ]; do
	if [ $cont -eq 0 ]; then
		echo "[+] Resetting device"
		echo 1 > /sys/bus/pci/devices/0000\:00\:0d.0/remove
		sleep 1
		echo 1 >/sys/bus/pci/rescan
	fi
	for _ in {1..5}; do
		if [ $cont -eq 0 ]; then
			echo -n "."
			sleep 1
			if [ -b /dev/sdb ]; then
				echo "ok"
				cont=1
				break
			else
				echo "waiting"
			fi
		else
			break
		fi
	done
done

echo "[+] It's back!"

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

let RC=$RANDOM%2;
if [ $RC -eq 1 ]; then
	echo "[+] Read then pussy then write"
	timeout 5s bash -c readfuzz
	timeout 5s bash -c miscfuzz
	timeout 5s bash -c writefuzz 
else
	echo "[+] Write then pussy then read"
	timeout 5s bash -c writefuzz 
	timeout 5s bash -c miscfuzz
	timeout 5s bash -c readfuzz
fi

done
