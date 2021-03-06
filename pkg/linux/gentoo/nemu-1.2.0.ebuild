# Copyright 1999-2017 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Id$

EAPI=6

inherit cmake-utils

DESCRIPTION="ncurses interface for QEMU"
HOMEPAGE="https://lib.void.so/nemu"
SRC_URI="http://lib.void.so/src/${P}.tar.gz"

LICENSE="BSD-2"
SLOT="0"
KEYWORDS="amd64 x86"
IUSE="+vnc-client debug"

RDEPEND="
	sys-libs/ncurses:0=[unicode]
	dev-db/sqlite:3=
	dev-libs/libusb:1=
	|| ( sys-fs/eudev sys-fs/udev )
	app-emulation/qemu[vnc]
	vnc-client? ( net-misc/tigervnc )"
DEPEND="${RDEPEND}
	sys-devel/gettext"

src_configure() {
	local mycmakeargs=(
		-DNM_WITH_VNC_CLIENT=$(usex vnc-client)
		-DNM_DEBUG=$(usex debug)
	)
	cmake-utils_src_configure
}

pkg_postinst() {
	elog "Old database is not supported (nEMU versions < 1.0.0)."
	elog "You will need to delete current database."
	elog "If upgraded from 1.0.0, execute script:"
	elog "/usr/share/nemu/scripts/upgrade_db.sh"
}
