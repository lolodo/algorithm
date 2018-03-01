# Android build config for libusb
# Copyright © 2012-2013 RealVNC Ltd. <toby.gray@realvnc.com>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
#

LOCAL_PATH:= $(call my-dir)

# libjthread
include $(CLEAR_VARS)
LOCAL_SRC_FILES := rtcpapppacket.cpp rtcpbyepacket.cpp rtcpcompoundpacket.cpp rtcpcompoundpacketbuilder.cpp rtcppacket.cpp rtcppacketbuilder.cpp rtcprrpacket.cpp rtcpscheduler.cpp rtcpsdesinfo.cpp rtcpsdespacket.cpp rtcpsrpacket.cpp rtpcollisionlist.cpp rtpdebug.cpp rtperrors.cpp rtpinternalsourcedata.cpp rtpipv4address.cpp rtpipv6address.cpp rtpipv4destination.cpp rtpipv6destination.cpp rtplibraryversion.cpp rtppacket.cpp rtppacketbuilder.cpp rtppollthread.cpp rtprandom.cpp rtprandomrand48.cpp  rtprandomrands.cpp rtprandomurandom.cpp rtpsession.cpp rtpsessionparams.cpp rtpsessionsources.cpp rtpsourcedata.cpp rtpsources.cpp rtptimeutilities.cpp rtpudpv4transmitter.cpp rtpudpv6transmitter.cpp rtpbyteaddress.cpp rtpexternaltransmitter.cpp rtpsecuresession.cpp rtpabortdescriptors.cpp rtptcpaddress.cpp rtptcptransmitter.cpp

LOCAL_SHARED_LIBRARIES += libjthread
LOCAL_MODULE := libjrtp
include $(BUILD_SHARED_LIBRARY)
