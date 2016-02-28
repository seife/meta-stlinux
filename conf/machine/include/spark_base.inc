require conf/machine/include/tune-sh4.inc
TARGET_ARCH = "sh4"
STLINUX_SH_UPD_SRPMS = "http://ftp.stlinux.com/pub/stlinux/2.4/updates/SRPMS"
STLINUX_SH_UPD_RPMS = "http://ftp.stlinux.com/pub/stlinux/2.4/updates/RPMS/sh4/"

STM_PATCH_STR ??= "0217"
# needed to select proper .bb file
STM_LINUX_VERSION ??= "2.6.32.61"

# if you want to use the old crap, put this into you conf/local.conf
# STM_PATCH_STR = "0211"
# STM_LINUX_VERSION = "2.6.32.59"

PREFERRED_VERSION_stlinux24-spark ?= "${STM_LINUX_VERSION}-stm24-${STM_PATCH_STR}"
# MACHINE=spark7162 is no longer needed nor really supported... but keep it anyway.
PREFERRED_VERSION_stlinux24-spark7162 ?= "${STM_LINUX_VERSION}-stm24-${STM_PATCH_STR}"
PREFERRED_VERSION_tdt-driver = "${STM_LINUX_VERSION}-stm24-${STM_PATCH_STR}%"
PREFERRED_VERSION_linux-libc-headers = "2.6.32"
PREFERRED_PROVIDER_virtual/kernel = "stlinux24-${MACHINE}"
PREFERRED_PROVIDER_openssl = "openssl"
PREFERRED_PROVIDER_libcrypto = "openssl"
PREFERRED_PROVIDER_libssl = "openssl"
#PREFERRED_PROVIDER_virtual/nativesdk-libc = "nativesdk-glibc"
#PREFERRED_PROVIDER_virtual/nativesdk-libiconv = "nativesdk-libiconv"
#PREFERRED_PROVIDER_virtual/nativesdk-libintl = "nativesdk-gettext"
#PREFERRED_VERSION_glibc = "2.10"
#PREFERRED_VERSION_glibc                   ?= "${GLIBCVERSION}"
#PREFERRED_VERSION_glibc-locale            ?= "${GLIBCVERSION}"
#PREFERRED_VERSION_glibc-nativesdk         ?= "2.10"
#PREFERRED_VERSION_glibc-initial           ?= "${GLIBCVERSION}"
#PREFERRED_VERSION_glibc-initial-nativesdk ?= "2.10"

USE_VT = "0"
MACHINE_FEATURES = "kernel26 ext2 alsa ext3 usbhost"

SERIAL_CONSOLE = "115200 ttyAS0"

KERNEL_IMAGETYPE = "uImage"

# save some extra space, kernel has its own mtd partion 
RDEPENDS_kernel-base = ""

UBI_VOLNAME = "rootfs"
MKUBIFS_ARGS = "-m 2048 -e 129024 -c 638   -F"
UBINIZE_ARGS = "-m 2048 -p 128KiB -O 512"

JFFS2_ERASEBLOCK = "0x20000"
JFFS2_ENDIANNESS = "${@base_conditional('SITEINFO_ENDIANNESS', 'le', '--little-endian', '--big-endian', d)}"
EXTRA_IMAGECMD_jffs2 = "--pad ${JFFS2_ENDIANNESS} --eraseblock=${JFFS2_ERASEBLOCK} --no-cleanmarkers"

JFFS2_SUM_EXTRA_ARGS = " \
	${@base_conditional('SITEINFO_ENDIANNESS', 'le', '-l', '-b', d)} \
	--pad --eraseblock=${JFFS2_ERASEBLOCK} --no-cleanmarkers \
"

INHERIT += "usb_image-spark71xx"
# IMAGE_FSTYPES ?= "tar.gz ubi ${SDIMG_ROOTFS_TYPE} jffs2.sum spark71xx-usbimg"
IMAGE_FSTYPES ?= "tar.gz ${SDIMG_ROOTFS_TYPE} jffs2.sum spark71xx-usbimg"

IMAGE_CLASSES += "usb_image-spark71xx"
INITRAMFS_FSTYPES = "ext2.gz spark-updt"

MACHINE_EXTRA_RRECOMMENDS = "kernel-modules"

MACHINE_ESSENTIAL_EXTRA_RDEPENDS += " \
                                     stlinux24-sh4-stx7111-fdma-firmware \
                                     stlinux24-sh4-stmfb-firmware-stx7111 \
                                     stslave-fw \
                                     fp-control \
                                     stfbcontrol \
                                     ustslave \
                                     libmmeimage \
                                     lirc \
                                     mtd-utils \
                                     mtd-utils-jffs2 \
                                     mtd-utils-misc \
                                     tdt-driver \
"

# MACHINE_ESSENTIAL_EXTRA_RDEPENDS += " mtd-utils-ubifs "

##
## We have a RTC in aotom thx to seife.
# MACHINE_FEATURES_BACKFILL_CONSIDERED = "rtc"


##
## This would not work unless the kernel supports open by handle syscall
## http://lwn.net/Articles/390863/ 
##
#DISTRO_FEATURES_append = " systemd"
#VIRTUAL-RUNTIME_init_manager = "systemd"
#DISTRO_FEATURES_BACKFILL_CONSIDERED = "sysvinit"

## stlinux is configured like that:
OLDEST_KERNEL = "2.6.32"

## the latest available stlinux gcc patch is for 4.8.4, and 4.9 is
## behaving "strange" -- to say the least.
## if you are feeling brave, you can still set GCCVERSION="4.9%" in conf/local.conf ;-)
GCCVERSION ?= "4.8.4"

## dizzy has libgcov-dev in packagegroup-core-standalone-sdk-target, but
## we don't build it anymore...
RDEPENDS_packagegroup-core-standalone-sdk-target_remove = "libgcov-dev"
