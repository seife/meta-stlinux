HOMEPAGE = "https://github.com/Duckbox-Developers/apps"
SECTION = "base"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://${THISDIR}/files/COPYING;md5=0636e73ff0215e8d672dc4c32c317bb3"
DEPENDS = "tdt-driver"
PACKAGE_ARCH = "${MACHINE_ARCH}"

PR = "r0"

SRCREV = "6d79026a12591038663721c14adb76f8f5c3cc42"

SRC_URI = " \
	git://github.com/Duckbox-Developers/apps.git;protocol=git \
"
PV = "1.0+git${SRCPV}"

S = "${WORKDIR}/git/tools"

inherit autotools-brokensep

EXTRA_OECONF = "--prefix="

do_configure() {
	# from ./autogen.sh, which is not cross-friendly
	if [ ! -e acinclude.m4 ]; then
		for i in .. ../.. ../../..; do
			if [ -e `pwd`/$i/acinclude.m4 ]; then
				ln -s `pwd`/$i/acinclude.m4 .
			fi
		done
	fi
	echo "Generating configuration files for $package, please wait...."
	echo "  aclocal $ACLOCAL_FLAGS"
	aclocal $ACLOCAL_FLAGS
	echo "  libtoolize --automake"
	libtoolize --automake
	echo "  autoconf"
	autoconf
	#echo "  autoheader"
	#autoheader
	echo "  automake --add-missing"
	automake --add-missing

	oe_runconf
}

do_compile() {
	 oe_runmake  'DESTDIR=${D}' 'SUBDIRS=${@d.getVar('PN',1).replace('-', '_')}' 
}

do_install() {
	 oe_runmake  'DESTDIR=${D}' 'SUBDIRS=${@d.getVar('PN',1).replace('-', '_')}' install
}
