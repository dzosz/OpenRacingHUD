from ctypes import Structure, c_int, c_float, c_double, c_char, sizeof, Array
import mmap
import json
import time


'''
class r3e_vec3_f64(Structure):
    _pack_ = 1
    _fields_ = [("x", c_double),
                ("y", c_double),
                ("z", c_double)
                ]
'''

class r3e_playerdata(Structure):
    _pack_ = 1
    _fields_ = [("game_simulation_ticks", c_int),
                ("game_simulation_time", c_double),
                ("position", c_double * 3),
                ("velocity", c_double * 3),
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
                ("power_mgu_h", c_double),
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
                ("unused3", c_double * 3)
                ]

class r3e_flags(Structure):
    _pack_ = 1
    _fields_ = [("yellow ", c_int),
                ("yellowCausedIt", c_int),
                ("yellowOvertake", c_int),
                ("yellowPositionsGained", c_int),
                ("sector_yellow", c_int * 3),
                ("closest_yellow_distance_into_track", c_float),
                ("blue", c_int),
                ("black", c_int),
                ("green", c_int),
                ("checkered", c_int),
                ("white", c_int),
                ("black_and_white", c_int)
                ]
                
class r3e_cut_track_penalties(Structure):
    _pack_ = 1
    _fields_ = [("drive_through", c_int),
                ("stop_and_go", c_int),
                ("pit_stop", c_int),
                ("time_deduction", c_int),
                ("slow_down", c_int),
                ]
                
class r3e_aid_settings(Structure):
    _pack_ = 1
    _fields_ = [("abs", c_int),
                ("tc", c_int),
                ("esp", c_int),
                ("countersteer", c_int),
                ("cornering", c_int),
                ]
      
     
class r3e_push_to_pass(Structure):
    _pack_ = 1
    _fields_ = [("available", c_int),
                ("engaged", c_int),
                ("amount_left", c_int),
                ("engaged_time_left", c_int),
                ("wait_time_left", c_int),
                ]
 
class r3e_drs(Structure):
    _pack_ = 1
    _fields_ = [("equipped", c_int),
                ("available", c_int),
                ("numActivationsLeft", c_int),
                ("engaged", c_int),
                ]


class r3e_brake_temp(Structure):
    _pack_ = 1
    _fields_ = [("current_temp", c_float),
                ("optimal_temp", c_float),
                ("cold_temp", c_float),
                ("hot_temp", c_float),
                ]

class r3e_car_damage(Structure):
    _pack_ = 1
    _fields_ = [("engine", c_float),
                ("transmission", c_float),
                ("aerodynamics", c_float),
                ("suspension", c_float),
                ("unused1", c_float),
                ("unused2", c_float),
                ]

class r3e_tire_temp(Structure):
    _pack_ = 1
    _fields_ = [("current_temp", c_float*3),
                ("optimal_temp", c_float),
                ("cold_temp", c_float),
                ("hot_temp", c_float),
                ]

