import setup as gs

usb_1 = gs.GS_Unit_Test()

usb_1.read_message_manual(1_000)

print(usb_1.rx)



