HOMEPAGE = "http://gitorious.org/open-duckbox-project-sh4"

SRCREV = "a187619deaf19d8419c3a868b78581fbbc3e7898"
# using our own tdt drivers because this has less overhead
SRC_URI = " \
        git://github.com/project-magpie/tdt-driver.git \
        file://0001-player2_191-silence-kmsg-spam.patch;apply=yes \
        file://0002-e2proc-silence-kmsg-spam.patch \
        file://0003-pti-silence-kmsg-spam.patch \
        file://0004-stmfb-silence-kmsg-spam.patch \
        file://0005-frontends-spark_dvbapi5-silence-kmsg-spam.patch \
        file://0006-frontends-spark7162-silence-kmsg-spam.patch;apply=yes \
        file://0001-import-cec-from-pinky-s-git.patch \
        file://0002-aotom-fix-include-file.patch \
        file://0003-aotom-add-VFDGETVERSION-ioctl-to-find-FP-type.patch \
        file://0004-aotom-improve-scrolling-text-code.patch \
        file://0005-aotom-speed-up-softi2c-lowering-CPU-load-of-aotom-dr.patch \
        file://0006-aotom-add-additional-chars-for-VFD-fix-missing-chars.patch \
        file://0007-aotom-register-reboot_notifier-implement-rtc-driver.patch \
        file://tdt-driver-avoid-buildtime.patch \
        file://aotom_udev.rules \
        file://tdt-driver.init \
        file://select_sparkbox.sh \
        file://COPYING \
"

PV = "2.6.32.59-stm24-0211+git${SRCPV}"
STMFB = "0102"
PLAYER2_INCLUDES = "player2_179"

require tdt-driver.inc
SUMMARY = "Driver modules from TDT"
DESCRIPTION = "Driver modules from TDT"

PR = "r2"
