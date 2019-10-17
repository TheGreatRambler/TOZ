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

#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <errno.h>
#include <linux/usb/gadgetfs.h>
#include "usb-gadget.h"
#include "list.h"

#define GADGETFS_DEVICE_PATH "/dev/gadget"
#define USB_BUFSIZ (7 * 1024)
#define NEVENT 5

struct _usb_gadget_endpoint
{
  struct usb_gadget_endpoint ep;
  struct usb_endpoint_descriptor *descriptor, *hs_descriptor;
  struct usb_gadget_list_head ep_list;
  usb_gadget_dev_handle *handle;
  int fd;
};

struct usb_gadget_dev_handle
{
  struct _usb_gadget_endpoint *ep0;
  struct usb_gadget_device *device;
  struct usb_gadget_list_head ep_list;
  usb_gadget_event_cb event_cb;
  void *event_arg;
  int debug_level;
  enum usb_device_speed speed;
};

static inline void
debug (usb_gadget_dev_handle *handle, int level, const char *format, ...)
{
  va_list ap;
  va_start(ap, format);
  if (handle->debug_level >= level)
    {
      vfprintf (stderr, format, ap);
      fflush (stderr);
    }
  va_end(ap);
}

void
usb_gadget_set_debug_level (usb_gadget_dev_handle *handle, int level)
{
  handle->debug_level = level;
}

static int
config_buf (void *buf, unsigned buflen, struct usb_descriptor_header **config)
{
  void *p = buf;
  int i;

  if (config[0]->bDescriptorType != USB_DT_CONFIG)
    {
      errno = EINVAL;
      return -1;
    }
  for (i = 0; config[i]; i++)
    {
      memcpy (p, config[i], config[i]->bLength);
      p += config[i]->bLength;
    }

  ((struct usb_config_descriptor *)buf)->wTotalLength =
    usb_gadget_cpu_to_le16(p - buf);

  return p - buf;
}

static struct _usb_gadget_endpoint *
find_ep0 (struct usb_gadget_dev_handle *handle)
{
  DIR *dirp;
  struct dirent *entry;
  static const char *table[] = {
    "dummy_udc",
    "net2280",
    "gfs_udc",
    "pxa2xx_udc",
    "goku_udc",
    "sh_udc",
    "omap_udc",
    "musb_hdrc",
    "at91_udc",
    "lh740x_udc",
    "atmel_usba_udc",
    NULL
  };
  struct _usb_gadget_endpoint *ep0 = NULL;

  dirp = opendir (GADGETFS_DEVICE_PATH);
  if (!dirp)
    return NULL;

  entry = malloc (offsetof(struct dirent, d_name)
                  + pathconf (GADGETFS_DEVICE_PATH, _PC_NAME_MAX)
                  + 1);
  if (!entry)
    {
      closedir (dirp);
      return NULL;
    }

  while (1)
    {
      struct dirent *result;
      int i;

      if (readdir_r (dirp, entry, &result) < 0)
	break;
      if (!result)
        break;
      for (i = 0; table[i] && strcmp (table[i], entry->d_name); i++)
        ;
      if (table[i])
        {
          ep0 = malloc (sizeof(*ep0));
          if (!ep0)
	    break;
          usb_gadget_init_list_head (&ep0->ep_list);
          ep0->ep.name = strdup (table[i]);
          if (!ep0->ep.name)
            {
              free (ep0);
              ep0 = NULL;
              break;
            }
	  ep0->fd = -1;
          break;
        }
    }

  free (entry);
  closedir (dirp);

  return ep0;
}

static int
open_ep0 (struct usb_gadget_dev_handle *handle)
{
  int ret;
  char buf[USB_BUFSIZ], *p;
  struct _usb_gadget_endpoint *ep0 = handle->ep0;

  snprintf (buf, sizeof(buf), "%s/%s", GADGETFS_DEVICE_PATH, ep0->ep.name);
  ep0->fd = open (buf, O_RDWR);
  if (ep0->fd < 0)
    return -1;

  p = buf;
  *(uint32_t *)p = 0;	/* tag */
  p += sizeof(uint32_t);

  ret = config_buf (p, sizeof(buf) - (p - buf), handle->device->config);
  if (ret < 0)
    goto error;
  p += ret;

  if (handle->device->hs_config)
    {
      ret = config_buf (p, sizeof(buf) - (p - buf), handle->device->hs_config);
      if (ret < 0)
	goto error;
      p += ret;
    }

  memcpy (p, handle->device->device, sizeof(struct usb_device_descriptor));
  p += sizeof(struct usb_device_descriptor);

  if (write (ep0->fd, buf, p - buf) < 0)
    {
      debug (ep0->handle, 2, "libusb-gadget: open_ep0: can't write config\n");
      goto error;
    }

  return 0;

 error:
  close (ep0->fd);
  ep0->fd = -1;
  return -1;
}

