import "../lib"

DataReader {
    import_filename : "f1_2019_telemetry_reader.py";
    init_script : "f1rcv = f1_2019_telemetry_reader.DataReceiver(f1_2019_telemetry_reader.F12019Parser())";
    start_script : "f1rcv.start()";
    stop_script : "f1rcv.stop()";
    get_data_script : "f1rcv.getJsonData()"
}
