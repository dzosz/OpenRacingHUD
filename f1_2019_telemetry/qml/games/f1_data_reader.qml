import "../lib"

DataReader {
    import_filename : "f1_2019_telemetry_reader.py";
    init_script : "receiver = f1_2019_telemetry_reader.DataReceiver(f1_2019_telemetry_reader.F12019Parser())";
    start_script : "receiver.start()";
    stop_script : "receiver.stop()";
    get_data_script : "receiver.getJsonData()"
}
