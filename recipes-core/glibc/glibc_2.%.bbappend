FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append_sh4 = "\
	file://glibc-sh4-retry-getXXbyYY.patch;striplevel=2 \
"
