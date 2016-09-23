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


AC_INIT(gtk-systray, 0.1)

LT_INIT([])

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11])

AM_SILENT_RULES([yes])

AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC

PKG_CHECK_MODULES([X11], [x11], [],
    [AC_MSG_ERROR([Missing dependency: X11])])
PKG_CHECK_MODULES([GTK], [gtk+-3.0], [],
    [AC_MSG_ERROR([Missing dependency: GTK+3])])

srcdir=`readlink -f "$srcdir"`
builddir=`readlink -f "$top_builddir"`
AC_DEFINE_UNQUOTED([PREFIX], ["$prefix"], [Installation Prefix])
AC_DEFINE_UNQUOTED([SOURCE_DIR], ["$srcdir"], [Absolute Source Directory])

AC_SUBST(VERSION_INFO, "-version-info 1:1:0")

AC_CONFIG_FILES([src/libgtk-systray/gtk-systray.pc])

AC_OUTPUT([
Makefile
src/Makefile
src/libgtk-systray/Makefile
src/example/Makefile
])

