
#include "generic-account-widget.h"
#include "credentialwidget.h"
#include "common.h"
#include "accountsmanagersingleton.h"
#include <AccountsUI/credentialwidgetmodel.h>
#include <AccountsUI/ProviderPluginProcess>
#include <AccountsUI/ProviderPluginProxy>
#include <QDeclarativeEngine>
#include <QDeclarativeComponent>
#include <QGraphicsObject>
#include <QDesktopServices>
#include <QGraphicsLinearLayout>
#include <QMessageBox>
#include <QAbstractButton>
#include <QPushButton>
#include <QGraphicsProxyWidget>

namespace AccountsUI {

class GenericAccountWidgetPrivate
{
public:
    GenericAccountWidgetPrivate(GenericAccountWidget *publicObject)
        :q_ptr(publicObject)
    {
        widgetModel = NULL;
        credentialWidget = NULL;
        doNotShowPassword = false;
        accountValidationInProgress = false;
        rememberMe = true;
    }
    ~GenericAccountWidgetPrivate()
    {}

    AbstractAccountSetupContext *context;
    QDomDocument providerAccountDocument;
    QObject *qmlObject;
    QGraphicsWidget *qmlWidget;
    QObject *qmlBottomObject;
    QGraphicsWidget *qmlBottomWidget;
    QString registerNewLink;
    QString forgotPasswordUrl;
    bool doNotShowPassword;
    QString authDomainSeparator;
    QString authDomainDefault;
    QString provider;
    QString providerName;
    CredentialWidgetModel *widgetModel;
    CredentialWidget *credentialWidget;
    QGraphicsLinearLayout *layout;
    bool accountValidationInProgress;
    QString username;
    QString password;
    bool rememberMe;
    QDeclarativeEngine *engine;
    QGraphicsWidget *hideWidget;