class r3e_driver_info(Structure):
    _pack_ = 1
    _fields_ = [("car_name", c_char*64),
                ("car_number", c_int),
                ("class_id", c_int),
                ("model_id", c_int),
                ("team_id", c_int),
                ("livery_id", c_int),
                ("manufacturer_id", c_int),
                ("user_id", c_int),
                ("slot_id", c_int),
                ("class_performance_index", c_int),
                ("engine_type", c_int),
                ("unused1", c_int),
                ("unused2", c_int),
                
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
                ("track_id", c_int),
                ("layout_id", c_int),
                ("layout_length", c_float),
                ("sector_start_factors", c_float * 3),
                ("race_session_laps", c_int * 3),
                ("race_session_minutes", c_int * 3),
                ("event_index", c_int),
                ("session_type", c_int),
                ("session_iteration", c_int),
                ("session_length_format", c_int),
                ("session_pit_speed_limit", c_float),
                ("session_phase", c_int),
                ("start_lights", c_int),
                ("tire_wear_active", c_int),
                ("fuel_use_active", c_int),
                ("number_of_laps", c_int),
                ("session_time_duration", c_float),
                ("session_time_remaining", c_float),
                ("event_unused1", c_int),
                ("event_unused2", c_float),
                # pit
                ("pit_window_status", c_int),
                ("pit_window_start", c_int),
                ("pit_window_end", c_int),
                ("in_pitlane", c_int),
                ("pit_menu_selection", c_int),
                ("pit_menu_state", c_int * 11),
                ("pit_state", c_int),
                ("pit_total_duration", c_float),
                ("pit_elapsed_time", c_float),
                ("pit_action", c_int),
                ("num_pitstops", c_int),
                ("pit_unused1", c_int),
                ("pit_unused2", c_float),
                ("flags", r3e_flags),
                # scoring and timings
                ("position", c_int ),
                ("position_class", c_int),
                ("finish_status", c_int),
                ("cut_track_warnings", c_int),
                ("penalties", r3e_cut_track_penalties),
                ("num_penalties", c_int),
                ("completed_laps", c_int),
                ("current_lap_valid", c_int),
                ("track_sector", c_int),
                ("lap_distance", c_float),
                ("lap_distance_fraction", c_float),
                ("lap_time_best_leader", c_float),
                ("lap_time_best_leader_class", c_float ),
                ("session_best_lap_sector_times", c_float*3),
                ("lap_time_best_self", c_float),
                ("sector_time_best_self", c_float * 3),
                ("lap_time_previous_self", c_float),
                ("sector_time_previous_self", c_float*3),
                ("lap_time_current_self", c_float),
                ("sector_time_current_self", c_float*3),
                ("lap_time_delta_leader", c_float),
                ("lap_time_delta_leader_class", c_float),
                ("time_delta_front", c_float),
                ("time_delta_behind", c_float),
                ("time_delta_best_self", c_float),
                ("best_individual_sector_time_self", c_float*3),
                ("best_individual_sector_time_leader", c_float*3),
                ("best_individual_sector_time_leader_class", c_float*3),
                ("score_unused1", c_int),
                ("score_unused2", c_int),
                ("score_unused3", c_float),
                ("score_unused4", c_float),
                # vehicle information
                ("vehicle_info", r3e_driver_info),
                ("player_name", c_char*64),
                # vehicle state
                ("control_type", c_int),
                ("speed", c_float),      
                ("engine_rps", c_float), 
                ("max_engine_rps", c_float), 
                ("upshift_rps", c_float), 
                ("gear", c_int), 
                ("num_gears", c_int), 
                ("car_cg_location", c_float * 3), 
                ("car_orientation", c_float * 3), 
                ("local_acceleration", c_float * 3),
                ("total_mass", c_float),
                ("fuel_left", c_float), 
                ("fuel_capacity", c_float),                 
                ("fuel_per_lap", c_float), 
                ("engine_water_temp", c_float), 
                ("engine_oil_temp", c_float), 
                ("fuel_pressure", c_float), 
                ("engine_oil_pressure", c_float), 
                ("turbo_pressure", c_float), 
                ("throttle", c_float), 
                ("throttle_raw", c_float), 
                ("brake", c_float), 
                ("brake_raw", c_float), 
                ("clutch", c_float), 
                ("cluch_raw", c_float), 
                ("steer_input_raw", c_float), 
                ("steer_lock_degrees", c_int), 
                ("steer_wheel_range_degrees", c_int), 
                ("aid_settings", r3e_aid_settings), 
                ("drs", r3e_drs), 
                ("pit_limiter", c_int), 
                ("push_to_pass", r3e_push_to_pass),
                ("brake_bias", c_float), 
                ("vehicle_unused1", c_int), 
                ("vehicle_unused2", c_int), 
                ("vehicle_unused3", c_float), 
                ("vehicle_unused4", c_float), 
                ("vehicle_unused5", c_float*3), 
                # tires
                ("tire_type", c_int), 
                ("tire_rps", c_float * 4), 
                ("tire_speed", c_float * 4), 
                ("tire_grip", c_float * 4), 
                ("tire_wear", c_float * 4), 
                ("tire_flatspot", c_int * 4), 
                ("tire_pressure", c_float * 4), 
                ("tire_dirt", c_float * 4), 
                ("tire_temp", r3e_tire_temp * 4), 
                ("tire_type_front", c_int), 
                ("tire_type_rear", c_int), 
                ("tire_subtype_front", c_int), 
                ("tire_subtype_rear", c_int), 
                ("brake_temp", r3e_brake_temp * 4), 
                ("brake_pressure", c_float * 4), 
                ("tire_unused1", c_int), 
                ("tire_unused2", c_int), 
                ("tire_unused3", c_float), 
                ("tire_unused4", c_float), 
                ("tire_unused5", c_float * 4), 
                ("tire_load", c_float * 4), 
                ("car_damage", r3e_car_damage), 
                ("num_cars", c_int),  
                #("all_drivers_data_1 ", r3e_driver_data * 128),                      
                ]


class RaceRoomData(object):
    def __init__(self):
        self.buff = None
        self.obj = r3e_shared()

    def getJsonData(self):
        return json.dumps(self.getData())

    def getData(self):
        obj = r3e_shared.from_buffer_copy(self.buff)
        data = {}
        self._getDictFromStructure(data, "", obj)
        self._convertData(data)
        return data

    def start(self):
        if not self.buff:
            R3E_SHARED_MEMORY_NAME = "$R3E"  
            print 'RaceRoomData::start() reading shared memory:', R3E_SHARED_MEMORY_NAME
            self.buff = mmap.mmap(-1, sizeof(r3e_shared), R3E_SHARED_MEMORY_NAME, access=mmap.ACCESS_READ)
        
    def stop(self):
        if self.buff:
            self.buff.close()
        self.buff = None
        
    def _getDictFromStructure(self, data, name, obj):   
        if isinstance(obj, Structure):
            for fname, ftype in obj._fields_:          
                self._getDictFromStructure(data, fname, getattr(obj, fname))        
        elif isinstance(obj, Array):
            data[name] = [self._getDictFromStructure({}, "", x) for x in obj]                     
        elif name:
            data[name] = obj
        else:
            return obj
        return data
        
    def _convertData(self, data):
        data['wheelSlip'] = [int((1-x)*100) for x in data['tire_grip']]
            
# EXAMPLE ######################################################################
if __name__ == '__main__':
    r3reader = RaceRoomData()
    r3reader.start()
    while True:
        print 'R3E data:', r3reader.getData()
        time.sleep(1)
