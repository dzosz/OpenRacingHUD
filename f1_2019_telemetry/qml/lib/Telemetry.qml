import QtQuick 2.0

Item {
    // default values
    property var wheelSlip : [0, 0, 0, 0];
    property double speed : 0;
    property double brake : 0;
    property double throttle : 0;


            /*
    Component.onCompleted: {

            var component = Qt.createComponent("SelfDestroyingRect.qml");
            for (var i=0; i<5; i++) {
                var object = component.createObject(container);
                object.x = (object.width + 10) * i;
            }

        }
                       */
    function onReceive(message : string) {
        //console.log("Telemetry::onReceive():", message);
        var data = JSON.parse(message);
        wheelSlip = data.wheelSlip;
        speed = data.speed;
        brake = data.brake;
        throttle = data.throttle;
    }

}
