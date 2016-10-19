inherit image_types

# This image reciope was taken from the raspberry pi meta layer
# Create an image that can by written onto a USB Thumb drive card using dd.
#
# The disk layout used is:
#
#    0                      -> IMAGE_ROOTFS_ALIGNMENT         - reserved for other data
#    IMAGE_ROOTFS_ALIGNMENT -> BOOT_SPACE                     - bootloader and kernel
#    BOOT_SPACE             -> USBIMG_SIZE                    - rootfs
#

#                                                     Default Free space = 1.3x
#                                                     Use IMAGE_OVERHEAD_FACTOR to add more space
#                                                     <--------->
#            4KiB              20MiB           USBIMG_ROOTFS
# <-----------------------> <----------> <---------------------->
#  ------------------------ ------------ ------------------------ -------------------------------
# | IMAGE_ROOTFS_ALIGNMENT | BOOT_SPACE | ROOTFS_SIZE            |     IMAGE_ROOTFS_ALIGNMENT    |
#  ------------------------ ------------ ------------------------ -------------------------------
# ^                        ^            ^                        ^                               ^
# |                        |            |                        |                               |
# 0                      4096      4KiB + 20MiB     4KiB + 20Mib + USBIMG_ROOTFS    4KiB + 20MiB + USBIMG_ROOTFS + 4KiB


# Set kernel and boot loader
IMAGE_BOOTLOADER ?= "u-boot-mkimage"

# Boot partition volume id
BOOTDD_VOLUME_ID ?= "${MACHINE}"

# Boot partition size [in KiB]
# default to 60MB, so that we can use the same stick for flashing via U-boot
# root-partition is aligned at 64mb
BOOT_SPACE ?= "61440"

# Set alignment to 4MB [in KiB]
IMAGE_ROOTFS_ALIGNMENT = "4096"

# Use an uncompressed ext3 by default as rootfs
USBIMG_ROOTFS_TYPE ?= "ext3"
USBIMG_ROOTFS = "${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.rootfs.${USBIMG_ROOTFS_TYPE}"

IMAGE_DEPENDS_spark71xx-usbimg = " \
			parted-native \
			mtools-native \
			dosfstools-native \
			virtual/kernel \
			${IMAGE_BOOTLOADER} \
			"

# ensure the ext3 image is built before the usb image build starts
IMAGE_TYPEDEP_spark71xx-usbimg = "${USBIMG_ROOTFS_TYPE}"

# USB image name
USBIMG = "${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.rootfs.spark71xx-usbimg"

# Additional files and/or directories to be copied into the vfat partition from the IMAGE_ROOTFS.
FATPAYLOAD ?= ""

IMAGEDATESTAMP = "${@time.strftime('%Y.%m.%d',time.gmtime())}"
# https://www.mail-archive.com/yocto@yoctoproject.org/msg29667.html
IMAGE_CMD_spark71xx-usbimg[vardepsexclude] = "DATETIME"

