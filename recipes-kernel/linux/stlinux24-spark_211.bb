require linux-stm.inc

# INC_PR is defined in the .inc file if something has change here just increase the number after the dot
PR = "${INC_PR}"

LINUX_VERSION = "2.6.32.59"
PV = "${LINUX_VERSION}-stm24-0211"
#SRCREV = "3bce06ff873fb5098c8cd21f1d0e8d62c00a4903"
SRCREV = "${AUTOREV}"

SRC_URI = "git://github.com/project-magpie/linux-sh4-2.6.32.y.git;protocol=https;branch=${KBRANCH} \
           file://defconfig_spark7111 \
           file://defconfig_spark7162 \
           file://st-coprocessor.h \
           file://linux-sh4-spark7162_setup_stm24_0211-magpie.patch \
           file://console.map.c-workaround-for-gcc-4.8.2-build.patch \
"

DEPENDS += " \
	stlinux24-sh4-stx7111-fdma-firmware \
	stlinux24-sh4-stx7105-fdma-firmware \
"
