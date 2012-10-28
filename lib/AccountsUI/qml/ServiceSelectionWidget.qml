// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0

Page {
    id: serviceSelection

    function setProviderName(pName) {chooseServiceHeader.title = pName}
    function setDescription(desc) {chooseServiceHeader.subTitle = desc}
    function setProviderIcon(pIcon) {chooseServiceHeader.iconName = pIcon}
    height: 1000
    width: 1000
    BasicHeaderWidget {
        id: chooseServiceHeader
    }

    ServiceSettingsWidget {
        id: ssw
        objectName: "selectServiceView"
        y: 200
        clip: true
        model: selectServiceModel
    }

}
