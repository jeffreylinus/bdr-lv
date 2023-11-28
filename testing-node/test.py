
'''
import time
import can
from gs_usb.gs_usb import GsUsb
from gs_usb.gs_usb_frame import GsUsbFrame
from gs_usb.constants import (
    CAN_EFF_FLAG,
    CAN_ERR_FLAG,
    CAN_RTR_FLAG,
)


# Find our device
devs = GsUsb.scan()
if len(devs) == 0:
    print("Can not find gs_usb device")
    
dev = devs[0]
bus = can.Bus(interface="gs_usb", channel=dev.gs_usb, index=0, bitrate=500000)
print ("reached")
'''

'''
import usb
import can

dev = usb.core.find(idVendor=0x1D50, idProduct=0x606F)
bus = can.Bus(
    interface="gs_usb",
    channel=dev.product,
    bus=dev.bus,
    address=dev.address,
    bitrate=250000
)


with can.Bus() as bus:
    msg = can.Message(
        arbitration_id=0xC0FFEE,
        data=[0, 25, 0, 1, 3, 1, 4, 1],
        is_extended_id=True
    )
try:
    bus.send(msg)
    print(f"Message sent on {bus.channel_info}")
except can.CanError:
    print("Message NOT sent")
'''

import time

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
    if not dev.set_bitrate(250000):
        print("Can not set bitrate for gs_usb")
        return

    # Start device
    dev.start()

    # Prepare frames
    data = b"\x12\x34\x56\x78\x9A\xBC\xDE\xF0"
    sff_frame = GsUsbFrame(can_id=0x7FF, data=data)
    sff_frame2 = GsUsbFrame(can_id=0x270, data=[0x00, 0x02, 0x4f, 0x55])
    sff_frame3 = GsUsbFrame(can_id=0x350, data=[0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa])
    sff_none_data_frame = GsUsbFrame(can_id=0x7FF)
    err_frame = GsUsbFrame(can_id=0x7FF | CAN_ERR_FLAG, data=data)
    eff_frame = GsUsbFrame(can_id=0x12345678 | CAN_EFF_FLAG, data=data)
    eff_none_data_frame = GsUsbFrame(can_id=0x12345678 | CAN_EFF_FLAG)
    rtr_frame = GsUsbFrame(can_id=0x7FF | CAN_RTR_FLAG)
    rtr_with_eid_frame = GsUsbFrame(can_id=0x12345678 | CAN_RTR_FLAG | CAN_EFF_FLAG)
    rtr_with_data_frame = GsUsbFrame(can_id=0x7FF | CAN_RTR_FLAG, data=data)
    frames = [
        sff_frame,
        sff_frame2,
        sff_frame3,
        sff_none_data_frame,
        err_frame,
        eff_frame,
        eff_none_data_frame,
        rtr_frame,
        rtr_with_eid_frame,
        rtr_with_data_frame,
    ]

    # Read all the time and send message in each second
    end_time, n = time.time() + 1, -1
    while True:
        iframe = GsUsbFrame()
        if dev.read(iframe, 1):
            print("RX  {}".format(iframe))

        if time.time() - end_time >= 0:
            end_time = time.time() + 1
            n += 1
            n %= len(frames)

            if dev.send(frames[n]):
                print("TX  {}".format(frames[n]))


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass