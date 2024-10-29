import setup as gs

usb_1 = gs.GS_Unit_Test()


while (1):
    frame, time = usb_1.send_message_manual(0x7FF, b"\x12\x34\x56\x78\x9A\xBC\xDE\xF0")
    print("{}".format(frame))
