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


dist_doc_DATA = \
	README \
	COPYING \
	AUTHORS \
	ChangeLog \
	INSTALL \
	NEWS

ACLOCAL_AMFLAGS = -I m4


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(docdir)

SUBDIRS = src

