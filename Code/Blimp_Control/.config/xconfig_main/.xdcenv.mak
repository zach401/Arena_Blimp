#
_XDCBUILDCOUNT = 0
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/ti/grace_1_10_04_36/packages;C:/ti/ccsv5/ccs_base;M:/EMBEDD~1/BLIMPP~1/air_modPWM/.config
override XDCROOT = C:/ti/xdctools_3_23_03_53
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti/grace_1_10_04_36/packages;C:/ti/ccsv5/ccs_base;M:/EMBEDD~1/BLIMPP~1/air_modPWM/.config;C:/ti/xdctools_3_23_03_53/packages;..
HOSTOS = Windows
endif
