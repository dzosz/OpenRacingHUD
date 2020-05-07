import "../lib"

DataReader {
    import_filename : "assetto_corsa_telemetry_reader.py";
    init_script : "assettoReader = assetto_corsa_telemetry_reader.AssettoCorsaData()";
    start_script : "assettoReader.start()";
    stop_script : "assettoReader.stop()";
    get_data_script : "assettoReader.getJsonData()"
}
