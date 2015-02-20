LICENSE = "CLOSED"

SRC_URI = "git://github.com/mdamt/linux-firmware.git;protocol=http"
SRC_URI = "git://github.com/BjornLee/linux-firmware.git;protocol=http"

S = "${WORKDIR}/git"


PACKAGE_ARCH = "all"
PACKAGES = "${PN}"
FILES_${PN} += "${base_libdir}/firmware"