static int
ep_matches (struct usb_gadget_dev_handle *handle,
	    const char *name, struct usb_endpoint_descriptor *descriptor)
{
  int address = -1, desired_address;
  int direction = -1, desired_direction;
  int type = -1, desired_type;

  if ('0' <= name[2] && name[2] <= '9')
    {
      char *endptr;

      address = strtoul (&name[2], &endptr, 10);
      if (!strncmp (endptr, "in", 2))
	{
	  direction = USB_DIR_IN;
	  endptr += 2;
	}
      else if (!strncmp (endptr, "out", 3))
	{
	  direction = USB_DIR_OUT;
	  endptr += 3;
	}

      if (!strcmp (endptr, "-bulk"))
	type = USB_ENDPOINT_XFER_BULK;
      else if (!strcmp (endptr, "-iso"))
	type = USB_ENDPOINT_XFER_ISOC;
      else if (!strcmp (endptr, "-int"))
	type = USB_ENDPOINT_XFER_INT;
    }

  desired_address = descriptor->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK;
  if (desired_address && address >= 0 && address != desired_address)
    return 0;
  desired_direction = descriptor->bEndpointAddress & USB_ENDPOINT_DIR_MASK;
  if (direction >= 0 && direction != desired_direction)
    return 0;
  desired_type = descriptor->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK;
  if (type >= 0 && type != desired_type)
    return 0;

  return 1;
}

static struct _usb_gadget_endpoint *
find_ep (struct usb_gadget_dev_handle *handle,
	 struct usb_endpoint_descriptor *descriptor)
{
  DIR *dirp;
  struct dirent *entry;
  struct _usb_gadget_endpoint *ep = NULL;

  assert (handle->ep0);

  dirp = opendir (GADGETFS_DEVICE_PATH);
  if (!dirp)
    return NULL;

  entry = malloc (offsetof(struct dirent, d_name)
                  + pathconf (GADGETFS_DEVICE_PATH, _PC_NAME_MAX)
                  + 1);
  if (!entry)
    {
      closedir (dirp);
      return NULL;
    }

  while (!ep)
    {
      struct dirent *result;
      int i;

    next:
      if (readdir_r (dirp, entry, &result) < 0)
	break;
      if (!result)
        break;

      if (strcmp (handle->ep0->ep.name, entry->d_name) &&
          !strncmp (entry->d_name, "ep", 2))
        {
	  struct _usb_gadget_endpoint *_ep;
	  usb_gadget_list_for_each_entry (_ep, &handle->ep_list, ep_list)
	    {
	      if (!strcmp (_ep->ep.name, entry->d_name))
		goto next;
	    }

	  if (!ep_matches (handle, entry->d_name, descriptor))
	    continue;

	  ep = malloc (sizeof(*ep));
	  if (!ep)
	    break;
	  ep->ep.name = strdup (entry->d_name);
	  if (!ep->ep.name)
	    {
	      free (ep);
	      ep = NULL;
	      break;
	    }
	  ep->fd = -1;
	  usb_gadget_init_list_head (&ep->ep_list);
	  usb_gadget_list_add (&ep->ep_list, &handle->ep_list);
        }
    }

  free (entry);
  closedir (dirp);
  return ep;
}

static int
open_ep (struct _usb_gadget_endpoint *ep,
         struct usb_endpoint_descriptor *descriptor,
         struct usb_endpoint_descriptor *hs_descriptor)
{
  int ret;
  char buf[USB_BUFSIZ], *p;

  snprintf (buf, sizeof(buf), "%s/%s", GADGETFS_DEVICE_PATH, ep->ep.name);
  ep->fd = open (buf, O_RDWR);
  if (ep->fd < 0)
    return -1;

  p = buf;
  *(uint32_t *)p = 1;      /* tag */
  p += sizeof(uint32_t);

