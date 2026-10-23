#!/bin/sh
set -e
echo "=== Installing PoseaAppFilter (Layer 7 Application Control) for pfSense ==="
if [ "$(id -u)" -ne 0 ]; then
    echo "[!] Error: Run as root."
    exit 1
fi
SCRIPT_DIR="$(cd "$(dirname "$0")/.." 2>/dev/null && pwd)"
if [ ! -d "${SCRIPT_DIR}/pfsense-pkg" ]; then
    TMP_DIR="/tmp/poseaapp_install"
    rm -rf "${TMP_DIR}"
    mkdir -p "${TMP_DIR}"
    fetch -q -o "${TMP_DIR}/main.tar.gz" "https://github.com/bysinbin/pfsense-poseaappfilter/archive/refs/heads/main.tar.gz"
    tar -xzf "${TMP_DIR}/main.tar.gz" -C "${TMP_DIR}"
    SCRIPT_DIR="${TMP_DIR}/pfsense-poseaappfilter-main"
fi

mkdir -p /usr/local/pkg /usr/local/www /var/etc/poseafilter
cp "${SCRIPT_DIR}/pfsense-pkg/usr/local/pkg/poseaapp.inc" /usr/local/pkg/poseaapp.inc
cp "${SCRIPT_DIR}/pfsense-pkg/usr/local/pkg/poseaappfilter.xml" /usr/local/pkg/poseaappfilter.xml
cp "${SCRIPT_DIR}/pfsense-pkg/usr/local/www/poseaapp.php" /usr/local/www/poseaapp.php
cp "${SCRIPT_DIR}/pfsense-pkg/usr/local/www/poseaappfilter.php" /usr/local/www/poseaappfilter.php

php -r '
require_once("/etc/inc/config.inc");
global $config;
$found = false;
foreach ($config["installedpackages"]["package"] ?? [] as $p) {
    if (($p["name"] ?? "") === "poseaappfilter") { $found = true; break; }
}
if (!$found) {
    $config["installedpackages"]["package"][] = [
        "name" => "poseaappfilter",
        "descr" => "PoseaApp Layer 7 Application Control for pfSense",
        "website" => "https://github.com/bysinbin/pfsense-poseaappfilter",
        "version" => "1.0.0",
        "status" => "Stable",
        "configurationfile" => "poseaappfilter.xml"
    ];
}
$mfound = false;
foreach ($config["installedpackages"]["menu"] ?? [] as $m) {
    if (($m["name"] ?? "") === "PoseaApp (L7 Filtre)") { $mfound = true; break; }
}
if (!$mfound) {
    $config["installedpackages"]["menu"][] = [
        "name" => "PoseaApp (L7 Filtre)",
        "section" => "Services",
        "url" => "/poseaapp.php"
    ];
}
write_config("Installed PoseaAppFilter package");
'
echo "[+] PoseaAppFilter successfully installed!"
