import threading
import socket
import struct
import time
import json

def singleton(class_):
    instances = {}
    def getinstance(*args, **kwargs):
        if class_ not in instances:
            instances[class_] = class_(*args, **kwargs)
        return instances[class_]
    return getinstance

# UDP RECEIVER ##########################################
@singleton
class DataReceiver:
    def __init__(self, parser):
        self._running = False
        self._thread = None
        self._parser = parser
        self._data = parser.getEmptyData()
        self._port = 20789
        self._connected = False
        self._callback = None

    def start(self):
        if self._thread:
            # jesli juz wystartowany to nic nie robimy
            return
        self._thread = threading.Thread(target=self._runServer)
        self._thread.daemon = True
        self._running = True
        self._thread.start()

    def isRunning(self):
        return self._running

    # data is updated inplace, no need to get new data on each iteration
    def getData(self):
        return self._data

    def getJsonData(self):
        flatData = dict(self._data['Motion'].__dict__)
        flatData.update(self._data['Telemetry'].__dict__)

        data = {key: value for key,value in flatData.items() if isinstance(value, int) or isinstance(value, float)}

        for key, tupleValues in flatData.items():
            if isinstance(tupleValues, tuple):
                data[key] = list(tupleValues)

        data['aaa'] = {'a':100, 'b':'asd'}
        ret = json.dumps(data)
        return ret

    def isConnected(self):
        return self._connected

    def stop(self):
        self._running = False
        self._thread.join()

    def register(self, callback):
        self._callback = callback

    def _runServer(self):
        try:
            #ip = '127.0.0.1'
            ip = '0.0.0.0' # broadcast
            msg = "Listening on " + ip + ":" + str(self._port)
            print msg
            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            sock.settimeout(0.2)
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            sock.bind((ip, self._port))
            while self._running:
                try:
                    packet, addr = sock.recvfrom(3096)
                    if packet == '':
                        raise RuntimeError("connection broken - header")
                    self._connected = True
                    parsed = self._parser.parseMessage(packet)
                    self._data.update(parsed)
                    if self._callback:
                        self._callback(self._data)
                except socket.timeout:
                    self._connected = False
                    continue
        except Exception:
            self._running = False
            raise
        self._running = False

# MOTION DATA #####################################################
class CarMotionData(object):
    def __init__(self, args=(0,)*18):  
        super(CarMotionData, self).__init__()
        self.worldPositionX = args[0];      # World space X position
        self.worldPositionY = args[1];      # World space Y position
        self.worldPositionZ = args[2];      # World space Z position
        self.worldVelocityX = args[3];      # Velocity in world space X
        self.worldVelocityY = args[4];      # Velocity in world space Y
        self.worldVelocityZ = args[5];      # Velocity in world space Z
        self.worldForwardDirX = args[6];    # World space forward X direction (normalised)
        self.worldForwardDirY = args[7];    # World space forward Y direction (normalised)
        self.worldForwardDirZ = args[8];    # World space forward Z direction (normalised)
        self.worldRightDirX = args[9];      # World space right X direction (normalised)
        self.worldRightDirY = args[10];     # World space right Y direction (normalised)
        self.worldRightDirZ = args[11];     # World space right Z direction (normalised)
        self.gForceLateral = args[12];      # Lateral G-Force component
        self.gForceLongitudinal = args[13]; # Longitudinal G-Force component
        self.gForceVertical = args[14];     # Vertical G-Force component
        self.yaw = args[15];                # Yaw angle in radians
        self.pitch = args[16];              # Pitch angle in radians
        self.roll = args[17];               # Roll angle in radians

class ExtraMotionData(object):
    def __init__(self, args=(0,)*30):
        super(ExtraMotionData, self).__init__()
        # Note: All wheel arrays have the following order: RL, RR, FL, FR
        self.suspensionPosition = args[0], args[1], args[2], args[3];       
        self.suspensionVelocity = args[4], args[5], args[6], args[7];       
        self.suspensionAcceleration = args[8], args[9], args[10], args[11];
        self.wheelSpeed = args[12], args[13], args[14], args[15];           # Speed of each wheel
        self.wheelSlip = args[16], args[17], args[18], args[19];            # Slip ratio for each wheel
        self.localVelocityX = args[20];       # Velocity in local space
        self.localVelocityY = args[21];       # Velocity in local space
        self.localVelocityZ = args[22];       # Velocity in local space
        self.angularVelocityX = args[23];	   # Angular velocity x-component
        self.angularVelocityY = args[24];     # Angular velocity y-component
        self.angularVelocityZ = args[25];     # Angular velocity z-component
        self.angularAccelerationX = args[26]; # Angular velocity x-component
        self.angularAccelerationY = args[27]; # Angular velocity y-component
        self.angularAccelerationZ = args[28]; # Angular velocity z-component
        self.frontWheelsAngle = args[29];     # Current front wheels angle in radians
 
class Motion(CarMotionData, ExtraMotionData):
    def __init__(self, args, player_id):
        start, end = player_id*18, (player_id*18)+18
        CarMotionData.__init__(self, args[start:end])
        ExtraMotionData.__init__(self, args[20*18:]) # skip data for first 20 cars
        #self.cars = [CarMotionData(args[x*18:(x*18)+18]) for x in range(20)] # 20 cars

