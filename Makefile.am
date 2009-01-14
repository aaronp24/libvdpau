SUBDIRS = src trace

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = vdpau.pc

EXTRA_DIST = vdpau.pc.in autogen.sh ChangeLog

.PHONY: ChangeLog

ChangeLog:
	(GIT_DIR=$(top_srcdir)/.git git-log > .changelog.tmp && mv .changelog.tmp ChangeLog; rm -f .changelog.tmp) || (touch ChangeLog; echo 'git directory not found: installing possibly empty changelog.' >&2)

dist-hook: ChangeLog