  memcpy (p, descriptor, USB_DT_ENDPOINT_SIZE);
  p += USB_DT_ENDPOINT_SIZE;
  if (hs_descriptor)
    {
      memcpy (p, hs_descriptor, USB_DT_ENDPOINT_SIZE);
      p += USB_DT_ENDPOINT_SIZE;
    }
  ret = write (ep->fd, buf, p - buf);
  if (ret < 0)
    {
      debug (ep->handle, 2, "libusb-gadget: open_ep: can't write config\n");
      close (ep->fd);
      return -1;
    }

  return 0;
}

static void
close_ep (struct _usb_gadget_endpoint *ep)
{
  assert (ep);

  usb_gadget_list_del (&ep->ep_list);
  if (ep->fd > 0)
    close (ep->fd);
  free (ep->ep.name);

  ep->fd = -1;
  ep->ep.name = NULL;
}

int
usb_gadget_endpoint_close (struct usb_gadget_endpoint *ep)
{
  struct _usb_gadget_endpoint *_ep;

  _ep = usb_gadget_container_of(ep, struct _usb_gadget_endpoint, ep);
  close_ep (_ep);

  return 0;
}

usb_gadget_dev_handle *
usb_gadget_open (struct usb_gadget_device *device)
{
  struct usb_gadget_dev_handle *handle;

  if (!device || !device->device || !device->config)
    {
      errno = EINVAL;
      return NULL;
    }

  handle = malloc (sizeof(*handle));
  if (!handle)
    goto error;
  handle->device = device;

  handle->ep0 = find_ep0 (handle);
  if (!handle->ep0)
    goto error;

  if (open_ep0 (handle) < 0)
    goto error;

  usb_gadget_init_list_head (&handle->ep_list);
  return handle;

 error:
  if (handle->ep0)
    {
      close_ep (handle->ep0);
      free (handle->ep0);
      handle->ep0 = NULL;
    }
  free (handle);
  return NULL;
}

int
usb_gadget_close (usb_gadget_dev_handle *handle)
{
  struct _usb_gadget_endpoint *ep;

  if (!handle || !handle->ep0)
    {
      errno = EINVAL;
      return -1;
    }
  close_ep (handle->ep0);
  free (handle->ep0);
  handle->ep0 = NULL;

  usb_gadget_list_for_each_entry (ep, &handle->ep_list, ep_list)
    {
      close_ep (ep);
      free (ep);
      ep = NULL;
    }
  
  return 0;
}

struct usb_gadget_endpoint *
usb_gadget_endpoint (usb_gadget_dev_handle *handle, int number)
{
  struct _usb_gadget_endpoint *ep;

  if (number == 0)
    return &handle->ep0->ep;
  
  usb_gadget_list_for_each_entry (ep, &handle->ep_list, ep_list)
    if ((ep->descriptor->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK) == number)
      return &ep->ep;

  return NULL;
}

static int
set_config (usb_gadget_dev_handle *handle, int value)
{
  struct usb_descriptor_header **header;
  struct usb_config_descriptor *config;
  struct _usb_gadget_endpoint *ep;

  if (value == 0)
    {
      usb_gadget_list_for_each_entry (ep, &handle->ep_list, ep_list)
	{
	  int number;

	  number = ep->descriptor->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK;

	  close_ep (ep);
	  if (handle->event_cb)
	    {
	      struct usb_gadget_event event;

	      event.type = USG_EVENT_ENDPOINT_DISABLE;
	      event.u.number = number;
	      handle->event_cb (handle, &event, handle->event_arg);
	    }
	}
      return 0;
    }

  config = (struct usb_config_descriptor *)handle->device->config[0];
  if (value != config->bConfigurationValue)
    {
      errno = EINVAL;
      return -1;
    }

  for (header = handle->device->config; *header; header++)
    {
      struct usb_descriptor_header **hs_header;
      struct usb_endpoint_descriptor *descriptor = NULL, *hs_descriptor = NULL;
      struct _usb_gadget_endpoint *ep;
      int number;

      if ((*header)->bDescriptorType != USB_DT_ENDPOINT)
	continue;
      descriptor = (struct usb_endpoint_descriptor *)*header;
      number = descriptor->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK;
      assert (number);

      if (handle->device->hs_config)
	{
	  for (hs_header = handle->device->hs_config; *hs_header; hs_header++)
	    {
	      if ((*hs_header)->bDescriptorType != USB_DT_ENDPOINT)
		continue;
	      hs_descriptor = (struct usb_endpoint_descriptor *)*hs_header;
	      if ((hs_descriptor->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)
		  == number)
		break;
	    }
	  if (!*hs_header)
	    hs_descriptor = NULL;
	}
      ep = find_ep (handle, descriptor);
      if (!ep)
	{
	  debug (handle, 2, "libusb-gadget: set_config: find_ep failed\n");
	  return -1;
	}
      if (open_ep (ep, descriptor, hs_descriptor) < 0)
	{
	  debug (handle, 2, "libusb-gadget: set_config: %s open failed\n",
		 ep->ep.name);
	  close_ep (ep);
	  return -1;
	}
      debug (handle, 2, "libusb-gadget: set_config: %s opened\n", ep->ep.name);
      ep->descriptor = descriptor;
      ep->hs_descriptor = hs_descriptor;
      ep->handle = handle;
      if (handle->event_cb)
	{
	  struct usb_gadget_event event;

	  event.type = USG_EVENT_ENDPOINT_ENABLE;
	  event.u.number = number;
	  handle->event_cb (handle, &event, handle->event_arg);
	}
    }
  return 0;
}

