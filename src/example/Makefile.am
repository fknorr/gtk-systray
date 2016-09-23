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


check_PROGRAMS = $(top_builddir)/example

__top_builddir__example_SOURCES = \
	main.c

__top_builddir__example_LDADD = \
	$(top_builddir)/libgtk-systray.la \
	$(GTK_LIBS)

__top_builddir__example_CPPFLAGS = \
	-I$(top_srcdir)/src/libgtk-systray \
	$(GTK_CFLAGS)


