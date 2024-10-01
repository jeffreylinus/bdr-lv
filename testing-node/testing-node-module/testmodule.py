import setup as gs

usb_1 = gs.GS_Unit_Test()


while (1):
    frame, time = usb_1.read_message_manual()
    print("{}".format(frame))
