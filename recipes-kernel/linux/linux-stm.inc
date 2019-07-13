inherit kernel siteinfo
DESCRIPTION = "Linux kernel from stlinux"
SECTION = "kernel"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=d7810fab7487fb0aad327b76f1be7cd7"

FILESEXTRAPATHS_prepend := "${THISDIR}/linux-stm:"

KEEPUIMAGE = "yes"
KTAG = "stlinux24_${STM_PATCH_STR}"
INC_PR = "r5"

KBRANCH = "project-magpie"

S = "${WORKDIR}/git"
# or get_kernelversion will fail...
B = "${WORKDIR}/git-7111"
PARALLEL_MAKEINST = ""

# KERNEL_VERSION = "${@get_kernelversion('${WORKDIR}/git-7111')}"

do_configure() {
	kernel_do_configure
}

kernel_do_configure() {
	cd ${S}
	if test -e scripts/Makefile.fwinst ; then
		sed -i -e "s:-m0644:-m 0644:g" scripts/Makefile.fwinst
	fi
	# ugly... this has no business in do_configure...
	rm -fr ${WORKDIR}/git-*
	mkdir ${WORKDIR}/git-7111
	mkdir ${WORKDIR}/git-7162
	cp -al ${WORKDIR}/git/. ${WORKDIR}/git-7111/.
	cp -al ${WORKDIR}/git/. ${WORKDIR}/git-7162/.
	for i in 7111 7162; do
		bbnote "changing into ${WORKDIR}/git-$i"
		cd ${WORKDIR}/git-$i
		# we'll only install 7111 config in ${STAGING_DIR_HOST}, so the configs
		# need to be sufficiently similar, differ only in really board specific things...
		rm -f .config
		sed "s#^\(CONFIG_EXTRA_FIRMWARE_DIR=\).*#\1\"${STAGING_DIR_HOST}/lib/firmware\"#" < ${WORKDIR}/defconfig_spark$i > .config
		yes '' | oe_runmake oldconfig
	done
}

do_compile() {
	for i in 7162 7111; do
		bbnote "changing into ${WORKDIR}/git-$i"
		cd ${WORKDIR}/git-$i
		kernel_do_compile
	done
}

do_compile_kernelmodules() {
	unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS MACHINE
	for i in 7162 7111; do
		bbnote "changing into ${WORKDIR}/git-$i"
		cd ${WORKDIR}/git-$i
		if (grep -q -i -e '^CONFIG_MODULES=y$' .config); then
			oe_runmake modules CC="${KERNEL_CC}" LD="${KERNEL_LD}"
			cp Module.symvers ${STAGING_KERNEL_BUILDDIR}/Module.symvers-$i
		else
			bbnote "no modules to compile"
		fi
	done
}

