SUMMARY = "GNU debugger for stlinux"
LICENSE = "GPLv3+"
SECTION = "devel"
DEPENDS = "expat zlib ncurses readline"

inherit src_rpm autotools gettext

LIC_FILES_CHKSUM = "file://COPYING;md5=59530bdf33659b29e73d4adb9f9f6552 \
                file://COPYING.LIB;md5=9f604d8a4f8e74f4f5140845a21b6674 \
                file://COPYING3;md5=d32239bcb673463ab874e80d47fae504 \
                file://COPYING3.LIB;md5=6a6a8e020838b23406c81b19c1d46df6"

PV = "7.6-51"

SRC_URI = " \
	${STLINUX_SH_UPD_SRPMS}/stlinux24-target-gdb-${PV}.src.rpm \
"
SRC_URI[md5sum] = "1c97e0dde27dfb8ff8c7646ade2b622e"
SRC_URI[sha256sum] = "a95d1c75c1351756826279a9d96e503bef64e23dfdfa82310921499e4afc1d89"

LOCAL_SRC = "\
            file://${WORKDIR}/gdb-7.6-stlinux.tar.bz2 \
            file://${WORKDIR}/gdb-7.6-libexpat.patch \
"

B = "${WORKDIR}/build-${TARGET_SYS}"
S = "${WORKDIR}/gdb-7.6"

EXTRA_OEMAKE = "'SUBDIRS=intl mmalloc libiberty opcodes bfd sim gdb etc utils'"

EXPAT = "--with-expat --with-libexpat-prefix=${STAGING_DIR_HOST}"

EXTRA_OECONF = "--disable-gdbtk --disable-tui --disable-x --disable-werror \
                --with-curses --disable-multilib --with-system-readline --disable-sim \
                --without-lzma \
                ${GDBPROPREFIX} ${EXPAT} \
                ${@bb.utils.contains('DISTRO_FEATURES', 'multiarch', '--enable-64-bit-bfd', '', d)} \
                --disable-rpath \
               "

GDBPROPREFIX = "--program-prefix=''"

do_configure () {
        # override this function to avoid the autoconf/automake/aclocal/autoheader
        # calls for now
        (cd ${S} && gnu-configize) || die "failure in running gnu-configize"
        oe_runconf
}

# we don't want gdb to provide bfd/iberty/opcodes, which instead will override the
# right bits installed by binutils.
do_install_append() {
        rm -rf ${D}${libdir}
        rm -rf ${D}${includedir}
        rm -rf ${D}${datadir}/locale
}

RRECOMMENDS_gdb_append_linux = " glibc-thread-db "
RRECOMMENDS_gdb_append_linux-gnueabi = " glibc-thread-db "
RRECOMMENDS_gdbserver_append_linux = " glibc-thread-db "
RRECOMMENDS_gdbserver_append_linux-gnueabi = " glibc-thread-db "

FILES_${PN} += " \
	${datadir}/gdb \
"
