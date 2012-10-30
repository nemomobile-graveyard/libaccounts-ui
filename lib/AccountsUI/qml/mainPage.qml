// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0

PageStackWindow {
    id: appWindow
    function setPage(msg) { appWindow.pageStack.push(settingsPage); }
    function openServiceSelectionPage(msg) {
        appWindow.pageStack.push(serviceSelectionWidget);
    }

    showToolBar: true
    showStatusBar: false
    initialPage: AddAccountsWidget{objectName: "addAccountsWidget"}

    AccountSettingsWidget{
        id: settingsPage
        objectName: "settingsWidget"
        visible: false
    }


    ServiceSelectionWidget{
        id: serviceSelectionWidget
        objectName: "serviceSelectionWidget"
        visible: false
    }


    ToolBar {
        anchors.bottom: parent.bottom
    }
}
