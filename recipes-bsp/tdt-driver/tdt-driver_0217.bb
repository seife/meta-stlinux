FILESEXTRAPATHS_prepend := "${THISDIR}/tdt-driver-0217:"

HOMEPAGE = "http://github.com/Duckbox-Developers"
KV = "2.6.32.61-stm24-0217"
SRCREV = "74561bfd0d8e8d3c8cab207a2b50dd164027fabf"

SRC_URI = " \
    git://github.com/Duckbox-Developers/driver.git;protocol=git \
    file://aotom_spark_procfs.patch;patch=1 \
    file://fix_videomode_names.patch;patch=1 \
    file://silence_tuner_printk.patch;patch=1 \
    file://silence_stmfb_printk.patch;patch=1 \
    file://silence_stmfb0104_printk.patch;patch=1 \
    file://tdt-driver-avoid-buildtime.patch \
    file://aotom_udev.rules \
    file://tdt-driver.init \
    file://select_sparkbox.sh \
    file://COPYING \
"

PV = "2.6.32.61-stm24-0217+git${SRCPV}"
STMFB = "0104"
PLAYER2_INCLUDES = "player2_191"

require tdt-driver.inc
SUMMARY = "Driver modules from TDT"
DESCRIPTION = "Driver modules from TDT"

PR = "r2"
