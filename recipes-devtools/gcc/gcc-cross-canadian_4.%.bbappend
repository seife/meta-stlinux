#### sh4 SDK is broken in current yocto dizzy release without this
#### these options are used in the non-sdk crossgcc build, so just
#### pull them over

GCCMULTILIB = "--disable-multilib"

EXTRA_OECONF_append_sh4 = " \
    --with-multilib-list= \
    --enable-incomplete-targets \
"
