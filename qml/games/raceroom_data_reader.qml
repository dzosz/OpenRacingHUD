import QtQuick 2.12
import "../lib"

DataReader {
    import_filename : "raceroom_telemetry_reader.py";
    init_script : "r3ercv = raceroom_telemetry_reader.RaceRoomData()";
    start_script : "r3ercv.start()";
    stop_script : "";
    get_data_script : "r3ercv.getJsonData()"
}
