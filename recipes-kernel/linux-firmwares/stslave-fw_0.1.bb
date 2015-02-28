DESCRIPTION = "STM ST-231 Coprocessor firmware"
LICENSE = "CLOSED"
SECTION = "base"
PACKAGE_ARCH = "all"

# fix architecture mismatch QA error
INSANE_SKIP_${PN} = "arch"

PR = "r4"

BINARY_STSLAVE_FW_PATH ?= "/data/stslave_fw"
FIRMWARE_DIR ?= "/lib/firmware"

SRC_URI = "file://${BINARY_STSLAVE_FW_PATH}/${MACHINE}/audio.elf \
           file://${BINARY_STSLAVE_FW_PATH}/${MACHINE}/video.elf \
           file://30-stm-stslave-firmware.rules \
"


FILES_${PN} += "${FIRMWARE_DIR}"

do_install () {
	install -d ${D}/${FIRMWARE_DIR}
	install -d ${D}/${sysconfdir}/udev/rules.d
	sed "s#@FW_DIR@#${FIRMWARE_DIR}#" ${WORKDIR}/30-stm-stslave-firmware.rules > ${D}/${sysconfdir}/udev/rules.d/30-stm-stslave-firmware.rules
	install -m 644 ${BINARY_STSLAVE_FW_PATH}/${MACHINE}/audio.elf ${D}/${FIRMWARE_DIR}
	install -m 644 ${BINARY_STSLAVE_FW_PATH}/${MACHINE}/video.elf ${D}/${FIRMWARE_DIR}
}


