# This file is part of libgtk-systray.
#
# libgtk-systray is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# libgtk-systray is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with libgtk-systray.  If not, see <http://www.gnu.org/licenses/>.
# Copyright (c) 2014-2015, Fabian Knorr


lib_LTLIBRARIES =  \
	$(top_builddir)/libgtk-systray.la

__top_builddir__libgtk_systray_la_SOURCES = \
	systray-box.c \
	systray-manager.c \
	systray-marshal.c \
	systray-socket.c \
	systray.c

gtkgldir = $(includedir)/gtk-systray
gtkgl_HEADERS = \
    $(top_srcdir)/src/libgtk-systray/systray.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = gtk-systray.pc

__top_builddir__libgtk_systray_la_CFLAGS = \
	$(GTK_CFLAGS) \
	$(X11_CFLAGS)

__top_builddir__libgtk_systray_la_LIBADD = \
	$(GTK_LIBS) \
	$(X11_LIBS) 

__top_builddir__libgtk_systray_la_LDFLAGS = \
    $(VERSION_INFO)
