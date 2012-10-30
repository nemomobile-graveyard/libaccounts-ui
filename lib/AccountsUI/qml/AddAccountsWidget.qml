// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0

Page {
    id: addAccountsPage
    height: 1000
    function setProviderName(pName) {providerHeader.title = pName}
    function setProviderDesc(pDesc) {providerHeader.subTitle = pDesc}
    function setProviderIcon(pIcon) {providerHeader.iconName = pIcon}

    PageHeader {
        id: header
        text: qsTr("Add new account")
    }

    BasicHeaderWidget {
        id: providerHeader
        anchors.top: header.bottom
    }
}
