require linux-stm.inc
FILESEXTRAPATHS_prepend := "${THISDIR}/linux-stm:"

DESCRIPTION = "Linux kernel from stlinux"
LINUX_VERSION = "2.6.32.57"

STM_PATCH_STR = "0210"

# INC_PR is defined in the .inc file if something has change here just increase the number after the dot
PR = "${INC_PR}.7"

PV = "${LINUX_VERSION}-stm24-${STM_PATCH_STR}"
KBRANCH = "stmicro"

KTAG = "stlinux24_0210"


### TODO ###
# This should already set in the kernel.bbclass 
# No doubt why it is not applied
# Normaly only stlinux24-sh4-stx7105-fdma-firmware should be required 
DEPENDS_spark7162_append += " \
                   virtual/${TARGET_PREFIX}gcc \
                   virtual/${TARGET_PREFIX}depmod \ 
                   virtual/${TARGET_PREFIX}gcc${KERNEL_CCSUFFIX} \
                   update-modules \
                   stlinux24-sh4-stx7105-fdma-firmware \
"
### TODO ###
# This should already set in the kernel.bbclass 
# No doubt why it is not applied here
# Normaly only stlinux24-sh4-stx7111-fdma-firmware should be required 
DEPENDS_spark_append  += " \
                   virtual/${TARGET_PREFIX}gcc \
                   virtual/${TARGET_PREFIX}depmod \ 
                   virtual/${TARGET_PREFIX}gcc${KERNEL_CCSUFFIX} \
                   update-modules \
                   stlinux24-sh4-stx7111-fdma-firmware \
"


SRCREV="7367427b3c1b8965a0f5c960a18c5c802ad2eb8f"
SRC_URI = "git://git.stlinux.com/stm/linux-sh4-2.6.32.y.git;protocol=git;branch=stmicro \
           file://${MACHINE}_defconfig \
           file://st-coprocessor.h \
"
SRC_URI_append_spark = "\
             file://linux-sh4-linuxdvb_stm24_${STM_PATCH_STR}.patch;patch=1 \
             file://linux-sh4-sound_stm24_${STM_PATCH_STR}.patch;patch=1 \
             file://linux-sh4-time_stm24_${STM_PATCH_STR}.patch;patch=1 \
             file://linux-sh4-init_mm_stm24_${STM_PATCH_STR}.patch;patch=1 \
             file://linux-sh4-copro_stm24_${STM_PATCH_STR}.patch;patch=1 \
             file://linux-squashfs-lzma_stm24_${STM_PATCH_STR}.patch;patch=1 \
             file://linux-sh4-ext23_as_ext4_stm24_${STM_PATCH_STR}.patch;patch=1 \
             file://bpa2_procfs_stm24_${STM_PATCH_STR}.patch;patch=1 \
             file://linux-ftdi_sio.c_stm24_${STM_PATCH_STR}.patch;patch=1 \
             file://linux-sh4-lzma-fix_stm24_${STM_PATCH_STR}.patch;patch=1 \
             file://linux-tune_stm24.patch;patch=1 \
             file://linux-sh4-stmmac_stm24_${STM_PATCH_STR}.patch;patch=1 \
             file://linux-sh4-lmb_stm24_${STM_PATCH_STR}.patch;patch=1 \
             file://linux-sh4-spark_setup_stm24_${STM_PATCH_STR}_multi_yaffs2.patch;patch=1 \
             file://linux-sh4-cifs-unaligned-mem-access-kernel_stm24.patch;patch=1 \
             file://linux-sh4-linux_yaffs2_stm24_${STM_PATCH_STR}.patch;patch=1 \
             file://linux-sh4-lirc_stm24_${STM_PATCH_STR}.patch;patch=1 \
             file://0001-added-pm_power_off-hoock-for-machine_halt.patch;patch=1 \
"

SRC_URI_append_spark7162 = "\
             file://linux-sh4-linuxdvb_stm24_${STM_PATCH_STR}.patch;patch=1 \
             file://linux-sh4-sound_stm24_${STM_PATCH_STR}.patch;patch=1 \
             file://linux-sh4-time_stm24_${STM_PATCH_STR}.patch;patch=1 \
             file://linux-sh4-init_mm_stm24_${STM_PATCH_STR}.patch;patch=1 \
             file://linux-sh4-copro_stm24_${STM_PATCH_STR}.patch;patch=1 \
             file://linux-squashfs-lzma_stm24_${STM_PATCH_STR}.patch;patch=1 \
             file://linux-sh4-ext23_as_ext4_stm24_${STM_PATCH_STR}.patch;patch=1 \
             file://bpa2_procfs_stm24_${STM_PATCH_STR}.patch;patch=1 \
             file://linux-ftdi_sio.c_stm24_${STM_PATCH_STR}.patch;patch=1 \
             file://linux-sh4-lzma-fix_stm24_${STM_PATCH_STR}.patch;patch=1 \
             file://linux-tune_stm24.patch;patch=1 \
             file://linux-sh4-stmmac_stm24_${STM_PATCH_STR}.patch;patch=1 \
             file://linux-sh4-lmb_stm24_${STM_PATCH_STR}.patch;patch=1 \
             file://linux-sh4-spark_setup_stm24_${STM_PATCH_STR}_multi_yaffs2.patch;patch=1 \
             file://linux-sh4-seife-revert-spark_setup_stmmac_mdio.patch;patch=1 \
             file://linux-sh4-spark7162_setup_stm24_${STM_PATCH_STR}.patch;patch=1 \
             file://linux-sh4-linux_yaffs2_stm24_${STM_PATCH_STR}.patch;patch=1 \
"


COMPATIBLE_MACHINE = "(spark|spark7162)"

PARALLEL_MAKEINST = ""

do_configure () {
        yes '' | oe_runmake oldconfig
}

do_configure_spark7162 () {
	sed -i "s#^\(CONFIG_EXTRA_FIRMWARE_DIR=\).*#\1\"${STAGING_DIR_HOST}/lib/firmware\"#" .config;
        yes '' | oe_runmake oldconfig
}

do_configure_spark () {
	sed -i "s#^\(CONFIG_EXTRA_FIRMWARE_DIR=\).*#\1\"${STAGING_DIR_HOST}/lib/firmware\"#" .config;
        yes '' | oe_runmake oldconfig
}


