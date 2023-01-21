import QtQuick
import QtQuick.Controls 2.12
import Monty 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    SomeClass {
        id: myClass
    }

    Button {
        id: myButton
        anchors.centerIn: parent
        text: "Click Me"
        onClicked: myClass.callMe()
    }

}
