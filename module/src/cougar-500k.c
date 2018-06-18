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
#define DRIVER_NAME "cougar_500k"
#define DRIVER_VERSION "0.4"
#define DRIVER_AUTHOR "Daniel M. Lambea <dmlambea@gmail.com>"
#define DRIVER_DESC "Cougar 500k Gaming Keyboard Driver"
#define DRIVER_LICENSE "GPL"

#define VENDOR_ID_COUGAR 0x060b
#define PRODUCT_ID_500K  0x500a

static struct hid_device_id cougar_id_table[] = {
    { HID_DEVICE(HID_BUS_ANY, HID_GROUP_ANY, VENDOR_ID_COUGAR, PRODUCT_ID_500K) },
    {}
};

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE(DRIVER_LICENSE);
// MODULE_SUPPORTED_DEVICE("{{Cougar,500k}}}");
MODULE_DEVICE_TABLE(hid, cougar_id_table);

static __u8 cougar_500k_rdesc_fixed[] = {
    0x05, 0x01, 0x09, 0x02, 0xa1, 0x01, 0x85, 0x01, 0x09, 0x01, 0xa1, 0x00,

    0x05, 0x09, 0x19, 0x01, 0x29, 0x05, 0x15, 0x00, 0x25, 0x01, 0x75, 0x01,
    0x95, 0x05, 0x81, 0x02, 0x75, 0x01, 0x95, 0x03, 0x81, 0x01, 

    0x05, 0x01, 0x09, 0x30, 0x09, 0x31, 0x16, 0x01, 0x80,
    0x26, 0xff, 0x2f,            /* Previously:   0x26, 0xff, 0x7f,   */
    0x75, 0x10, 0x95, 0x02, 0x81, 0x06, 0x09, 0x38, 0x15, 0x81, 0x25, 0x7f,
    0x75, 0x08, 0x95, 0x01, 0x81, 0x06, 0x05, 0x0c, 0x0a, 0x38, 0x02,
    0x95, 0x01, 0x81, 0x06, 0xc0, 0xc0,

    0x05, 0x01, 0x09, 0x80, 0xa1, 0x01, 0x85, 0x02, 0x19, 0x81, 0x29, 0x83,
    0x15, 0x00, 0x25, 0x01, 0x75, 0x01, 0x95, 0x03, 0x81, 0x02, 0x75, 0x05,
    0x95, 0x01, 0x81, 0x01, 0xc0, 

    0x05, 0x0c, 0x09, 0x01, 0xa1, 0x01, 0x85, 0x03, 0x19, 0x00,
    0x2a, 0xff, 0x2f,            /* Previously:   0x2a, 0xff, 0x7f,   */
    0x15, 0x00, 
    0x26, 0xff, 0x2f,            /* Previously:   0x26, 0xff, 0x7f,   */
    0x75, 0x10, 0x95, 0x03, 0x81, 0x00, 0x75, 0x08, 0x95, 0x01, 0x81, 0x01,
    0xc0,

    0x06, 0x00, 0xff, 0x09, 0x01, 0xa1, 0x01, 0x85, 0x06, 0x15, 0x00,
    0x26, 0xff, 0x00, 0x09, 0x30, 0x95, 0x03, 0x75, 0x08, 0x81, 0x02,
    0xc0,

    0x06, 0x01, 0xff, 0x09, 0x01, 0xa1, 0x01, 0x85, 0x07, 0x15, 0x00,
    0x26, 0xff, 0x00, 0x09, 0x20, 0x75, 0x08, 0x95, 0x07, 0xb1, 0x02,
    0xc0,

    0x05, 0x01, 0x09, 0x06, 0xa1, 0x01, 0x85, 0x09, 0x95, 0x70, 0x75, 0x01,
    0x15, 0x00, 0x25, 0x01, 0x05, 0x07, 0x19, 0xe0, 0x29, 0xe7, 0x19, 0x00,
    0x29, 0x67, 0x81, 0x02, 0x95, 0x08, 0x75, 0x01, 0x81, 0x01, 0xc0,

    0x05, 0x01, 0x09, 0x07, 0xa1, 0x01, 0x85, 0x0a, 0x05, 0x07, 0x75, 0x08,
    0x95, 0x01, 0x81, 0x01, 0x05, 0x07, 0x19, 0x00, 0x2a, 0xff, 0x00,
    0x15, 0x00, 0x26, 0xff, 0x00, 0x75, 0x08, 0x95, 0x06, 0x81, 0x00,
    0xc0,

    0x05, 0x01, 0x09, 0x07, 0xa1, 0x01, 0x85, 0x0b, 0x05, 0x07, 0x75, 0x08,
    0x95, 0x01, 0x81, 0x01, 0x05, 0x07, 0x19, 0x00, 0x2a, 0xff, 0x00, 0x15, 0x00,
    0x26, 0xff, 0x00, 0x75, 0x08, 0x95, 0x06, 0x81, 0x00, 0xc0,

    0x05, 0x01, 0x09, 0x07, 0xa1, 0x01, 0x85, 0x0c, 0x05, 0x07, 0x75, 0x08,
    0x95, 0x01, 0x81, 0x01, 0x05, 0x07, 0x19, 0x00, 0x2a, 0xff, 0x00,
    0x15, 0x00, 0x26, 0xff, 0x00, 0x75, 0x08, 0x95, 0x06, 0x81, 0x00, 0xc0,

    0x05, 0x01, 0x09, 0x07, 0xa1, 0x01, 0x85, 0x0d, 0x05, 0x07, 0x75, 0x08,
    0x95, 0x01, 0x81, 0x01, 0x05, 0x07, 0x19, 0x00, 0x2a, 0xff, 0x00,
    0x15, 0x00, 0x26, 0xff, 0x00, 0x75, 0x08, 0x95, 0x06, 0x81, 0x00, 0xc0
};

