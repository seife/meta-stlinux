HOMEPAGE = "https://www.gitorious.org/open-duckbox-project-sh4/tdt"
SECTION = "base"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://${THISDIR}/files/COPYING;md5=0636e73ff0215e8d672dc4c32c317bb3"
DEPENDS = "tdt-driver"
PACKAGE_ARCH = "${MACHINE_ARCH}"

PR = "r3"

SRCREV = "f5159041295774ef5bbb338c7f5d831bc2620d03"

SRC_URI = " \
	git://gitorious.org/open-duckbox-project-sh4/tdt.git \
"
PV = "0.1+git${SRCPV}"

S = "${WORKDIR}/git/tdt/cvs/apps/misc/tools"
DRVDIR = "${WORKDIR}/git/tdt/cvs/driver"

inherit autotools-brokensep

EXTRA_OECONF = "--enable-silent-rules --prefix="

do_configure_prepend () {
	## same as in tdt-driver...
	oldpwd=`pwd`
	cd ${DRVDIR}
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
	ln -s stmfb-3.1_stm24_0102 ${DRVDIR}/include/stmfb
	ln -s player2_179 ${DRVDIR}/include/player2
	ln -s ../multicom-3.2.4/include ${DRVDIR}/include/multicom
	ln -s player2_191 ${DRVDIR}/player2
	ln -s multicom-3.2.4 ${DRVDIR}/multicom
	ln -s stmfb-3.1_stm24_0102 ${DRVDIR}/stgfb/stmfb
	cd $oldpwd
}

do_compile() {
	 oe_runmake  'DESTDIR=${D}' 'SUBDIRS=${@d.getVar('PN',1).replace('-', '_')}' 
}

do_install() {
	 oe_runmake  'DESTDIR=${D}' 'SUBDIRS=${@d.getVar('PN',1).replace('-', '_')}' install
}
