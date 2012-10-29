// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0

ListView {
    id: servicesListView
    width: parent.width; height: 400
    spacing : 20
    signal serviceEnabled(string service, bool status)
    function setServiceStatus(status){
        serviceEnabled(servicesListView.currentItem.myModel.serviceName, status)
    }

    delegate: Item {
        id: item
        width: parent.width;
        height: 40
        property variant myModel: model
        Row {
            id: row
            Column {
                CheckBox {
                    id: enableButton
                    checked: serviceStatus
                    onClicked: {
                        servicesListView.currentIndex = index;
                        setServiceStatus(checked);
                    }
                }
            }

            Column {
                Label {
                    text: serviceName
                    verticalAlignment: Text.AlignVCenter
                }

                Label {
                    font.pixelSize: 15
                    wrapMode: Text.WordWrap
                    text: serviceDesc
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
}
