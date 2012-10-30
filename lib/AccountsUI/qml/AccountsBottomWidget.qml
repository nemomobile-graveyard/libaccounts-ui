// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id: subscribeWidget
    signal registerNewAccount()
    function setSubscribeNewAccountText(text) {subscribeNew.text = text}
    Text {
        id: questionLabel
        width: 800;
        font.pixelSize: 20
        text: "Don't have an account yet?"
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
    }

    Text {
        id: subscribeNew
        width: 800;
        font.pixelSize: 20
        textFormat: Text.RichText
        text:"aaaaaaaaaaaaaaaaaaa"
        color: "blue"
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        anchors.top: questionLabel.bottom
        MouseArea {
            anchors.fill: parent
            onClicked: registerNewAccount()
        }
    }
}