do_install() {
	kerneldir=${D}${KERNEL_SRC_PATH}
	# hack, old yocto (dizzy) does not have do_shared_workdir
	if type do_shared_workdir > /dev/null; then
		bbnote "running do_shared_workdir"
		do_shared_workdir
	fi
	MY_KDIR="${STAGING_KERNEL_BUILDDIR}"
	kdir=${STAGING_KERNEL_DIR}
	if [ x"$MY_KDIR" = x"" ]; then
		# old (dizzy) location...
		MY_KDIR=$kerneldir
		kdir=$kerneldir
	fi
	for i in 7162 7111; do
		bbnote "changing into ${WORKDIR}/git-$i"
		cd ${WORKDIR}/git-$i
		#
		cp System.map $MY_KDIR/System.map-${KERNEL_VERSION}-$i
		cp Module.symvers $MY_KDIR/Module.symvers-$i
		for j in auto.conf auto.conf.cmd tristate.conf; do
			cp -a include/config/$j $MY_KDIR/include/config/$j-$i || bbfail $j-$i
		done
		#
		# kernel_do_install does mkdir -p .../Documentation and fails...
		rm -fr ${kerneldir}/Documentation
		bbnote "running kernel_do_install"
		kernel_do_install
		bbnote "running kernel_do_install finished"
		mkdir ${D}/lib/modules/$i
		rm -f ${D}/lib/modules/${KERNEL_VERSION}/build
		mv ${D}/lib/modules/${KERNEL_VERSION} ${D}/lib/modules/$i
		bbnote "in ${WORKDIR}/git-$i, installing .config to ${MY_KDIR}/.config-$i"
		cp -av .config ${MY_KDIR}/.config-$i
		cp -av .config ${MY_KDIR}/.config
		# do not copy to -7111...
		[ $i = 7111 ] && break
		cp -a ${D}/${KERNEL_IMAGEDEST}/${KERNEL_IMAGETYPE}-${KERNEL_VERSION} ${D}/${KERNEL_IMAGEDEST}/${KERNEL_IMAGETYPE}-${KERNEL_VERSION}-$i
		for j in System.map config vmlinux Module.symvers; do
			[ -e ${D}/boot/$j-${KERNEL_VERSION} ] || continue
			cp -a ${D}/boot/$j-${KERNEL_VERSION} ${D}/boot/$j-${KERNEL_VERSION}-$i
		done
	done
	ln -s 7111/${KERNEL_VERSION} ${D}/lib/modules/
	install -m 644 include/linux/utsrelease.h ${MY_KDIR}/include/linux/
	# $PWD is git-7111
	if [ -f include/linux/bounds.h ]; then
		mkdir -p $kdir/include/linux
		cp include/linux/bounds.h $kdir/include/linux/bounds.h
	fi
	if [ -f include/asm-sh/machtypes.h ]; then
		mkdir -p $kdir/include/asm-sh
		cp include/asm-sh/machtypes.h $kdir/include/asm-sh
		rm -f $kdir/include/asm
		ln -sf asm-sh $kdir/include/asm
	fi
	oe_runmake headers_install INSTALL_HDR_PATH=${D}${exec_prefix}/src/linux-${KERNEL_VERSION} ARCH=$ARCH

	install -d ${D}${includedir}/linux	
   	install -m 644 ${WORKDIR}/st-coprocessor.h ${D}${includedir}/linux

	cd ${D}/lib/modules
	for i in `find 7111 -name '*.ko'`; do
		j=7162${i#7111}
		[ -e $j ] || continue
		A="`md5sum < $i`"
		B="`md5sum < $j`"
		[ "$A" = "$B" ] || continue
		bbnote "$i == $j => hardlink"
		rm $j
		ln $i $j # hardlink!
	done
}

do_deploy() {
	for i in 7162 7111; do
		bbnote "changing into ${WORKDIR}/git-$i"
		cd ${WORKDIR}/git-$i
		kernel_do_deploy
		[ x$i = x7111 ] && break
		mv ${DEPLOYDIR}/${KERNEL_IMAGE_BASE_NAME}.bin ${DEPLOYDIR}/${KERNEL_IMAGE_BASE_NAME}.bin-$i
		ln -sf ${KERNEL_IMAGE_BASE_NAME}.bin-7162 ${DEPLOYDIR}/${KERNEL_IMAGETYPE}-$i
	done
}

# hack to override kernel.bbclass...
# uimages are already built in kernel compile
do_uboot_mkimage() {
	:
}

FILES_kernel-dev += "${includedir}/linux"

### we need /lib/modules/${KERNEL_VERSION} to be a symlink,
### but split_kernel_module_packages calls depmod and creates the directory.

PACKAGESPLITFUNCS_append = " undo_depmod "
undo_depmod() {
	if [ -d ${D}/lib/modules/${KERNEL_VERSION} ]; then
		rm -r ${D}/lib/modules/${KERNEL_VERSION}
		ln -s 7111/${KERNEL_VERSION} ${D}/lib/modules/
	fi
}

# do not add, but set absolute, because kernel.bbclass sets (wrong) defaults...
FILES_kernel-base = " \
	/lib/modules/${KERNEL_VERSION} \
	/lib/modules/7111/${KERNEL_VERSION}/modules.order \
	/lib/modules/7162/${KERNEL_VERSION}/modules.order \
	/lib/modules/7111/${KERNEL_VERSION}/modules.builtin \
	/lib/modules/7162/${KERNEL_VERSION}/modules.builtin \
"

# bitbake.conf only prepends PARALLEL make in tasks called do_compile, which isn't the case for compile_modules
# So explicitly enable it for that in here
EXTRA_OEMAKE = "${PARALLEL_MAKE} V=1 "

PACKAGES =+ "kernel-headers"
FILES_kernel-headers = "${exec_prefix}/src/linux*"


## Automatically depend on lzop-native if CONFIG_KERNEL_LZO is enabled
python () {
    try:
        defconfig = bb.fetch2.localpath('file://defconfig', d)
    except bb.fetch2.FetchError:
        return

    try:
        configfile = open(defconfig)
    except IOError:
        return

    if 'CONFIG_KERNEL_LZO=y\n' in configfile.readlines():
        depends = d.getVar('DEPENDS', False)
        d.setVar('DEPENDS', depends + ' lzop-native')
}

pkg_preinst_kernel-image() {
#!/bin/sh
[ -n "$D" ] && exit 0
# is /boot already mounted? then someone configured this manually.
grep -q " /boot " /proc/mounts && exit 0
ROOTDEV=$(stat -c %04D /)
# it might have been removed when the package is removed
mkdir /boot 2>/dev/null
if [ "$ROOTDEV" = 1f06 ]; then # /dev/mtd6
	mount -t tmpfs tmpfs /boot -osize=16M
fi
if [ "$ROOTDEV" = 0802 ]; then # /dev/sda2 => USB boot
	mount /dev/sda1 /boot
fi
exit 0 # no errors
# no further update-alternatives stuff etc needed...
}

pkg_postinst_kernel-image() {
#!/bin/sh
[ -n "$D" ] && exit 0
ROOTDEV=$(stat -c %04D /)
read a b c < /proc/cpuinfo
if [ "$c" = hdk7105 ]; then
	SUFFIX="-7162"
elif [ "$c" = sat7111 ]; then
	SUFFIX=""
else
	echo "unknown system board?"
	exit 0
fi
P=${PKGV}
P=${P//-/_}
if [ "$ROOTDEV" = 1f06 ]; then # /dev/mtd6
	UIMAGE=/boot/uImage-$P$SUFFIX
	RET=true
	if [ ! -e $UIMAGE ]; then
		echo "$UIMAGE is missing!"
	else
		flash_erase /dev/mtd5 0 0
		nandwrite -p /dev/mtd5 $UIMAGE || RET=false
	fi
	if grep -q '^tmpfs /boot ' /proc/mounts; then
		umount /boot
	fi
	$RET
elif [ "$ROOTDEV" = 0802 ]; then
	if grep -q '^/dev/sda1 /boot ' /proc/mounts; then
		UIMAGE=/boot/uImage-$P
		rm -f /boot/uImage # symlink might be present from imaging
		mv $UIMAGE		/boot/uImage
		mv $UIMAGE-7162		/boot/uImage-7162
		umount /boot
	fi
fi
exit
}
