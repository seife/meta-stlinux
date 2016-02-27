do_fetch() {
	:
}
do_fetch[noexec] = "1"
deltask do_unpack
deltask do_patch

SRC_URI = ""

## this works with dizzy and jethro
do_configure[depends] += "gcc-source:do_preconfigure"
do_populate_lic[depends] += "gcc-source:do_unpack"

# this no longer works well with dizzy, but is standard on jethro
# do_configure[depends] += "gcc-source-${PV}:do_preconfigure"
# do_populate_lic[depends] += "gcc-source-${PV}:do_unpack"
