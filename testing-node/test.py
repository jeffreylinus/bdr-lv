import can


bus = can.interface.Bus(channel='/dev/ttyUSBx', bustype='slcan', bitrate=500000)


from can.interface import Bus


