require linux-stm.inc

# INC_PR is defined in the .inc file if something has change here just increase the number after the dot
PR = "${INC_PR}"

LINUX_VERSION = "2.6.32.61"
PATCH_STR = "0217"
PV = "${LINUX_VERSION}-stm24-${PATCH_STR}"
SRCREV = "${AUTOREV}"
INSANE_SKIP_kernel-dev = "staticdev"

# thes are taken from oe-alliance-core:
# git://github.com/oe-alliance/oe-alliance-core.git
#	meta-brands/meta-fulan/recipes-linux/linux-fulan/
SRC_URI = " \
	git://github.com/seife/linux-sh4-2.6.32.y.git;protocol=http;branch=stmicro \
	file://linux-sh4-linuxdvb_stm24_${PATCH_STR}.patch;patch=1 \
	file://linux-sh4-sound_stm24_${PATCH_STR}.patch;patch=1 \
	file://linux-sh4-time_stm24_${PATCH_STR}.patch;patch=1 \
	file://linux-sh4-init_mm_stm24_${PATCH_STR}.patch;patch=1 \
	file://linux-sh4-copro_stm24_${PATCH_STR}.patch;patch=1 \
	file://linux-sh4-strcpy_stm24_${PATCH_STR}.patch;patch=1 \
	file://linux-sh4-ext23_as_ext4_stm24_${PATCH_STR}.patch;patch=1 \
	file://linux-sh4-bpa2_procfs_stm24_${PATCH_STR}.patch;patch=1 \
	file://linux-ftdi_sio.c_stm24_${PATCH_STR}.patch;patch=1 \
	file://linux-sh4-lzma-fix_stm24_${PATCH_STR}.patch;patch=1 \
	file://linux-tune_stm24.patch;patch=1 \
	file://linux-sh4-console_missing_argument_stm24_${PATCH_STR}.patch;patch=1 \
	file://linux-sh4-mmap_stm24.patch;patch=1 \
	file://linux-sh4-cpuinfo.patch;patch=1 \
	file://linux-sh4-add_missing_eid.patch;patch=1 \
	file://kbuild-generate-mudules-builtin.patch;patch=1 \
	file://st-coprocessor.h \
	file://defconfig_spark7111 \
	file://defconfig_spark7162 \
	file://linux-sh4-stmmac_stm24_${PATCH_STR}.patch;patch=1 \
	file://linux-sh4-lmb_stm24_${PATCH_STR}.patch;patch=1 \
	file://linux-sh4-spark7162_setup_stm24_${PATCH_STR}.patch;patch=1 \
	file://linux-sh4-spark_setup_stm24_${PATCH_STR}.patch;patch=1 \
	file://linux-sh4-lirc_stm_stm24_${PATCH_STR}.patch;patch=1 \
	file://linux-sh4-spark-af901x-NXP-TDA18218.patch;patch=1 \
	file://linux-sh4-spark-dvb-as102.patch;patch=1 \
"

RDEPENDS_${PN} += " \
	stlinux24-sh4-fdma-firmware-spark \
"
