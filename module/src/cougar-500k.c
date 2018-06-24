/*
 *  Copyright (c) 2018 Daniel M. Lambea
 *
 *  Cougar 500k Gaming Keyboard support
 *
 *  Inspired by, and parts copied from, aziokbd.c, Copyright (c) 2013 Colin Svingen
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Should you need to contact me, the author, you can do so by email.
 * Mail your message to Daniel M. Lambea <dmlambea@gmail.com>
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb/input.h>
#include <linux/hid.h>
#include <linux/usb.h>

/*
 * Version Information
 */
#define DRIVER_NAME                "cougar_500k"
#define DRIVER_VERSION             "0.5"
#define DRIVER_AUTHOR              "Daniel M. Lambea <dmlambea@gmail.com>"
#define DRIVER_DESC                "Cougar 500k Gaming Keyboard Driver"
#define DRIVER_LICENSE             "GPL"

#define USB_VENDOR_ID_COUGAR       0x060b
#define USB_DEVICE_ID_COUGAR_500K  0x500a

#define COUGAR_KEYB_INTFNO         0
#define COUGAR_RESERVED_INTFNO     2

#define COUGAR_RESERVED_FLD_CODE   1
#define COUGAR_RESERVED_FLD_ACTION 2

#define COUGAR_KEY_G1              0x83
#define COUGAR_KEY_G2              0x84
#define COUGAR_KEY_G3              0x85
#define COUGAR_KEY_G4              0x86
#define COUGAR_KEY_G5              0x87
#define COUGAR_KEY_G6              0x78
#define COUGAR_KEY_FN              0x0d
#define COUGAR_KEY_MR              0x6f
#define COUGAR_KEY_M1              0x80
#define COUGAR_KEY_M2              0x81
#define COUGAR_KEY_M3              0x82
#define COUGAR_KEY_LEDS            0x67
#define COUGAR_KEY_LOCK            0x6e

static struct hid_device_id cougar_id_table[] = {
    { HID_USB_DEVICE(USB_VENDOR_ID_COUGAR, USB_DEVICE_ID_COUGAR_500K) },
    {}
};

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE(DRIVER_LICENSE);
MODULE_DEVICE_TABLE(hid, cougar_id_table);

static __u8 cougar_500k_rdesc_reserved_intf_fixup[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x06,        // Usage (Keyboard)
    0xA1, 0x01,        // Collection (Application)
    0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x09, 0x30,        //   Usage (0x30)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x08,        //   Report Count (8)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x31,        //   Usage (0x31)
    0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              // End Collection
};

/* Default key mappings */
static unsigned char cougar_mapping[][2] = {
    { COUGAR_KEY_G1,   KEY_F13 },
    { COUGAR_KEY_G2,   KEY_F13 },
    { COUGAR_KEY_G3,   KEY_F13 },
    { COUGAR_KEY_G4,   KEY_F13 },
    { COUGAR_KEY_G5,   KEY_F13 },
    { COUGAR_KEY_G6,   KEY_SPACE },
    { COUGAR_KEY_LOCK, KEY_SCREENLOCK },
    { 0, 0 },
};

/* This struct holds the input device from the keyboard interface (intf COUGAR_KEYB_INTFNO) */
/* to be used by faked keyboard intf COUGAR_RESERVED_INTFNO. */
struct cougar_data {
    struct hid_device *owner;
    struct input_dev  *input;
};

/**
 * cougar_apply_max_usage_fixup() - rdesc fixup for usage counts higher than max. allowed.
 *
 * @hdev: the device for debugging purposes
 * @usage: pointer to the usage value to be modified, if needed
 */ 
static inline void cougar_apply_max_usage_fixup(struct hid_device *hdev, __u16 *usage) {
    if (*usage >= (HID_MAX_USAGES-1) ) {
        hid_info(hdev, "%x usages exceeds max. allowed of %x: fixup applied", *usage, (HID_MAX_USAGES-1) );
        *usage = (HID_MAX_USAGES-1);
    }
}

/**
 * cougar_report_fixup() - rdesc fixup for intf 1 and 2
 *
 * @hdev: the device
 * @rdesc: pointer to current rdesc from the device
 * @rsize: pointer to rdesc's size
 */
