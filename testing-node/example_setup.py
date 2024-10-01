from gs_usb.gs_usb import GsUsb
from gs_usb.gs_usb_frame import GsUsbFrame
from gs_usb.constants import (
    CAN_EFF_FLAG,
    CAN_ERR_FLAG,
    CAN_RTR_FLAG,
)

def main():
    # Find our device
    devs = GsUsb.scan()
    if len(devs) == 0:
        print("Can not find gs_usb device")
        return
    dev = devs[0]

    # Configuration
    if not dev.set_bitrate(500_000):
        print("Can not set bitrate for gs_usb")
        return
    

    # Start device
    dev.start()

    node_id = input("set an id for this device")
    try:
        if len(node_id) != 3:
            raise ValueError
        node_id = int(node_id, 16)
    except ValueError:
        print("id was not a hex string or not the right len")
        return

    while True:
        message = input("choose a message")
        try:
            if len(message) > 16:
                raise ValueError
            message = int(node_id, 16)

        except ValueError:
            print("Usage: ")
            continue
        