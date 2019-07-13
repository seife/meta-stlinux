FILESEXTRAPATHS_prepend := "${THISDIR}/tdt-driver-0217:"

HOMEPAGE = "http://github.com/Duckbox-Developers"
KV = "2.6.32.61-stm24-0217"
SRCREV = "467d71a3d9d27a2762ff80487cf0eeeefb4a9f97"

SRC_URI = " \
    git://github.com/Duckbox-Developers/driver.git;protocol=http \
    file://aotom_spark_procfs.patch;patch=1 \
    file://silence_stmerger_printk.patch;patch=1 \
    file://ddt-driver-avoid-buildtime.patch \
    file://ddt-driver-bpamem-auto-deallocate.patch \
    file://ddt-driver-backward-compat.patch;patch=1 \
    file://ddt-driver-silence-dvbdemux.patch;patch=1 \
    file://ddt-driver-silence-misc-drivers.patch;patch=1 \
    file://0001-spark_dvbapi5-stv090x-fix-up-GM990-ix7306-tuning.patch \
    file://bpamem-new-toolchain-workaround.diff \
    file://aotom_udev.rules \
    file://tdt-driver.init \
    file://select_sparkbox.sh \
    file://COPYING \
"

PV = "2.6.32.61-stm24-0217+git${SRCPV}"

require tdt-driver.inc
SUMMARY = "Driver modules from DDT"
DESCRIPTION = "Driver modules from DDT"

PR = "r7"
