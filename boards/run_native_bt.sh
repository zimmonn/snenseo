#!/usr/bin/env bash
set -euo pipefail

# Directory of this script (assumes build dir is ../build/zephyr)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ZEPHYR_EXE="${SCRIPT_DIR}/../build/zephyr/zephyr.exe"

if [[ ! -x "$ZEPHYR_EXE" ]]; then
    echo "Error: zephyr.exe not found at: $ZEPHYR_EXE" >&2
    exit 1
fi

HCI_DEV="${HCI_DEV:-hci0}"

echo "[*] Stopping bluetooth service..."
sudo systemctl stop bluetooth || true

echo "[*] Bringing ${HCI_DEV} down..."
if hciconfig "${HCI_DEV}" >/dev/null 2>&1; then
    sudo hciconfig "${HCI_DEV}" down || true
else
    echo "Warning: ${HCI_DEV} not found, continuing anyway." >&2
fi

echo "[*] Starting Zephyr with ${HCI_DEV}..."
# Use sudo to keep same privileges as your manual command
sudo "${ZEPHYR_EXE}" --bt-dev="${HCI_DEV}"
