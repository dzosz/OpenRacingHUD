import QtQuick 2.12

Item {
    property string import_filename;
    property string init_script;
    property string start_script;
    property string stop_script;
    property string get_data_script;

    Component.onCompleted: {
        if (import_filename !== "")
            pythonExecutor.import(import_filename)
        if (init_script !== "")
            pythonExecutor.run(init_script)
    }

    function start()
    {
        if (start_script !== "")
            pythonExecutor.run(start_script)
    }

    function getJsonData() : string
    {
        return pythonExecutor.eval(get_data_script)
    }

    function stop()
    {
        if (stop_script !== "")
            pythonExecutor.run(stop_script)
    }
}
