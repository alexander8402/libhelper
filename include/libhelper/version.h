/**
 *     libhelper
 *     Copyright (C) 2019, @h3adsh0tzz
 *
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#ifndef LIBHELPER_VERSION_H
#define LIBHELPER_VERSION_H

#define LIBHELPER_VERSION_LONG      "libhelper-1000.742.56.7.4"
#define LIBHELPER_VERSION_SHORT     "1.0.0"
#define HTOOL_VERSION_TAG           "Alpha"
#define HTOOL_VERSION_TAG_CAPS      "ALPHA"

#ifdef __APPLE__
#   define LIBHELPER_PLATFORM       "Darwin"
#else
#   define LIBHELPER_PLATFORM       "Linux"
#endif

#ifdef __x86_64__
#   define LIBHELPER_VERS_WITH_ARCH     LIBHELPER_VERSION_LONG "/" HTOOL_VERSION_TAG_CAPS "_X86_64 x86_64"
#elif __arm__
#   define LIBHELPER_VERS_WITH_ARCH     LIBHELPER_VERSION_LONG "/" HTOOL_VERSION_TAG_CAPS "_ARM64 arm64"
#else 
#   define LIBHELPER_VERS_WITH_ARCH     LIBHELPER_VERSION_LONG "/" HTOOL_VERSION_TAG_CAPS "_NA unknown_architecture"
#endif

#endif /* libhelper_version_h */