static void
setup (struct usb_gadget_dev_handle *handle, struct usb_ctrlrequest *ctrl)
{
  int ret;
  uint8_t buf[256];
  uint16_t value = usb_gadget_le16_to_cpu(ctrl->wValue);
  uint16_t index = usb_gadget_le16_to_cpu(ctrl->wIndex);
  uint16_t length = usb_gadget_le16_to_cpu(ctrl->wLength);
  struct _usb_gadget_endpoint *ep;

  debug (handle, 2,
	 "libusb-gadget: setup: ctrl->bRequestType = %d, ctrl->bRequest = %d, "
	 "ctrl->wValue = %d, ctrl->wIndex = %d, ctrl->wLength = %d\n",
	 ctrl->bRequestType, ctrl->bRequest, value, index, length);
  switch (ctrl->bRequestType & USB_TYPE_MASK)
    {
    case USB_TYPE_STANDARD:
      switch (ctrl->bRequest)
	{
	case USB_REQ_GET_DESCRIPTOR:
	  if (ctrl->bRequestType != USB_DIR_IN)
	    goto stall;
	  switch (value >> 8)
	    {
	    case USB_DT_DEVICE:
	      ret = sizeof(struct usb_device_descriptor);
	      if (ret > length)
		ret = length;
	      write (handle->ep0->fd, handle->device->device, ret);
	      break;
	    case USB_DT_CONFIG:
	      ret = config_buf (buf, value >> 8,
				handle->device->hs_config ?
				handle->device->hs_config :
				handle->device->config);
	      if (ret < 0)
		goto stall;
	      write (handle->ep0->fd, buf, ret);
	      break;
	    case USB_DT_STRING:
	      ret = usb_gadget_get_string (handle->device->strings, value & 0xff, buf);
	      if (ret < 0)
		goto stall;
	      if (ret > length)
		ret = length;
	      write (handle->ep0->fd, buf, ret);
	      break;
	    default:
	      goto stall;
	    }
	  return;

	case USB_REQ_SET_CONFIGURATION:
	  if (ctrl->bRequestType != USB_DIR_OUT)
	    goto stall;
	  if (set_config (handle, value) < 0)
	    {
	      debug (handle, 2, "libusb-gadget: setup: set_config failed\n");
	      goto stall;
	    }

	  read (handle->ep0->fd, &ret, 0);
	  return;
	case USB_REQ_GET_INTERFACE:
	  if (ctrl->bRequestType != (USB_DIR_IN|USB_RECIP_INTERFACE)
	      || index != 0
	      || length > 1)
	    goto stall;

	  buf[0] = 0;
	  write (handle->ep0->fd, buf, length);
	  return;
	case USB_REQ_SET_INTERFACE:
	  if (ctrl->bRequestType != USB_RECIP_INTERFACE
	      || index != 0
	      || value != 0)
	    goto stall;

	  ret = 0;
	  usb_gadget_list_for_each_entry (ep, &handle->ep_list, ep_list)
	    {
	      debug (handle, 2, "libusb-gadget: setup: clear halt %s %d %d\n",
		     ep->ep.name, ep->fd, ret);
	      if (ep->fd > 0
		  /* FIXME: dummy_udc and musb_hdrc don't return from
		     this ioctl */
		  && (strcmp (handle->ep0->ep.name, "dummy_udc")
		      || strcmp (handle->ep0->ep.name, "musb_hdrc"))
		  && ioctl (ep->fd, GADGETFS_CLEAR_HALT) < 0)
		ret = -1;
	    }
	  if (ret < 0)
	    goto stall;

	  read (handle->ep0->fd, &ret, 0);
	  return;
	default:
	  goto stall;
	}
      break;
    }

stall:
  if (ctrl->bRequestType & USB_DIR_IN)
    read (handle->ep0->fd, &ret, 0);
  else
    write (handle->ep0->fd, &ret, 0);
}