# TELEMETRY DATA #####################################################
class CarTelemetry(object):
    def __init__(self, args=(0,)*30):
        super(CarTelemetry, self).__init__()
        self.speed = args[0]              # Speed of car in kilometres per hour
        self.throttle = args[1]           # Amount of throttle applied (0.0 to 1.0)
        self.steer = args[2]              # Steering (-1.0 (full lock left) to 1.0 (full lock right))
        self.brake = args[3]              # Amount of brake applied (0.0 to 1.0)
        self.clutch = args[4]             # Amount of clutch applied (0 to 100)
        self.gear = args[5]               # Gear selected (1-8, N=0, R=-1)
        self.engineRPM = args[6]          # Engine RPM
        self.drs = args[7]                # 0 = off, 1 = on
        self.revLightsPercent = args[8]   # Rev lights indicator (percentage)
        self.brakesTemperature = args[9], args[10], args[11], args[12]        # Brakes temperature (celsius)
        self.tyresSurfaceTemperature = args[13],args[14],args[15],args[16] # Tyres surface temperature (celsius)
        self.tyresInnerTemperature = args[17],args[18],args[19],args[20]   # Tyres inner temperature (celsius)
        self.engineTemperature = args[21]                                     # Engine temperature (celsius)
        self.tyresPressure = args[22],args[23],args[24],args[25]            # Tyres pressure (PSI)
        self.surfaceType = args[26],args[27],args[28],args[29]                # Driving surface, see appendices

class Telemetry(CarTelemetry):
    def __init__(self, args, player_id):  
        start, end = player_id*30, (player_id*31)+31
        CarTelemetry.__init__(self, args[:30])
        self.buttonStatus = args[-1] # pressed buttons
        #self.cars = [CarTelemetry(args[x*30:(x*30)+30]) for x in range(20)] # 20 cars


# PARSER ##########################################################
class F12019Parser(object):
    HEADER_LENGTH = 23
    HEADER_PATTERN = '<HBBBBQfIB'

    CAR_MOTION_PATTERN = 'ffffffhhhhhhffffff'
    CAR_TELEMETRY_PATTERN = 'HfffBbHBB' + ('H'*12) + 'HffffBBBB'
    PACKET_ID_TO_SIZE = {0: 1343, 1:149, 2:843, 3:32, 4:1104, 5:843, 6:1347, 7:1143}

    ID_TO_PATTERN = {0: '<' + (20*CAR_MOTION_PATTERN) + (30*'f'),\
                     6: '<' + (20*CAR_TELEMETRY_PATTERN) + 'I'}
    ID_TO_CLASS = {0: Motion, 6: Telemetry}
    ID_TO_NAME = {0:"Motion", 1:"Session", 2:"Lap Data", 3:"Event",\
                  4:"Participants", 5:"Car Setups", 6:"Telemetry", 7:"Car Status"}

    def parseMessage(self, packet):
        header = packet[:F12019Parser.HEADER_LENGTH]
        packet_id, player_id = self._getMessageType(header)
        assert len(packet) == F12019Parser.PACKET_ID_TO_SIZE[packet_id], "Packet size does not match the message"
        if packet_id not in F12019Parser.ID_TO_PATTERN:
            return {}        
        unpack_pattern = F12019Parser.ID_TO_PATTERN[packet_id]
        msg = packet[F12019Parser.HEADER_LENGTH:]        
        parsed = struct.unpack(unpack_pattern, msg)    
        message_name = F12019Parser.ID_TO_NAME[packet_id];
        cls = F12019Parser.ID_TO_CLASS[packet_id];
        return {message_name: cls(parsed, player_id)}       
    
    def getEmptyData(self):
        data = {}
        for packet_id, cls in F12019Parser.ID_TO_CLASS.items():
            name = F12019Parser.ID_TO_NAME[packet_id]
            num_fields = len(F12019Parser.ID_TO_PATTERN[packet_id])
            data[name] = cls((0,)*num_fields, 0)            
        return data

    def _getMessageType(self, header_data):
        version, _, _, _, packet_id, _, _, _, player_id = struct.unpack(F12019Parser.HEADER_PATTERN, header_data)
        assert version == 2019, 'VERSION IS NOT 2019: ' + str(version)
        return packet_id, player_id        

# EXAMPLE ######################################################################
# te instrukcje beda wykonane tylko jezeli odpalasz ten skrypt bezposrednio: C:\Python27\python.exe C:\Users\tomas\Desktop\f1.py
if __name__ == '__main__':
    # PRZYKLADOWE UZYCIE    

    receiver = DataReceiver(F12019Parser())
    #overlay = F1Overlay()
    #receiver.register(lambda data: overlay.update(*data['Motion'].wheelSlip))
    receiver.start() # wystartuje osobny watek, ktory odbiera pakiety w tle
    data = receiver.getData() # przeczytaj telemetrie raz - sama sie odswieza
    while receiver.isRunning(): #  nieskonczona petla
        if receiver.isConnected():
            #print "speed", data['Telemetry'].speed, " throttle", data['Telemetry'].throttle, \
            #    " brake", data['Telemetry'].brake, " gear", data['Telemetry'].gear
            pass
         # nie uzywaj sleepa w programie sterujacym!
        time.sleep(1)
