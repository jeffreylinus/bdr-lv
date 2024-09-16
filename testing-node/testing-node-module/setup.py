from gs_usb.gs_usb import GsUsb
from gs_usb.gs_usb_frame import GsUsbFrame
from gs_usb.constants import (
    CAN_EFF_FLAG,
    CAN_ERR_FLAG,
    CAN_RTR_FLAG,
)
import time
from datetime import datetime



class GS_Unit_Test:
    rx = []
    tx = []

    def __init__(self, **kwargs):
        # set params
        bitrate = kwargs.pop('bitrate', 500_000)
        dev = kwargs.pop('dev', 0)

        # find device
        devs = GsUsb.scan()
        if len(devs) == 0:
            raise Exception("Can not find gs_usb device")
        dev = devs[dev]

        # Configuration
        if not dev.set_bitrate(bitrate):
            raise Exception("Can not set bitrate for gs_usb")

        # Start device
        dev.start()
        self.start_time = time.time()

        # Read dev into class
        self.GS_USB = dev
    
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
        Defines a Function that listens for a message for time timout, if message was found
        
        
        '''
        # initialize empty frame
        frame = GsUsbFrame()

        # read, if true return
        if self.GS_USB.read(frame, timeout):
            return frame, self.timestamp
        
        # exception: frame not read
        print('frame was not read!')

    
    def read_mode(self, time_set = 60, timout = 0.05):
        '''
        Defines a function that reads for a set amoiunt of time
        
        ''' 
        time_set = time.time() + time_set
        while (time_set > time.time()):
            frame = GsUsbFrame()
            if self.GS_USB.read(frame):
                self.rx.append((frame, self.timestamp()))

    def export_data(self, path, name = datetime.now(), TX_RX = "BOTH"):
        #TODO
        '''
        Exports data as a CSV from self.tx and self.rx. tx and rx are arrays of touples (canframe, timestamp)
        Export as canframe, timestamp... in a csv

        path: filepath
        name: filename
        TX_RX: export tx or rx or both
        '''
        export_buffer = []

        match TX_RX:
            case "BOTH":
                counter_tx = 0
                counter_rx = 0

                while counter_tx < len(self.tx) and counter_rx < len(self.rx):
                    if self.tx[counter_tx][1] > self.rx[counter_rx][1]:
                        export_buffer.append(self.tx[counter_tx])
                        counter_tx += 1
                    else:
                        export_buffer.append(self.rx[counter_rx])
                        counter_rx += 1

                while counter_rx < len(self.rx):
                    export_buffer.append(self.rx[counter_rx])
                    counter_rx += 1

                while counter_tx < len(self.tx):
                    export_buffer.append(self.tx[counter_tx])
                    counter_tx += 1

            case "TX":
               export_buffer = self.tx 

            case "RX":
                export_buffer = self.rx
            

    def send_from_csv(self, path):
        #TODO
        '''
        I don't think we need this
        Sends can frames from a csv file (csv will be formatted as CAN_ID, CAN_DATA...)

        path: filepath
        '''
        return