int
usb_gadget_handle_control_event (usb_gadget_dev_handle *handle)
{
  struct usb_gadgetfs_event events[NEVENT];
  struct usb_gadget_event event;
  int ret, nevent, i;

  ret = read (handle->ep0->fd, &events, sizeof(events));
  if (ret < 0)
    return ret;

  nevent = ret / sizeof(events[0]);
  debug (handle, 2, "libusb-gadget: %d events received\n", nevent);
  for (i = 0; i < nevent; i++)
    {
      debug (handle, 2, "libusb-gadget: event %d\n", events[i].type);
      switch (events[i].type)
	{
	case GADGETFS_SETUP:
	  setup (handle, &events[i].u.setup);
	  break;
	case GADGETFS_NOP:
	  break;
	case GADGETFS_CONNECT:
	  if (handle->event_cb)
	    {
	      event.type = USG_EVENT_CONNECT;
	      handle->speed = events[i].u.speed;
	      debug (handle, 2, "libusb-gadget: connected with speed %d\n",
		     handle->speed);
	      handle->event_cb (handle, &event, handle->event_arg);
	    }
	  break;
	case GADGETFS_DISCONNECT:
	  if (handle->event_cb)
	    {
	      handle->speed = USB_SPEED_UNKNOWN;
	      event.type = USG_EVENT_DISCONNECT;
	      handle->event_cb (handle, &event, handle->event_arg);
	    }
	  break;
	case GADGETFS_SUSPEND:
	  if (handle->event_cb)
	    {
	      event.type = USG_EVENT_SUSPEND;
	      handle->event_cb (handle, &event, handle->event_arg);
	    }
	  break;
	default:
	  break;
	}
    }
}

ssize_t
usb_gadget_endpoint_write (struct usb_gadget_endpoint *ep, const void *buf, size_t len,
		    int timeout)
{
  struct _usb_gadget_endpoint *_ep;
  struct usb_endpoint_descriptor *descriptor;

  _ep = usb_gadget_container_of(ep, struct _usb_gadget_endpoint, ep);
  if (_ep->fd < 0)
    {
      debug (_ep->handle, 2, "libusb-gadget: usb_gadget_endpoint_write: %s is closed\n",
	     ep->name);
      errno = EINVAL;
      return -1;
    }

  if (_ep->handle->speed == USB_SPEED_HIGH)
    descriptor = _ep->hs_descriptor;
  else
    descriptor = _ep->descriptor;

  if (len > usb_gadget_le16_to_cpu(descriptor->wMaxPacketSize))
    {
      debug (_ep->handle, 2, "libusb-gadget: usb_gadget_endpoint_write: too long message\n");
      errno = EINVAL;
      return -1;
    }

  return write (_ep->fd, buf, len);
}

ssize_t
usb_gadget_endpoint_read (struct usb_gadget_endpoint *ep, void *buf, size_t len,
		   int timeout)
{
  struct _usb_gadget_endpoint *_ep;
  struct usb_endpoint_descriptor *descriptor;

  _ep = usb_gadget_container_of(ep, struct _usb_gadget_endpoint, ep);
  if (_ep->fd < 0)
    {
      debug (_ep->handle, 2, "libusb-gadget: usb_gadget_endpoint_read: %s is closed\n",
	     ep->name);
      errno = EINVAL;
      return -1;
    }

  if (_ep->handle->speed == USB_SPEED_HIGH)
    descriptor = _ep->hs_descriptor;
  else
    descriptor = _ep->descriptor;
  
  if (len > usb_gadget_le16_to_cpu(descriptor->wMaxPacketSize))
    {
      debug (_ep->handle, 2, "libusb-gadget: usb_gadget_endpoint_read: too long message\n");
      errno = EINVAL;
      return -1;
    }

  return read (_ep->fd, buf, len);
}

void
usb_gadget_set_event_cb (usb_gadget_dev_handle *handle, usb_gadget_event_cb cb, void *arg)
{
  handle->event_cb = cb;
  handle->event_arg = arg;
}

int
usb_gadget_control_fd (usb_gadget_dev_handle *handle)
{
  return handle->ep0->fd;
}
