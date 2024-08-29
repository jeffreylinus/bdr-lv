from gs_usb.gs_usb import GsUsb
from gs_usb.gs_usb_frame import GsUsbFrame
from gs_usb.constants import (
    CAN_EFF_FLAG,
    CAN_ERR_FLAG,
    CAN_RTR_FLAG,
)
import time

class GS_Unit_Test:
    rx = []
    tx = []

    def __init__(self, **kwargs):
        # set params
        bitrate = kwargs.pop('bitrate', 500000)
        dev = kwargs.pop('dev', 0)

        # find device
        devs = GsUsb.scan()
        if len(devs) == 0:
            print("Can not find gs_usb device")
            return
        dev = devs[dev]

        # Configuration
        if not dev.set_bitrate(bitrate):
            print("Can not set bitrate for gs_usb")
            return

        # Start device
        dev.start()
        self.start_time = time.time()

        # Read dev into class
        self.GS_USB = dev

        return
    
    def timestamp(self):
        return time.time - self.start_time
    
    def send_message_manual(self, can_id, data):
        frame_str = f'{can_id}, {data}'

        # intitialize frame
        frame = GsUsbFrame(can_id, data)
        
        # send frame
        self.GS_USB.send(frame)
        
        # return frame, send time
        return frame_str, self.timestamp

        
    def read_message_manual(self, timeout = 1):
        '''
        Defines a Function that 
        
        
        '''
        # initialize empty frame
        frame = GsUsbFrame()

        # read, if true return
        if self.GS_USB.read(frame, timeout):
            return frame, self.timestamp
        
        # exception: frame not read
        print('frame was not read!')
        return
    
    def read_mode(self, time_set = 60):
        '''
        Defines a function that puts 
        
        ''' 
        time_set = time.time() + time_set
        while (time_set > time.time()):
            frame = GsUsbFrame()
            if self.GS_USB.read(frame, 0):
                self.rx.append((frame, self.timestamp()))

        return

    def export_data(self, path, name, TX_RX = "BOTH"):
        #TODO
        '''
        Exports data as a CSV from self.tx and self.rx. tx and rx are arrays of touples (canframe, timestamp)
        Export as canframe, timestamp... in a csv

        path: filepath
        name: filename
        TX_RX: export tx or rx or both
        '''

        match TX_RX:
            case "BOTH":
                counter_tx = 0
                counter_rx = 0
                while counter_tx < len(self.tx) or counter_rx < len(self.rx):
                    

            case "TX":

            case "RX":

        return

    def send_from_csv(self, path):
        #TODO
        '''
        Sends can frames from a csv file (csv will be formatted as CAN_ID, CAN_DATA...)

        path: filepath
        '''
        return