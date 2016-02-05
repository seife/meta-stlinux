# hack to fix build with jethro on sh4
do_configure_prepend() {
	rm -f ${S}/linux/linux
	ln -s . ${S}/linux/linux
}
