import mmap
import struct
import math
import time
import json

def convertDegreeArcToPercent(value):
    return max(value/360, 0)


class AssettoCorsaData(object):
        def __init__(self):
            print('AssettoCorsaData() init()')
            self.fields = 'packetId throttle brake fuel gear rpm steerAngle speed velocity1 velocity2 velocity3 accGX accGY accGZ wheelSlipFL wheelSlipFR wheelSlipRL wheelSlipRR wheelLoadFL wheelLoadFR wheelLoadRL wheelLoadRR wheelsPressureFL wheelsPressureFR wheelsPressureRL wheelsPressureRR wheelAngularSpeedFL wheelAngularSpeedFR wheelAngularSpeedRL wheelAngularSpeedRR TyrewearFL TyrewearFR TyrewearRL TyrewearRR tyreDirtyLevelFL tyreDirtyLevelFR tyreDirtyLevelRL tyreDirtyLevelRR TyreCoreTempFL TyreCoreTempFR TyreCoreTempRL TyreCoreTempRR camberRADFL camberRADFR camberRADRL camberRADRR suspensionTravelFL suspensionTravelFR suspensionTravelRL suspensionTravelRR drs tc1 heading pitch roll cgHeight carDamagefront carDamagerear carDamageleft carDamageright carDamagecentre numberOfTyresOut pitLimiterOn abs1 kersCharge kersInput automat rideHeightfront rideHeightrear turboBoost ballast airDensity airTemp roadTemp localAngularVelX localAngularVelY localAngularVelZ finalFF performanceMeter engineBrake ersRecoveryLevel ersPowerLevel ersHeatCharging ersIsCharging kersCurrentKJ drsAvailable drsEnabled brakeTempFL brakeTempFR brakeTempRL brakeTempRR clutch tyreTempI1 tyreTempI2 tyreTempI3 tyreTempI4 tyreTempM1 tyreTempM2 tyreTempM3 tyreTempM4 tyreTempO1 tyreTempO2 tyreTempO3 tyreTempO4 isAIControlled tyreContactPointFLX tyreContactPointFLY tyreContactPointFLZ tyreContactPointFRX tyreContactPointFRY tyreContactPointFRZ tyreContactPointRLX tyreContactPointRLY tyreContactPointRLZ tyreContactPointRRX tyreContactPointRRY tyreContactPointRRZ tyreContactNormalFLX tyreContactNormalFLY tyreContactNormalFLZ tyreContactNormalFRX tyreContactNormalFRY tyreContactNormalFRZ tyreContactNormalRLX tyreContactNormalRLY tyreContactNormalRLZ tyreContactNormalRRX tyreContactNormalRRY tyreContactNormalRRZ tyreContactHeadingFLX tyreContactHeadingFLY tyreContactHeadingFLZ tyreContactHeadingFRX tyreContactHeadingFRY tyreContactHeadingFRZ tyreContactHeadingRLX tyreContactHeadingRLY tyreContactHeadingRLZ tyreContactHeadingRRX tyreContactHeadingRRY tyreContactHeadingRRZ brakeBias localVelocityX localVelocityY localVelocityZ P2PActivation P2PStatus currentMaxRpm mz1 mz2 mz3 mz4 fx1 fx2 fx3 fx4 fy1 fy2 fy3 fy4 slipRatio1 slipRatio2 slipRatio3 slipRatio4 slipAngle1 slipAngle2 slipAngle3 slipAngle4 tcinAction absInAction suspensionDamage1 suspensionDamage2 suspensionDamage3 suspensionDamage4 tyreTemp1 tyreTemp2 tyreTemp3 tyreTemp4 waterTemp brakePressureFL brakePressureFR brakePressureRL brakePressureRR frontBrakeCompound rearBrakeCompound padLifeFL padLifeFR padLifeRL padLifeRR discLifeFL discLifeFR discLifeRL discLifeRR'.replace('  ', ' ').split(' ')
            self.layout = 'ifffiiffffffff 4f fffffffffffffffffffffffffffffffffffffffffffiifffiffffffffffffiiiiifiifffffffffffffffffiffffffffffffffffffffffffffffffffffffffffiifffffffffffffffffffffiifffffffffffffiiffffffff'
            self.physics_shm_size = struct.calcsize(self.layout)
            self.mmapPhysic = None
            self.mmapStatic = None

        def start(self):
            print('AssettoCorsaData() start()')
            if not self.mmapPhysic:
                self.mmapPhysic = mmap.mmap(-1, self.physics_shm_size, "Local\\acpmf_physics",  access=mmap.ACCESS_READ)
            #self.mmapStatic = mmap.mmap(-1, XYZ, u"Local\\acpmf_static")

        def getData(self):
            self.mmapPhysic.seek(0)
            rawData = self.mmapPhysic.read(self.physics_shm_size)
            data = {}
            for index, value in enumerate(struct.unpack(self.layout, rawData)):
                data[self.fields[index]] = value

            self._convertData(data)
            return data

        def getJsonData(self):
            return json.dumps(self.getData())

        def stop(self):
            print('AssettoCorsaData() stop()')
            if self.mmapPhysic:
                self.mmapPhysic.close()
            if self.mmapStatic:
                self.mmapStatic.close()

            self.mmapPhysic = None
            self.mmapStatic = None

        def _convertData(self, data):
            # TODO make these conversions immediately when reading from shm
            for newName in ['wheelSlip', 'wheelLoad', 'wheelsPressure',
                    'brakeTemp', 'brakePressure', 'Tyrewear', 'wheelAngularSpeed',
                    'padLife', 'discLife', 'camberRAD', 'TyreCoreTemp', 'tyreDirtyLevel',
                    'suspensionTravel']:
                data[newName] = []
                for oldName in [newName + 'FL', newName+'FR', newName+'RL', newName+'RR']:
                    data[newName].append(convertDegreeArcToPercent(data[oldName]))
                    del data[oldName]


if __name__ == '__main__':
    assettoReader = AssettoCorsaData()
    assettoReader.start()
    while True:
        print 'Assetto data:', assettoReader.getData()
        time.sleep(1)
