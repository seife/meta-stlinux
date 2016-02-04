SECTION = "kernel/modules"
RDEPENDS_${PN} = "ustslave"

LICENSE = " GPLv2"
LIC_FILES_CHKSUM = "file://${WORKDIR}/COPYING;md5=751419260aa954499f7abaabaa882bbe"

inherit module

DEPENDS_append = " update-rc.d-native stlinux24-spark"

PACKAGES = "${PN} ${PN}-dev"
# PACKAGES_DYNAMIC += "kernel-module-*"
# without this, tdt-driver package will depend on lots of unavailable kernel-module-*
# (all those modules are in tdt-driver)
KERNEL_MODULES_META_PACKAGE = "None"

BINARY_PTI_NP_PATH ?= "/data/pti_np"

FILES_${PN} = "/etc/udev/rules.d "
FILES = ""

S = "${WORKDIR}/git"

do_configure() {
    if [ -L include/stmfb ]; then
        rm include/stmfb
    fi
    if [ -L include/player2 ]; then
        rm include/player2
    fi
    if [ -L include/multicom ]; then
        rm include/multicom
    fi
    if [ -L player2 ]; then
        rm player2
    fi
    if [ -L stgfb/stmfb ]; then
        rm stgfb/stmfb
    fi
    if [ -L multicom ]; then
        rm multicom
    fi
    ln -s stmfb-3.1_stm24_${STMFB} ${S}/include/stmfb
    ln -s ${PLAYER2_INCLUDES} ${S}/include/player2
    ln -s ../multicom-3.2.4/include ${S}/include/multicom
    ln -s player2_191 ${S}/player2
    ln -s multicom-3.2.4 ${S}/multicom
    ln -s stmfb-3.1_stm24_${STMFB} ${S}/stgfb/stmfb
    rm -f include/asm
    ln -s ${STAGING_KERNEL_BUILDDIR}/include/asm-sh include/asm
    rm -f .config
    printf "export CONFIG_PLAYER_191=y\nexport CONFIG_MULTICOM324=y\n" > .config
    # disable wireless build
    # sed -i 's/^\(obj-y.*+= wireless\)/# \1/' Makefile
    # disable useless simu_button
    sed -i 's/^\(obj-y.*+= simu_button\)/# \1/' Makefile
    rm -fr ${WORKDIR}/git-7162
    rm -fr ${WORKDIR}/git-7111
    cp -al ${WORKDIR}/git ${WORKDIR}/git-7162
    cp -al ${WORKDIR}/git ${WORKDIR}/git-7111

    rm -fr ${WORKDIR}/kernel-source
    mkdir ${WORKDIR}/kernel-source
    cp -al ${STAGING_KERNEL_DIR}/. ${WORKDIR}/kernel-source
    cp -al ${STAGING_KERNEL_BUILDDIR}/. ${WORKDIR}/kernel-source
    rm ${WORKDIR}/kernel-source/source
}

# EXTRA_OEMAKE = "-e "

do_compile() {
	unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS
	for i in 7162 7111; do
		cd ${WORKDIR}/git-$i
		MACH=SPARK
		if [ $i = 7162 ]; then
			MACH=SPARK7162
		fi
		rm -f ${WORKDIR}/kernel-source/.config
		cp ${WORKDIR}/kernel-source/.config-$i ${WORKDIR}/kernel-source/.config
		touch ${WORKDIR}/kernel-source/.config # ensure that it is newer
		# do_make_scripts
		bbnote "make scripts for MACH=$MACH, config=.config-$i"
		make CC="${KERNEL_CC}" LD="${KERNEL_LD}" AR="${KERNEL_AR}" \
			-C ${WORKDIR}/kernel-source scripts silentoldconfig
		bbnote "oe_runmake in ${WORKDIR}/git-$i MACH=$MACH"
		rm -f ${WORKDIR}/build
		ln -s git-$i ${WORKDIR}/build
		cd ${WORKDIR}/build
		oe_runmake KERNEL_PATH=${WORKDIR}/kernel-source \
			KERNEL_SRC=${WORKDIR}/kernel-source \
			KERNEL_VERSION=${KERNEL_VERSION}    \
			$MACH=1 \
			M=${WORKDIR}/build V=1 \
			ARCH=sh \
			KBUILD_NOPEDANTIC=1 \
			PLAYER191=player191 \
			DRIVER_TOPDIR="${WORKDIR}/git-$i" \
			KERNEL_LOCATION="${WORKDIR}/kernel-source" \
			CONFIG_KERNEL_BUILD="${WORKDIR}/kernel-source" \
			CONFIG_KERNEL_PATH="${WORKDIR}/kernel-source" \
			CONFIG_MODULES_PATH="${D}" \
			CONFIG_PLAYER_191=y \
			CCFLAGSY="-I${STAGING_DIR_HOST}/usr/include" \
			${PARALLEL_MAKE} \
			all
	done
}

#			CCFLAGSY="-I${STAGING_DIR_HOST}/usr/include -I${WORKDIR}/build/include" \
#

