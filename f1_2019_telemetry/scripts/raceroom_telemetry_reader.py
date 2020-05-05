from ctypes import Structure, c_int, c_float, c_double, c_char, sizeof
import mmap
import json

class r3e_playerdata(Structure):
     _pack_ = 1
     _fields_ = [("game_simulation_ticks", c_int),
                 ("game_simulation_time", c_double),
                 ("position", c_double * 3),
                 ("velocity",c_double * 3),
                 ("local_velocity", c_double * 3),
                 ("acceleration", c_double * 3),
                 ("local_acceleration", c_double * 3),
                 ("orientation", c_double * 3),
                 ("rotation", c_double * 3),
                 ("angular_acceleration", c_double * 3),
                 ("angular_velocity", c_double * 3),
                 ("local_angular_velocity", c_double * 3),
                 ("local_g_force", c_double * 3),
                 ("steering_force", c_double),
                 ("steering_force_percentage", c_double),
                 ("engine_torque", c_double),
                 ("current_downforce", c_double),
                 ("voltage", c_double),
                 ("ers_level", c_double),
                 ("power_mgu_h", c_double * 3),
                 ("power_mgu_k", c_double),
                 ("torque_mgu_k", c_double),
                 # Car setup (radians, meters, meters per second)
                 ("suspension_deflection", c_double * 4),
                 ("suspension_velocity", c_double * 4),
                 ("camber", c_double * 4),
                 ("ride_height", c_double * 4),
                 ("front_wing_height", c_double),
                 ("front_roll_angle", c_double),
                 ("rear_roll_angle", c_double),
                 ("unused1", c_double),
                 ("unused2", c_double),
                 ("unused3", c_double * 3),
                 ]

class r3e_shared(Structure):
     _pack_ = 1
     _fields_ = [("version_major", c_int),
                 ("version_minor", c_int),
                 ("all_drivers_offset", c_int),
                 ("drivers_data_size", c_int),
                 ("game_paused", c_int),
                 ("game_in_menus", c_int),
                 ("game_in_replay", c_int),
                 ("game_using_vr", c_int),
                 ("game_unused1", c_int),
                 ("player", r3e_playerdata),
                 ("track_name", c_char*64),
                 ("layout_name", c_char*64),
                 ]


class RaceRoomData(object):
    def __init__(self):
        pass

    def getJsonData(self):
        data = r3e_shared.from_buffer(self.buff)
        for fname, ftype in data._fields_:
            print fname, ftype
            #setattr(data, fname, 0)

        print data
        print data.__dict__()
        return json.dumps(data)

    def start(self):
        R3E_SHARED_MEMORY_NAME = "$R3"
        self.buff = mmap.mmap(-1, sizeof(r3e_shared), R3E_SHARED_MEMORY_NAME)