IMAGE_CMD_spark71xx-usbimg () {

	# Align partitions
	BOOT_SPACE_ALIGNED=$(expr ${BOOT_SPACE} + ${IMAGE_ROOTFS_ALIGNMENT} - 1)
	BOOT_SPACE_ALIGNED=$(expr ${BOOT_SPACE_ALIGNED} - ${BOOT_SPACE_ALIGNED} % ${IMAGE_ROOTFS_ALIGNMENT})
	USBIMG_SIZE=$(expr ${IMAGE_ROOTFS_ALIGNMENT} + ${BOOT_SPACE_ALIGNED} + $ROOTFS_SIZE + ${IMAGE_ROOTFS_ALIGNMENT})
	USBIMG_ROOTOFFSET=$(expr ${BOOT_SPACE_ALIGNED} + ${IMAGE_ROOTFS_ALIGNMENT})

	if dd if=/dev/zero of=/dev/null count=0 conv=sparse >/dev/null 2>&1; then
		echo "creating sparse image"
		SPARSE=,sparse
	else
		echo "not creating sparse image"
		SPARSE=""
	fi
	# Initialize usbstick image file
	dd if=/dev/zero of=${USBIMG} bs=1k count=0 seek=${USBIMG_SIZE}

	# Create partition table
	parted -s ${USBIMG} mklabel msdos
	# Create boot partition and mark it as bootable
	parted -s ${USBIMG} unit KiB mkpart primary fat32 ${IMAGE_ROOTFS_ALIGNMENT} ${USBIMG_ROOTOFFSET}
	parted -s ${USBIMG} set 1 boot on
	# Create rootfs partition
	parted -s ${USBIMG} unit KiB mkpart primary ext2 ${USBIMG_ROOTOFFSET} $(expr ${USBIMG_ROOTOFFSET} \+ ${ROOTFS_SIZE})
	parted ${USBIMG} print

	# Create a vfat image with boot files
	BOOT_BLOCKS=$(LC_ALL=C parted -s ${USBIMG} unit b print | awk '/ 1 / { print substr($4, 1, length($4 -1)) / 512 /2 }')
	mkfs.vfat -n "${BOOTDD_VOLUME_ID}" -S 512 -C ${WORKDIR}/boot.img $BOOT_BLOCKS
	mcopy -i ${WORKDIR}/boot.img -s ${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${MACHINE}.bin ::uImage
	if [ -e ${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-7162 ]; then
		mcopy -i ${WORKDIR}/boot.img -s ${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-7162 ::uImage-7162
	fi

	if [ -n ${FATPAYLOAD} ] ; then
		echo "Copying payload into VFAT"
		for entry in ${FATPAYLOAD} ; do
				# add the || true to stop aborting on vfat issues like not supporting .~lock files
				mcopy -i ${WORKDIR}/boot.img -s -v ${IMAGE_ROOTFS}$entry :: || true
		done
	fi

	# this is u-boot script code until EOF
	cat > ${WORKDIR}/script.scr <<EOF
echo "checking if boot from FLASH is requested..."
if fatls usb 0:1 spark.run; then
	echo "...yes, booting SPARK from FLASH"
	setenv bootargs \${bootargs_spark}
	if test \${board} = pdk7105; then
		echo "SPARK7162 -> nboot.i"
		nboot.i 0x80000000 0 \${kernel_base_spark}
		bootm 0x80000000
		# does (should :-) not return
	fi
	echo "no SPARK7162 -> direct bootm"
	bootm \${kernel_base_spark}
	# we should not reach this...
	exit
fi
if fatls usb 0:1 enigma2.run; then
	echo "...yes, booting ENIGMA partition from FLASH"
	setenv bootargs \${bootargs_enigma2}
	nboot.i 0x80000000 0 \${kernel_base_enigma2}
	bootm 0x80000000
	# again, this should not return...
	exit
fi
echo "...no, booting from USB..."
if test \${board} = pdk7105; then
	fatload usb 0:1 80000000 uImage-7162
else
	fatload usb 0:1 80000000 uImage
fi
setenv bootargs console=ttyAS0,115200 root=/dev/sda2 rootfstype=${USBIMG_ROOTFS_TYPE} rw coprocessor_mem=4m@0x40000000,4m@0x40400000 printk=1 printk.time=1 nwhwconf=device:eth0,hwaddr:00:80:E1:12:40:69 bigphysarea=6000 stmmaceth=msglvl:0,phyaddr:2,watchdog:5000 panic=10 rootwait usb_storage.delay_use=0
bootm 80000000
EOF

	mkimage -A sh -O linux -T script -C none -a 0 -e 0 -n "autoscript" -d ${WORKDIR}/script.scr ${WORKDIR}/script.img
	mcopy -i ${WORKDIR}/boot.img -v ${WORKDIR}//script.img ::

	# Add stamp file
	echo "${IMAGE_NAME}-${IMAGEDATESTAMP}" > ${WORKDIR}/image-version-info
	mcopy -i ${WORKDIR}/boot.img -v ${WORKDIR}//image-version-info ::

	# Burn Partitions
	dd if=${WORKDIR}/boot.img of=${USBIMG} conv=notrunc${SPARSE} bs=1k seek=${IMAGE_ROOTFS_ALIGNMENT}
	# If USBIMG_ROOTFS_TYPE is a .xz file use xzcat
	CAT=cat
	if [[ "$USBIMG_ROOTFS_TYPE" == *.xz ]]
	then
		xzcat ${USBIMG_ROOTFS} | dd of=${USBIMG} conv=notrunc${SPARSE} bs=1k seek=${USBIMG_ROOTOFFSET}
	else
		dd if=${USBIMG_ROOTFS} of=${USBIMG} conv=notrunc${SPARSE} bs=1k seek=${USBIMG_ROOTOFFSET}
	fi
}