static __u8 *cougar_report_fixup(struct hid_device *hdev, __u8 *rdesc, unsigned int *rsize) {
    struct usb_interface *intf = to_usb_interface(hdev->dev.parent);

    /* Check for the correct device */
    if (hdev->vendor != USB_VENDOR_ID_COUGAR ||
        hdev->product != USB_DEVICE_ID_COUGAR_500K) {
        hid_warn(hdev, "report descriptor fixup not applied, unsupported device: %x:%x", hdev->vendor, hdev->product);
        return rdesc;
    }

    switch (intf->cur_altsetting->desc.bInterfaceNumber) {
    case 1:
        /* Overflowing usage is at position 0x73 */
        cougar_apply_max_usage_fixup(hdev, (__u16 *)(&rdesc[0x73]));
        break;
    case COUGAR_RESERVED_INTFNO:
        /* Reserved interface rdesc must be completely replaced for a fake keyboard descriptor */
        rdesc = cougar_500k_rdesc_reserved_intf_fixup;
        *rsize = sizeof(cougar_500k_rdesc_reserved_intf_fixup);
        break;
    }
    return rdesc;
}

/**
 * cougar_get_sibling_hid_device() - returns the sibling hid_device for a given intf number
 *
 * @hdev: the current device which a sibling hdev is being searched for
 * @intfno: the intf number of the hid_device we're looking for
 */
static struct hid_device *cougar_get_sibling_hid_device(struct hid_device *hdev, const __u8 intfno) {
    struct usb_interface *intf = to_usb_interface(hdev->dev.parent);
    struct usb_device *device;
    struct usb_host_config *hostcfg;
    struct hid_device *siblingHdev;
    int i;

    if (intf->cur_altsetting->desc.bInterfaceNumber == intfno) {
        hid_warn(hdev, "returning hid device's data from myself?");
    }

    device = interface_to_usbdev(intf);
    hostcfg = device->config;
    siblingHdev = NULL;
    for (i = 0; i < hostcfg->desc.bNumInterfaces; i++) {
        if (i == intfno) {
            return usb_get_intfdata(hostcfg->interface[i]);
        }
    }
    return NULL;
}

/**
 * cougar_probe() - probe function
 *
 * @hdev: the current device which a sibling hdev is being searched for
 * @intfno: the intf number of the hid_device we're looking for
 */
static int cougar_probe(struct hid_device *hdev, const struct hid_device_id *id) {
    struct cougar_data *internal = NULL;
    struct usb_device *device;
    struct hid_device *siblingHdev;
    int ret;

    struct usb_interface *intf = to_usb_interface(hdev->dev.parent);
    hid_dbg(hdev, "about to probe intf %d", intf->cur_altsetting->desc.bInterfaceNumber);

    /* Cougar internal data structure must be created before probing COUGAR_KEYB_INTFNO
     * because the cougar_mapping_configured will want this struct to exist
     * so that it can set the input device to be located later in COUGAR_RESERVED_INTFNO probe.
     */
    if (intf->cur_altsetting->desc.bInterfaceNumber == COUGAR_KEYB_INTFNO) {
        internal = devm_kzalloc(&hdev->dev, sizeof(*internal), GFP_KERNEL);
        if (internal == NULL) {
            hid_err(hdev, "can't alloc keyboard descriptor");
            return -ENOMEM;
        }
        hid_set_drvdata(hdev, internal);
        hid_dbg(hdev, "driver data set to %px", internal);
    }

    /* Parse and start hw */
    ret = hid_parse(hdev);
    if (ret) {
        goto err;
    }
    ret = hid_hw_start(hdev, HID_CONNECT_DEFAULT);
    if (ret) {
        goto err;
    }

    /* Before finishing intf COUGAR_RESERVED_INTFNO probe, cougar internal data has to be located and
     * remembered. The descriptor ownership has to be retained by intf COUGAR_KEYB_INTFNO, so that
     * its .remove hook can free the associated memory.
     */
    if (intf->cur_altsetting->desc.bInterfaceNumber == COUGAR_RESERVED_INTFNO) {
        siblingHdev = cougar_get_sibling_hid_device(hdev, COUGAR_KEYB_INTFNO);
        if (siblingHdev == NULL) {
            hid_err(hdev, "no sibling hid device found for intf %d", COUGAR_KEYB_INTFNO);
            ret = -ENODEV;
            goto err;
        }

        internal = hid_get_drvdata(siblingHdev);
        if (internal == NULL || internal->input == NULL) {
            hid_err(hdev, "keyboard descriptor not found in intf %d", COUGAR_RESERVED_INTFNO);
            ret = -ENODATA;
            goto err;
        }
        hid_set_drvdata(hdev, internal);
        hid_dbg(hdev, "keyboard descriptor attached to intf %d", COUGAR_RESERVED_INTFNO);
    }
    hid_dbg(hdev, "intf %d probed successfully", intf->cur_altsetting->desc.bInterfaceNumber);

    return 0;
err:
    hid_set_drvdata(hdev, NULL);
    if (internal != NULL) {
        if (internal->owner == hdev) {
            devm_kfree(&hdev->dev, internal);
        }
    }
    return ret;
}