do_install() {
	unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS
	for i in 7162 7111; do
		cd ${WORKDIR}/git-$i
		MACH=SPARK
		if [ $i = 7162 ]; then
			MACH=SPARK7162
		fi
		rm -f ${WORKDIR}/kernel-source/.config
		cp ${WORKDIR}/kernel-source/.config-$i ${WORKDIR}/kernel-source/.config
		touch ${WORKDIR}/kernel-source/.config # ensure that it is newer
		# do_make_scripts
		# bbnote "make scripts for MACH=$MACH, config=.config-$i"
		make CC="${KERNEL_CC}" LD="${KERNEL_LD}" AR="${KERNEL_AR}" \
			-C ${WORKDIR}/kernel-source scripts silentoldconfig
		bbnote "oe_runmake modules_install in ${WORKDIR}/git-$i MACH=$MACH"
		rm -f ${WORKDIR}/build
		ln -s git-$i ${WORKDIR}/build # this avoids binaries differing due to pathnames
		cd ${WORKDIR}/build
		oe_runmake KERNEL_PATH=${WORKDIR}/kernel-source \
			KERNEL_SRC=${WORKDIR}/kernel-source \
			KERNEL_VERSION=${KERNEL_VERSION}    \
			$MACH=1 \
			M=${WORKDIR}/build V=1 \
			ARCH=sh \
			KBUILD_NOPEDANTIC=1 \
			PLAYER191=player191 \
			DRIVER_TOPDIR="${WORKDIR}/git-$i" \
			KERNEL_LOCATION="${WORKDIR}/kernel-source" \
			CONFIG_KERNEL_BUILD="${WORKDIR}/kernel-source" \
			CONFIG_KERNEL_PATH="${WORKDIR}/kernel-source" \
			CONFIG_MODULES_PATH="${D}" \
			CONFIG_PLAYER_191=y \
			CCFLAGSY="-I${STAGING_DIR_HOST}/usr/include" \
			INSTALL_MOD_PATH=${D} install
		mkdir ${D}/lib/modules/$i
		mv ${D}/lib/modules/${KERNEL_VERSION} ${D}/lib/modules/$i
	done
	# $PWD is git-7111
	# install header files
	install -d ${D}/${includedir}/linux/dvb
	install -m 644 bpamem/bpamem.h ${D}/${includedir}
	for i in aotom_spark aotom; do
		if test -d frontcontroller/$i; then
			install -m 644 frontcontroller/$i/aotom_main.h ${D}/${includedir}
			break
		fi
	done
	install -m 644 player2/linux/include/linux/dvb/stm_ioctls.h ${D}/${includedir}/linux/dvb
	install -m 644 stgfb/stmfb/linux/drivers/video/stmfb.h ${D}/${includedir}/linux
	# for libmmeimage
	install -m 644 multicom/include/mme.h ${D}/${includedir}
	install -m 644 include/player2/JPEG_VideoTransformerTypes.h ${D}/${includedir}

	#install modutils config
	# install -d ${D}/${sysconfdir}/modules-load.d
	# install -d ${D}/${sysconfdir}/modprobe.d
	install -d ${D}/${sysconfdir}/udev/rules.d/
	cp ${WORKDIR}/aotom_udev.rules ${D}/${sysconfdir}/udev/rules.d/32-aotom.rules

	# if a custom pti.ko is present, overwrite the tdt one
	#if [ -e ${BINARY_PTI_NP_PATH}/pti_${STM_PATCH_STR}.ko ]; then
	#	echo "Found custom pti.." 
	#	install -m 644 ${BINARY_PTI_NP_PATH}/pti_${STM_PATCH_STR}.ko ${D}/lib/modules/${KERNEL_VERSION}/extra/pti/
	#fi
	find ${D} -name stmcore-display-sti7106.ko | xargs -r rm # we don't have a 7106 chip

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

	# select_sparkbox sets the symlink from /lib/modules/{7111,7162}/KVER to /lib/module/KVER
	# early during boot, additionally it selects the right component.fw in /lib/firmware
	# TODO: this does belong somewhere else.
	install -D -m 0755 ${WORKDIR}/select_sparkbox.sh ${D}/etc/init.d/zz_select_sparkbox.sh
	# needs to come after 02sysfs.sh and before 03udev... => 02zz_select...
	update-rc.d -r ${D} zz_select_sparkbox.sh start 02 S .

	install -D -m 0755 ${WORKDIR}/tdt-driver.init ${D}/etc/init.d/tdt-driver
	# drivers seem to be right at about ~ 30
	update-rc.d -r ${D} tdt-driver start 30 S .
}

MOD7111 = "/lib/modules/7111/${KERNEL_VERSION}/extra"
MOD7162 = "/lib/modules/7162/${KERNEL_VERSION}/extra"
# include all except wireless, so that they can be installed separately
# i2c_spi module (7162) is also not included, it's very special and should
# only be used for testing anyway.
FILES_${PN} += " \
	${MOD7111}/avs		${MOD7162}/avs \
	${MOD7111}/boxtype	${MOD7162}/boxtype \
	${MOD7111}/bpamem	${MOD7162}/bpamem \
	${MOD7111}/cec		${MOD7162}/cec \
	${MOD7111}/compcache	${MOD7162}/compcache \
	${MOD7111}/cpu_frequ	${MOD7162}/cpu_frequ \
	${MOD7111}/e2_proc	${MOD7162}/e2_proc \
	${MOD7111}/frontcontroller ${MOD7162}/frontcontroller \
	${MOD7111}/frontends	${MOD7162}/frontends \
	${MOD7111}/multicom	${MOD7162}/multicom \
	${MOD7111}/player2	${MOD7162}/player2 \
	${MOD7111}/pti		${MOD7162}/pti \
	${MOD7111}/smartcard	${MOD7162}/smartcard \
	${MOD7111}/stgfb	${MOD7162}/stgfb \
				${MOD7162}/i2c_spi \
	/etc/init.d \
	/etc/rcS.d \
"

pkg_postinst_${PN}() {
#!/bin/sh
# create the default symlink to 7111 module dir or image build will fail.
if ! [ -e $D/lib/modules/${KERNEL_VERSION} ]; then
	ln -s 7111/${KERNEL_VERSION} $D/lib/modules/
fi
if [ -z "$D" ]; then
	depmod -a ${KERNEL_VERSION}
else
	depmodwrapper -a -b $D ${KERNEL_VERSION}
fi
}

FILES_${PN}-dev += "${includedir}"
