echo "[>] Starting resetter"
while sleep 1; do
	echo "[+] Waiting for device to show"
	while [ ! -b /dev/sdb ]; do
		echo "[+] Resetting device"
		echo 1 > /sys/bus/pci/devices/0000\:00\:0d.0/remove
		sleep 1
		echo 1 >/sys/bus/pci/rescan
	done
	echo "[+] Device is back"
done
