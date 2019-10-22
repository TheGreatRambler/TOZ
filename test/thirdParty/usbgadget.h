/*
 * Copyright (C) 2009 Daiki Ueno <ueno@unixuser.org>
 * This file is part of libusb-gadget.
 *
 * libusb-gadget is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libusb-gadget is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __USG_H
#define __USG_H

#include <endian.h>
#include <stdint.h>
#include <unistd.h>

/* borrowed from libusb/libusb.h */
#define usb_gadget_bswap16(x) (((x & 0xFF) << 8) | (x >> 8))
#define usb_gadget_bswap32(x) ((usb_gadget_bswap16(x & 0xFFFF) << 16) | usb_gadget_bswap16(x >> 16))
/* borrowed from libusb/libusb.h */
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define usb_gadget_cpu_to_le16(x) (x)
#define usb_gadget_le16_to_cpu(x) (x)
#define usb_gadget_cpu_to_le32(x) (x)
#define usb_gadget_le32_to_cpu(x) (x)
#elif __BYTE_ORDER == __BIG_ENDIAN
#define usb_gadget_cpu_to_le16(x) usb_gadget_bswap16(x)
#define usb_gadget_le16_to_cpu(x) usb_gadget_bswap16(x)
#define usb_gadget_cpu_to_le32(x) usb_gadget_bswap32(x)
#define usb_gadget_le32_to_cpu(x) usb_gadget_bswap32(x)
#else
#error "Unrecognized endianness"
#endif

/*
struct usb_gadget_string {
	uint8_t id;
	const char* s;
};

struct usb_gadget_strings {
	uint16_t language; // 0x0409 for en-us
	struct usb_gadget_string* strings;
};
*/

#endif