    void createUiFromXml(const QDomDocument &providerAccountDocument);

protected:
    Q_DECLARE_PUBLIC(GenericAccountWidget);
    GenericAccountWidget *q_ptr;

};

void GenericAccountWidgetPrivate::createUiFromXml(const QDomDocument &aProviderAccountDocument)
{
    Q_Q(GenericAccountWidget);

    // xml file that describes the ui elements for the provider
    providerAccountDocument = aProviderAccountDocument;
    QDomElement root = aProviderAccountDocument.documentElement();
    QDomElement descriptionElement = root.firstChildElement("description");
    QDomElement providerIcon = root.firstChildElement("icon");
    QDomElement catalog = root.firstChildElement("translations");
    QDomElement signUpLink = root.firstChildElement("sign-up-link");
    QDomElement forgotPasswordLink = root.firstChildElement("forgot-password-link");
    QDomElement doNotShowPasswordField = root.firstChildElement("do-not-show-password");
    provider = root.attribute("id");

    providerName = qtTrId(root.firstChildElement("name").text().toLatin1());

    QString descriptionText = descriptionElement.text();
    QString providerIconId = providerIcon.text();
    registerNewLink = signUpLink.text();
    forgotPasswordUrl = forgotPasswordLink.text();
    if (!doNotShowPasswordField.text().isEmpty() &&
        !doNotShowPasswordField.text().compare(QLatin1String("yes"), Qt::CaseInsensitive))
        doNotShowPassword = true;
    else
        doNotShowPassword = false;

    QDomElement node = root.firstChildElement("account-setup").firstChildElement();
    while (!node.isNull()) {
        if (node.tagName() == "authdomain") {
            /* we select the first occurrence*/
            authDomainSeparator = node.attribute("separator");
            authDomainDefault = node.attribute("default_domain");
            break;
        }
        node = node.nextSiblingElement();
    }
    QObject *accountsObject = qmlObject->findChild<QObject*>("addAccountsWidget");

    QMetaObject::invokeMethod(accountsObject, "setProviderName", Q_ARG(QVariant, providerName));
    QMetaObject::invokeMethod(accountsObject, "setProviderDesc", Q_ARG(QVariant, descriptionText));
    QMetaObject::invokeMethod(accountsObject, "setProviderIcon", Q_ARG(QVariant, providerIconId));

    // Credentials widget
    if (widgetModel) {
        delete widgetModel;
        widgetModel = NULL;
    }

    widgetModel = new CredentialWidgetModel();
    widgetModel->setDialogsVisabilityConfig(CredentialWidgetModel::LoginDialogVisible);
    widgetModel->setSignInButtonVisible(true);
    if (!forgotPasswordUrl.isEmpty()) {
        widgetModel->setForgotPasswordUrl(forgotPasswordUrl);
        widgetModel->setForgotPasswordVisible(true);

        QObject::connect(widgetModel, SIGNAL(forgotPasswordClicked(QString)),
                         q_ptr, SLOT(forgotPassword(QString)));
    }

    widgetModel->setPasswordFieldVisible(!doNotShowPassword);

    QObject::connect(widgetModel, SIGNAL(signInClicked()),
                     q_ptr , SLOT(signIn()));

    credentialWidget = new CredentialWidget(widgetModel);
    QObject::connect(credentialWidget, SIGNAL(doSignIn()),
            q_ptr, SLOT(signIn()));

    hideWidget = new QGraphicsWidget();
    QGraphicsLinearLayout *widgetLayout = new QGraphicsLinearLayout;
    widgetLayout->setOrientation(Qt::Vertical);
    widgetLayout->addItem(credentialWidget);
    widgetLayout->addStretch(2);
    widgetLayout->setSpacing(50);
    QMetaObject::invokeMethod(qmlBottomObject, "setSubscribeNewAccountText",
                              Q_ARG(QVariant, "Get one here"));
    widgetLayout->addItem(qmlBottomWidget);
    QObject::connect(qmlBottomObject, SIGNAL(registerNewAccount()),
                     q_ptr, SLOT(registerNew()));
    hideWidget->setLayout(widgetLayout);
    layout->addItem(hideWidget);

}


GenericAccountWidget::GenericAccountWidget(AbstractAccountSetupContext *context)
    : d_ptr(new GenericAccountWidgetPrivate(this))
{
    Q_D(GenericAccountWidget);
    d->context = context;
//    connect(this, SIGNAL(validateSignal()), d->context, SIGNAL(validated()));
    connect(this, SIGNAL(validateSignal()), d->context, SLOT(validate()));
    connect(d->context, SIGNAL(error(AccountsUI::ErrorCode, const QString&)), SLOT(validationOver()));
    connect(d->context, SIGNAL(validated()), SLOT(validationOver()));
    connect(d->context,
            SIGNAL(error(AccountsUI::ErrorCode, const QString&)),
            SLOT(errorSlot()));

    d->layout = new QGraphicsLinearLayout();
    d->layout->setOrientation(Qt::Vertical);

    d->engine = new QDeclarativeEngine;
    QDeclarativeComponent *component =
            new QDeclarativeComponent(d->engine,
                                    QUrl("qrc:/qml/mainPage.qml"));
    if (component->isError())
        qWarning() << component->errors();
    else
        d->qmlObject = component->create();

    QGraphicsObject *content = qobject_cast<QGraphicsObject*>(d->qmlObject);
    d->qmlWidget = new QGraphicsWidget;
    if(content)
        content->setParentItem(d->qmlWidget);

    connect(this, SIGNAL(navigateToServiceSelectionPage()),
            this, SLOT(openServiceSelectionPage()));

    d->qmlWidget->setMinimumSize(1000,150);
    /* TODO: when the credential widget is in qml put the below widget in
    single qml file */
    QDeclarativeComponent *bottomComponent =
            new QDeclarativeComponent(d->engine,
                                    QUrl("qrc:/qml/AccountsBottomWidget.qml"));
    if (bottomComponent->isError())
        qWarning() << bottomComponent->errors();
    else
        d->qmlBottomObject = bottomComponent->create();

    QGraphicsObject *bContent = qobject_cast<QGraphicsObject*>(d->qmlBottomObject);
    d->qmlBottomWidget = new QGraphicsWidget;
    if(bContent)
        bContent->setParentItem(d->qmlBottomWidget);
    d->qmlBottomWidget->setMinimumSize(1000,150);

    d->layout->addItem(d->qmlWidget);
    d->layout->setItemSpacing(0, 50);
    setLayout(d->layout);
}

GenericAccountWidget::~GenericAccountWidget()
{
}

void GenericAccountWidget::setDomDocument(QDomDocument providerAccountDocument)
{
    Q_D(GenericAccountWidget);
    d->providerAccountDocument = providerAccountDocument;
    //call the ui after domdocument is set
    d->createUiFromXml(d->providerAccountDocument);
}

QDomDocument GenericAccountWidget::domDocument()
{
    Q_D(GenericAccountWidget);
    QDomDocument providerAccountDocument;
    QString errorStr;
    int errorLine;
    int errorColumn;

    if (!providerAccountDocument.setContent(d->providerAccountDocument.toString(),
                                            true,
                                            &errorStr,
                                            &errorLine,
                                            &errorColumn) ) {
        QString topic("DOM error reading of account provider domdocument");
        QString message("Parse error at line %1, column %2:\n%3");
        message = message.arg(errorLine).arg(errorColumn).arg(errorStr);
        Q_ASSERT_X(false, topic.toAscii(), message.toAscii());
        qWarning() << topic << message;
    }
    return providerAccountDocument;
}

const QString GenericAccountWidget::username() const
{
    Q_D(const GenericAccountWidget);
    return d->username;
}

const QString GenericAccountWidget::password() const
{
    Q_D(const GenericAccountWidget);
    return d->password;
}

const QString GenericAccountWidget::providerName() const
{
    Q_D(const GenericAccountWidget);
    return d->providerName;
}

bool GenericAccountWidget::rememberMe() const
{
    Q_D(const GenericAccountWidget);
    return d->rememberMe;
}

QDeclarativeEngine *GenericAccountWidget::engine()
{   //Call after the constructor
    Q_D(const GenericAccountWidget);
    return d->engine;
}

QObject *GenericAccountWidget::ui()
{
    Q_D(const GenericAccountWidget);
    return d->qmlObject;
}

void GenericAccountWidget::setUsernameDisplayString(const QString &displayString)
{
    Q_D(const GenericAccountWidget);
    d->credentialWidget->setUsernameDisplayString(displayString);
}

void GenericAccountWidget::registerNew()
{
    Q_D(GenericAccountWidget);
    if (!QDesktopServices::openUrl(QUrl(d->registerNewLink)))
    qWarning() << Q_FUNC_INFO << "Unable to open web browser";
}

void GenericAccountWidget::forgotPassword(QString link)
{
    if (!QDesktopServices::openUrl(QUrl(link)))
        qWarning() << Q_FUNC_INFO << "Unable to open web browser";
}

void GenericAccountWidget::signIn()
{
    Q_D(GenericAccountWidget);
    if (d->widgetModel->username().isEmpty()) {
        //% "Fill in username"
        showInfoBanner(d->qmlObject, "Fill in username");
        d->credentialWidget->setFocusOnUserNameField();
        return;
    }

    if (!d->doNotShowPassword && d->widgetModel->password().isEmpty()) {
        //% "Fill in password"
        showInfoBanner(d->qmlObject, "Fill in password");
        d->credentialWidget->setFocusOnPasswordField();
        return;
    }

    if (!d->authDomainSeparator.isEmpty() && !d->authDomainDefault.isEmpty() &&
        !d->widgetModel->username().contains(d->authDomainSeparator)){
        // User did not enter the required domain name. Hence using the default domain name.
        d->widgetModel->blockSignals(true);
        QString domainDefault = d->authDomainSeparator + qtTrId(d->authDomainDefault.toLatin1());
        d->widgetModel->setUsername(d->widgetModel->username() + domainDefault);
        d->widgetModel->blockSignals(false);
    }

    Accounts::AccountIdList idList = AccountsManager::instance()->accountList();
    foreach (Accounts::AccountId id, idList) {
        Accounts::Account *account = AccountsManager::instance()->account(id);
        if ((d->widgetModel->username() == account->displayName()) &&
            (d->provider == account->providerName())) {

            QString messageBoxText = QString("%1\n%2").
                arg(qtTrId("qtn_acc_one_account_allowed").arg(d->providerName)).
                arg(qtTrId("qtn_acc_tune_it_in_accounts"));

            //TODO: qmlize this MessageBox
            QMessageBox queryBox;
            queryBox.setText(qtTrId("qtn_acc_account_exists"));
            queryBox.setInformativeText(messageBoxText);
            QPushButton  *continueButton = queryBox.addButton("Continue", QMessageBox::ActionRole);
            QAbstractButton  *cancelButton = queryBox.addButton("Cancel", QMessageBox::RejectRole);

            queryBox.exec();

            ProviderPluginProcess *pluginProcess = ProviderPluginProcess::instance();

            if (queryBox.clickedButton() == continueButton) {
                QVariant data(AccountsUI::ProviderPluginProxy::EDIT_EXISTING_ACCOUNT);
                pluginProcess->setExitData(data);
                pluginProcess->setEditExistingAccount(id);
                pluginProcess->quit();
            } else if (queryBox.clickedButton() == cancelButton) {
                pluginProcess->quit();
            }

            queryBox.removeButton(continueButton);
            queryBox.removeButton(continueButton);

            delete continueButton;
            delete cancelButton;
        }
    }
    d->username = d->widgetModel->username();
    d->password = d->widgetModel->password();
    d->rememberMe = d->widgetModel->checkboxPressed();

    validateCredentials();

}

void GenericAccountWidget::validateCredentials()
{
    Q_D(GenericAccountWidget);
    if (d->accountValidationInProgress == true)
        return;

    d->accountValidationInProgress = true;
    emit validateSignal();
}

void GenericAccountWidget::validationOver()
{
    Q_D(GenericAccountWidget);
    d->accountValidationInProgress = false;
}

void GenericAccountWidget::errorSlot()
{
    /*
     * TODO: define
     * actual functionality for this
     * */

    Q_D(GenericAccountWidget);
    d->accountValidationInProgress = false;
}

void GenericAccountWidget::openServiceSelectionPage()
{
    Q_D(GenericAccountWidget);

//    d->hideWidget->setVisible(false);

//    QMetaObject::invokeMethod(d->qmlObject, "openServiceSelectionPage",
//                              Q_ARG(QVariant, "add"));
}

}