/**
 * cougar_input_configured() - keeps track of the input device associated with intf COUGAR_KEYB_INTFNO
 */
static int cougar_input_configured(struct hid_device *hdev, struct hid_input *hidinput) {
    struct cougar_data *internal;
    struct usb_interface *intf = to_usb_interface(hdev->dev.parent);
    if (intf->cur_altsetting->desc.bInterfaceNumber != COUGAR_KEYB_INTFNO) {
        /* Skip interfaces other than COUGAR_KEYB_INTFNO, which is the one containing the configured hidinput */
        hid_dbg(hdev, "skipping intf %d", intf->cur_altsetting->desc.bInterfaceNumber);
        return 0;
    }
    internal = hid_get_drvdata(hdev);
    internal->owner = hdev;
    internal->input = hidinput->input;
    hid_dbg(hdev, "intf %d internal data configured", intf->cur_altsetting->desc.bInterfaceNumber);
    return 0;
}

/**
 * cougar_raw_event() - processing of COUGAR_RESERVED_INTFNO special keystrokes
 */
static int cougar_raw_event(struct hid_device *hdev, struct hid_report *report, u8 *data, int size) {
    struct usb_interface *intf = to_usb_interface(hdev->dev.parent);
    struct cougar_data *internal;
    char code, action;
    int i;

    if (intf->cur_altsetting->desc.bInterfaceNumber != COUGAR_RESERVED_INTFNO) {
        return 0;
    }

    // Enable this to see a dump of the data received from reserved interface 
    // print_hex_dump(KERN_INFO, DRIVER_NAME " data : ", DUMP_PREFIX_OFFSET, 8, 1, data, size, 0);
    code = data[COUGAR_RESERVED_FLD_CODE];
    action = data[COUGAR_RESERVED_FLD_ACTION];

    /* Try normal key mappings */
    for (i = 0; cougar_mapping[i][0]; i++) {
        if (cougar_mapping[i][0] == code) {
            internal = hid_get_drvdata(hdev);
            input_event(internal->input, EV_KEY, cougar_mapping[i][1], action);
            input_sync(internal->input);
            return -1;
        }
    }

    /* Reserved key: must be processed individually */
    switch(code) {
    case COUGAR_KEY_FN:
        hid_dbg(hdev, "FN (special function) key is handled by the hardware itself");
        break;
    case COUGAR_KEY_MR:
        hid_dbg(hdev, "MR (macro recording) key is handled by the hardware itself");
        break;
    case COUGAR_KEY_M1:
        hid_dbg(hdev, "M1 (macro set 1) key is handled by the hardware itself");
        break;
    case COUGAR_KEY_M2:
        hid_dbg(hdev, "M2 (macro set 2) key is handled by the hardware itself");
        break;
    case COUGAR_KEY_M3:
        hid_dbg(hdev, "M3 (macro set 3) key is handled by the hardware itself");
        break;
    case COUGAR_KEY_LEDS:
        hid_dbg(hdev, "LED (led set) key is handled by the hardware itself");
        break;
    }
    return -1;
}

/**
 * cougar_remove() - frees the cougar internal data
 */
static void cougar_remove(struct hid_device *hdev) {
    struct cougar_data *internal = hid_get_drvdata(hdev);
    hid_set_drvdata(hdev, NULL);
    if (internal != NULL && internal->owner == hdev) {
        devm_kfree(&hdev->dev, internal);
    }
    hid_hw_stop(hdev);
}

/************************************************************************/

static struct hid_driver cougar_driver = {
    .name             = DRIVER_NAME,
    .id_table         = cougar_id_table,
    .report_fixup     = cougar_report_fixup,
    .probe            = cougar_probe,
    .input_configured = cougar_input_configured,
    .remove           = cougar_remove,
    .raw_event        = cougar_raw_event,
};

module_hid_driver(cougar_driver);
