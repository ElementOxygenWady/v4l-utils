SUBDIRS = \
	libv4l2util \
	libmedia_dev \
	ivtv-ctl \
	ir-ctl \
	cx18-ctl \
	keytable \
	media-ctl \
	v4l2-ctl \
	v4l2-dbg \
	v4l2-sysfs-path \
	libcecutil \
	cec-ctl \
	cec-compliance \
	cec-follower \
	rds-ctl

if WITH_V4L2_TRACER
SUBDIRS += \
	v4l2-tracer
endif

if WITH_LIBDVBV5
SUBDIRS += \
	dvb
endif

if WITH_V4L2_COMPLIANCE
SUBDIRS += \
	v4l2-compliance
endif

if WITH_QV4L2
SUBDIRS += qv4l2
endif

if WITH_QVIDCAP
SUBDIRS += qvidcap
endif

EXTRA_DIST = common