/* Special keys G1 - G6, Lock and Fn */
static const unsigned char cougar_special_keycode[8] = {
    0x83, 0x84, 0x85, 0x86, 0x87, 0x78, 0x6e, 0x0d
};

/* Keys the special keys are mapped to (G1 -> F1, etc) */
static const unsigned char cougar_translated_keycode[8] = {
    KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_SPACE, KEY_SCREENLOCK, KEY_RESERVED
};

struct cougar_kbd {
    struct input_dev *dev;
    struct usb_device *usbdev;
    struct urb *irq;
    char name[128];
    char phys[64];

    unsigned char *new;
    dma_addr_t new_dma;
};

/* Device's rdesc comes with a too high value for come report usage counts, so this
 * fixup lowers it to the allowed max.
 * TODO: use the kernel constant value for this, instead of hardcoding the 0x2fff.
 */
static __u8 *cougar_report_fixup(struct hid_device *hdev, __u8 *rdesc, unsigned int *rsize) {
    struct usb_interface *intf = to_usb_interface(hdev->dev.parent);

    if (intf->cur_altsetting->desc.bInterfaceNumber == 1) {
        if (rdesc[43] == 0x26 && rdesc[44] == 0xff && rdesc[45] == 0x7f) {
            rdesc = cougar_500k_rdesc_fixed;
            *rsize = sizeof(cougar_500k_rdesc_fixed);
        } else {
            hid_warn(hdev, "report descriptor fixup not applied, wrong device?");
        }
    }
    return rdesc;
}

static void cougar_irq(struct urb *urb) {
    struct cougar_kbd *kbd = urb->context;
    int i;

    if (urb->status != 0) {
        return;
    }

    /* Some debug info to see extended keycodes sent via interface 2 */
    print_hex_dump(KERN_INFO, DRIVER_NAME " keypress: ", DUMP_PREFIX_OFFSET, 8, 1, kbd->new, 8, 0);
    if (kbd->new[0] == 7) {
        for (i=0; i<8; i++) {
            if (kbd->new[1] == cougar_special_keycode[i]) {
                input_report_key(kbd->dev, cougar_translated_keycode[i], kbd->new[2]);
                break;
            }
        }
    }
    input_sync(kbd->dev);

    i = usb_submit_urb(urb, GFP_ATOMIC);
    if (i) {
        hid_err(urb->dev, "can't resubmit intr, %s-%s/input0, status %d",
            kbd->usbdev->bus->bus_name, kbd->usbdev->devpath, i);
    }
}

static int cougar_alloc_mem(struct usb_device *dev, struct cougar_kbd *kbd) {
    kbd->irq = usb_alloc_urb(0, GFP_KERNEL);
    if (!kbd->irq) {
        return -1;
    }
    kbd->new = usb_alloc_coherent(dev, 8, GFP_ATOMIC, &kbd->new_dma);
    if (!kbd->new) {
        return -1;
    }

    return 0;
}

static void cougar_free_mem(struct usb_device *dev, struct cougar_kbd *kbd) {
    printk("cougar_500k: freeing mem");
    usb_free_urb(kbd->irq);
    usb_free_coherent(dev, 8, kbd->new, kbd->new_dma);
}

static int cougar_event(struct input_dev *dev, unsigned int type,
             unsigned int code, int value) {
//    struct cougar_kbd *kbd = input_get_drvdata(dev);

    printk("cougar_500k: event type %d", type);
    return -1;
}

static int cougar_open(struct input_dev *dev) {
    struct cougar_kbd *kbd = input_get_drvdata(dev);
    kbd->irq->dev = kbd->usbdev;
    if (usb_submit_urb(kbd->irq, GFP_KERNEL)) {
        return -EIO;
    }
    return 0;
}

static void cougar_close(struct input_dev *dev) {
    struct cougar_kbd *kbd = input_get_drvdata(dev);
    usb_kill_urb(kbd->irq);
}

static int cougar_probe(struct hid_device *hdev, const struct hid_device_id *id) {
    struct usb_host_interface *interface;
    struct usb_interface *iface;
    struct usb_endpoint_descriptor *endpoint;
    struct usb_device *dev;
    struct input_dev *input_dev;
    struct cougar_kbd *kbd;
    int i, pipe, maxp;
    int error = -ENOMEM;

    if (hid_parse(hdev)) {
        hid_err(hdev, "HID parse failed");
    }

    if (hid_hw_start(hdev, HID_CONNECT_DEFAULT)) {
        hid_err(hdev, "HID hardware start failed");
    }

    iface = to_usb_interface(hdev->dev.parent);
    interface = iface->cur_altsetting;
    if (interface->desc.bInterfaceNumber != 2) {
        return 0;
    }

    hid_info(hdev, "installing interface 2");
    endpoint = &interface->endpoint[0].desc;
    if (!usb_endpoint_is_int_in(endpoint))
        return -ENODEV;

    dev = interface_to_usbdev(iface);
    pipe = usb_rcvintpipe(dev, endpoint->bEndpointAddress);
    maxp = usb_maxpacket(dev, pipe, usb_pipeout(pipe));
    kbd = kzalloc(sizeof(struct cougar_kbd), GFP_KERNEL);
    input_dev = input_allocate_device();
    if (!kbd || !input_dev){
        goto fail1;
    }

    if (cougar_alloc_mem(dev, kbd)){
        goto fail2;
    }

    kbd->usbdev = dev;
    kbd->dev = input_dev;

    if (dev->manufacturer)
        strlcpy(kbd->name, dev->manufacturer, sizeof(kbd->name));

    if (dev->product) {
        if (dev->manufacturer)
            strlcat(kbd->name, " ", sizeof(kbd->name));
        strlcat(kbd->name, dev->product, sizeof(kbd->name));
    }

    if (!strlen(kbd->name)) {
        snprintf(kbd->name, sizeof(kbd->name),
             "Cougar 500k Gaming Keyboard %04x:%04x",
             le16_to_cpu(dev->descriptor.idVendor),
             le16_to_cpu(dev->descriptor.idProduct));
    }

    hid_info(hdev, "detected %s", kbd->name);

    usb_make_path(dev, kbd->phys, sizeof(kbd->phys));
    strlcat(kbd->phys, "/input0", sizeof(kbd->phys));

    input_dev->name = kbd->name;
    input_dev->phys = kbd->phys;
    usb_to_input_id(dev, &input_dev->id);
    input_dev->dev.parent = &iface->dev;

    input_set_drvdata(input_dev, kbd);

    input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);

    for (i = 0; i < 255; i++) {
        set_bit(i, input_dev->keybit);
    }
    clear_bit(0, input_dev->keybit);

    input_dev->event = cougar_event;
    input_dev->open = cougar_open;
    input_dev->close = cougar_close;

    usb_fill_int_urb(kbd->irq, dev, pipe,
             kbd->new, (maxp > 8 ? 8 : maxp),
             cougar_irq, kbd, endpoint->bInterval);
    kbd->irq->transfer_dma = kbd->new_dma;
    kbd->irq->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
    error = input_register_device(kbd->dev);
    if (error){
        goto fail2;
    }

    usb_set_intfdata(iface, kbd);
    device_set_wakeup_enable(&dev->dev, 1);
    return 0;

fail2:
    cougar_free_mem(dev, kbd);
fail1:
    input_free_device(input_dev);
    kfree(kbd);
    return error;
}

static void cougar_remove(struct hid_device *hdev) {
    struct usb_interface *iface;
    struct cougar_kbd *kbd;

    iface = to_usb_interface(hdev->dev.parent);
    if (iface->cur_altsetting->desc.bInterfaceNumber != 2) {
        return;
    }

    kbd = hid_get_drvdata(hdev);
    hid_set_drvdata(hdev, NULL);
    if (kbd) {
        usb_kill_urb(kbd->irq);
        input_unregister_device(kbd->dev);
        cougar_free_mem(interface_to_usbdev(iface), kbd);
        kfree(kbd);
    }
}

/************************************************************************/

static struct hid_driver cougar_driver = {
    .name         =    DRIVER_NAME,
    .id_table     =    cougar_id_table,
    .probe        =    cougar_probe,
    .report_fixup = cougar_report_fixup,
    .remove       = cougar_remove,
};

module_hid_driver(cougar_driver);
