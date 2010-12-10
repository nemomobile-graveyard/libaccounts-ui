/*
 * This file is part of accounts-ui
 *
 * Copyright (C) 2009-2010 Nokia Corporation.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "generic-account-setup-context.h"

//project library
#include <AccountsUI/AccountsManagerSingleton>
#include <AccountsUI/NetworkSessionManager>
#include <AccountsUI/Common>
#include "genericaccountsetupform.h"

//signon
#include <SignOn/Identity>
#include <SignOn/identityinfo.h>
#include <SignOn/SessionData>
#include <SignOn/Error>

//Qt
#include <QDebug>
#include <QTimer>

//accounts-qt lib
#include <Accounts/Account>

//Meegotouch
#include <MInfoBanner>

using namespace Accounts;
using namespace SignOn;

static QStringList parseRealms(QDomElement realmsElement)
{
    QStringList realms;

    if (!realmsElement.isNull()) {
        QDomElement realmElement = realmElement.firstChildElement("realm");
        while (!realmElement.isNull()) {
            QString realm = realmElement.text();

            if (!realm.isEmpty())
                realms.append(realm);
            realmElement = realmElement.nextSiblingElement("realm");
        }
    }

    return realms;
}

/* !! TODO - organize the XML parsing code - should not be spread through
             all the methods that need data from the XML conf file !!
*/
namespace AccountsUI {

class GenericAccountSetupContextPrivate
{
public:
    GenericAccountSetupContextPrivate(GenericAccountSetupContext *parent)
        : genericAccountSetupForm(0)
        , identity(0)
        , authSession(0)
        , networkManager(0)
        , contextIsValidated(false)
        , identityCreated(false)
        , credentialsStored(false)
        , q_ptr(parent)
    {}

    ~GenericAccountSetupContextPrivate()
    {
        if (identity)
            identity->deleteLater();

        if (networkManager) {
            networkManager->stopSession();
            networkManager->deleteLater();
        }
    }

    bool storeAccountPassword(const QDomDocument &domDocument);
    bool rememberPassword(const QDomDocument &domDocument);
    void disconnectAuthSessionSignals();

public:
    GenericAccountSetupForm *genericAccountSetupForm;

    IdentityInfo identityInfo;
    Identity *identity;
    AuthSession *authSession;

    NetworkSessionManager *networkManager;

    bool contextIsValidated;
    bool identityCreated;
    bool credentialsStored;

private:
    GenericAccountSetupContext *q_ptr;
    Q_DECLARE_PUBLIC(GenericAccountSetupContext)
};

bool GenericAccountSetupContextPrivate::storeAccountPassword(const QDomDocument &domDocument)
{
    QDomElement root = domDocument.documentElement();
    QDomElement element = root.firstChildElement("account-setup");
    QDomElement child = element.firstChildElement("setting");

    while (!child.isNull()) {
        if (!child.attribute("name").compare("store_password_in_accounts")) {
            if (!child.attribute("default_value").compare("true"))
                return true;
        }
        child = child.nextSiblingElement("setting");
    }

    return false;
}

bool GenericAccountSetupContextPrivate::rememberPassword(const QDomDocument &domDocument)
{
    QDomElement root = domDocument.documentElement();
    QDomElement element = root.firstChildElement("account-setup");
    QDomElement child = element.firstChildElement("setting");
    while (!child.isNull()) {
        if (!child.attribute("name").compare("remember")) {
            if (!child.attribute("value").compare("false"))
                return false;
        }
        child = child.nextSiblingElement("setting");
    }
    return true;
}

void GenericAccountSetupContextPrivate::disconnectAuthSessionSignals()
{
    Q_Q(GenericAccountSetupContext);

    if(authSession && q) {
        QObject::disconnect(authSession, SIGNAL(response(const SignOn::SessionData &)),
                            q, SLOT(authenticationDone(const SignOn::SessionData &)));
        QObject::disconnect(authSession, SIGNAL(error(const SignOn::Error &)),
                            q, SLOT(authSessionError(const SignOn::Error &)));
    }
}

GenericAccountSetupContext::GenericAccountSetupContext(Account *account,
                                                     SetupType type,
                                                     QObject *parent)
    : AbstractAccountSetupContext(account, type, parent)
    , d_ptr(new GenericAccountSetupContextPrivate(this))
{
    Q_D(GenericAccountSetupContext);

    d->identity = Identity::newIdentity();
    Q_ASSERT(d->identity);
    connect(d->identity, SIGNAL(credentialsStored(const quint32)),
            this, SLOT(credentialsReady(const quint32)));
    connect(d->identity, SIGNAL(error(const SignOn::Error &)),
            this, SLOT(storeCredentialsError(const SignOn::Error &)));
}

GenericAccountSetupContext::~GenericAccountSetupContext()
{
    if (d_ptr->credentialsStored &&
        !d_ptr->contextIsValidated)
    {
        qDebug() << Q_FUNC_INFO << "Need to delete credentials";
        d_ptr->identity->remove();
    }
    else
        qDebug() << Q_FUNC_INFO << "no need to remove credentials";

    delete d_ptr;
}

void GenericAccountSetupContext::store()
{
    Q_D(GenericAccountSetupContext);
    qDebug() << Q_FUNC_INFO;

    Q_ASSERT(!account()->providerName().isEmpty());
    if (account()->providerName().isEmpty()) {
        QString msg(QLatin1String("No provider was set for this account."));
        qWarning() << msg;
        emit error(UnknownError, msg);
        return;
    }
    account()->selectService(NULL);
    if (d->genericAccountSetupForm) {
        qDebug()<< d->genericAccountSetupForm->username();
        QString username = d->genericAccountSetupForm->username();
        QString password = d->genericAccountSetupForm->password();
        account()->setDisplayName(username);
         //TODO: check that username is valid(empty, mail address ...)
        account()->setValue("username", username);
        /*
        * FIXME: this is only temporary: it's needed for glib applications, while
        * libsignon-glib is not yet ready.
        */
        if (d->storeAccountPassword(d->genericAccountSetupForm->domDocument()))
            account()->setValue("password", password);

        disconnect(d->identity, SIGNAL(credentialsStored(const quint32)),
            this, SLOT(credentialsReady(const quint32)));
        connect(d->identity, SIGNAL(credentialsStored(const quint32)),
            this, SLOT(credentialsStored(const quint32)));
        storeIdentity();
    } else {
        emit stored();
    }
}

void GenericAccountSetupContext::storeIdentity()
{
    Q_D(GenericAccountSetupContext);
    qDebug() << Q_FUNC_INFO;

    d->identityInfo.setCaption(d->genericAccountSetupForm->providerName());
    d->identityInfo.setUserName(d->genericAccountSetupForm->username());
    bool storePassword = d->rememberPassword(d->genericAccountSetupForm->domDocument());
    if (storePassword)
        storePassword = d->genericAccountSetupForm->rememberMe();
    d->identityInfo.setSecret(d->genericAccountSetupForm->password(),
                               storePassword);
    d->identityInfo.setStoreSecret(storePassword);

    if (d->identityCreated) {
        d->identity->storeCredentials(d->identityInfo);
        return;
    }

    //add ACL and other data for identity info
    QDomDocument domDocument = d->genericAccountSetupForm->domDocument();

    QDomElement root = domDocument.documentElement();
    QDomElement element = root.firstChildElement("account-setup");
    QDomElement authSection = element.firstChildElement("authsession");
    QDomElement child = authSection.firstChildElement("method");

    //all tokens from different methods and mechanisms will be added into same ACL
    QStringList acl = QStringList();

    while (!child.isNull()) {
        if (child.hasAttribute("name")) {
            MechanismsList mechanisms = QStringList();
            QDomNamedNodeMap attribs = child.attributes();
            for (int i=0; i<attribs.count(); i++) {
                if (attribs.item(i).nodeName() == "token")
                    acl.append(attribs.item(i).nodeValue());
            }
            QString method = child.attribute("name");
            qDebug() << Q_FUNC_INFO << "method name: " << method;
            QDomElement mech = child.firstChildElement("mechanism");
            while (!mech.isNull()) {
                if (mech.hasAttribute("name")) {
                    QString value = mech.attribute("name");
                    mechanisms.append(value);

                    QDomElement token = child.firstChildElement("tokens");
                    QStringList tokens = token.text().split(",", QString::SkipEmptyParts);
                    acl.append(tokens);
                    //                        acl.append(value);
                    token = token.nextSiblingElement("tokens");

                    QDomNamedNodeMap attribs = mech.attributes();
                    for (int i=0; i<attribs.count(); i++) {
                        if (attribs.item(i).nodeName() == "token")
                            acl.append(attribs.item(i).nodeValue());
                    }
                }
                mech = mech.nextSiblingElement("mechanism");
            }
            //insert into info
            d->identityInfo.setMethod(method, mechanisms);
        }
        child = child.nextSiblingElement("method");
    }

    //clean ACL and insert into info
    acl.removeDuplicates();
    d->identityInfo.setAccessControlList(acl);

    QStringList realms =
        parseRealms(authSection.firstChildElement("realms"));
    d->identityInfo.setRealms(realms);

    d->identity->storeCredentials(d->identityInfo);
}

MWidget *GenericAccountSetupContext::widget(QGraphicsItem *parent)
{
    qDebug() << Q_FUNC_INFO;
    Q_D(GenericAccountSetupContext);

    if (setupType() == EditExisting)
        return 0;

    if (d->genericAccountSetupForm) {
        qWarning() << Q_FUNC_INFO << "There is an existing widget so it will return this.";
        return d->genericAccountSetupForm;
    }

    Q_ASSERT(account());
    if (!account()) {
        return 0;
    }

    QString providerName = account()->providerName();
    Accounts::Provider *provider = AccountsManager::instance()->provider(providerName);

    Q_ASSERT(provider);
    if (!provider) {
        return 0;
    }

    d->genericAccountSetupForm = new GenericAccountSetupForm(this, parent);
    d->genericAccountSetupForm->setDomDocument(provider->domDocument());

    return d->genericAccountSetupForm;
}

SignOn::Identity *GenericAccountSetupContext::identity() const
{
    Q_D(const GenericAccountSetupContext);
    return d->identity;
}

QString GenericAccountSetupContext::userName() const
{
    Q_D(const GenericAccountSetupContext);
    return d->identityInfo.userName();
}

void GenericAccountSetupContext::validate()
{
    Q_D(GenericAccountSetupContext);
    qDebug() << Q_FUNC_INFO;

    if (d->contextIsValidated == true) {
        qDebug() << Q_FUNC_INFO << "context is validated already";
        emit validated();
        return;
    }

    if (setupType() != CreateNew) {
        qDebug() << Q_FUNC_INFO << __LINE__;
        d->contextIsValidated = true;
        emit validated();
        return;
    }

    // set the user name for the AbstractAccountSetupContext
    setUserName(d->genericAccountSetupForm->username());

    emit validating();

    //check authsession part of provider file
    QDomDocument domDocument = d->genericAccountSetupForm->domDocument();
    QDomElement root = domDocument.documentElement();
    QDomElement element = root.firstChildElement("account-setup");
    QDomElement authSection = element.firstChildElement("authsession");

    if (authSection.isNull()) {
        //no section how to do validation, just emit validated
        qDebug() << Q_FUNC_INFO << __LINE__;
        d->contextIsValidated = true;
        emit validated();
        return;
    }

    if (d->networkManager == 0) {
        d->networkManager = new NetworkSessionManager;

        connect(d->networkManager, SIGNAL(sessionReady()), SLOT(storeIdentity()));
        connect(d->networkManager, SIGNAL(error(ErrorType)), SLOT(networkSessionError()));

        d->networkManager->startSession();

    } else if (!d->networkManager->isSessionReady()) {
        d->networkManager->startSession();
    } else {
        storeIdentity();
    }
}

void GenericAccountSetupContext::startAuthSession()
{
    Q_D(GenericAccountSetupContext);

    //check authsession part of provider file
    QDomDocument domDocument = d->genericAccountSetupForm->domDocument();
    QDomElement root = domDocument.documentElement();
    QDomElement element = root.firstChildElement("account-setup");
    QDomElement authSection = element.firstChildElement("authsession");

    QString method = QString();
    QString mechanism = QString();
    QVariantMap data;
    //first check parameters how to validate credentials
    QDomElement child = authSection.firstChildElement("validate");
    if (!child.isNull()) {
        QDomNamedNodeMap attribs = child.attributes();
        for (int i=0; i<attribs.count(); i++) {
            QString name = attribs.item(i).nodeName();
            QString value = attribs.item(i).nodeValue();
            qDebug() << Q_FUNC_INFO << "name: " << name;
            qDebug() << Q_FUNC_INFO << "value: " << value;
            if (name == QLatin1String("method")) {
                method = value;
            } else if (name == QLatin1String("mechanism")) {
                mechanism = value;
            } else { //give rest of params to plugin as they are
                data.insert(name, value);
            }
        }
    }

    SignOn::SessionData sessionData(data);

    sessionData.setUiPolicy(SignOn::ValidationPolicy);
    sessionData.setUserName(d->genericAccountSetupForm->username());
    sessionData.setSecret(d->genericAccountSetupForm->password());

    if (!d->authSession) {
        d->authSession = d->identity->createSession(method);
    }

    if (!d->authSession) {
        qWarning() << QLatin1String("Try to make concurrent authentication");
        emit error(UnknownError,
                   QLatin1String("Could not create authetication session."));

        /*!! If this method gets called while another auth session is processing
             there is a programming error in the controller. This code should
             get executed only if the session cretion fails for reasons different
             than of another session for this method being already requsted!! */
        return;
    }

    connect(d->authSession, SIGNAL(response(const SignOn::SessionData &)),
            this, SLOT(authenticationDone(const SignOn::SessionData &)));
    connect(d->authSession, SIGNAL(error(const SignOn::Error &)),
            this, SLOT(authSessionError(const SignOn::Error &)), Qt::QueuedConnection);

    d->authSession->process(sessionData, mechanism);
}

void GenericAccountSetupContext::stopAuthSession()
{
    Q_D(GenericAccountSetupContext);

    d->disconnectAuthSessionSignals();
    d->authSession->cancel();
    d->networkManager->stopSession();
}

void GenericAccountSetupContext::authSessionError(const SignOn::Error &err)
{
    qDebug() << QString::fromLatin1("AUTHENTICATION ERROR type %1, message %2:")
        .arg(err.type()).arg(err.message());

    Q_D(GenericAccountSetupContext);

    d->disconnectAuthSessionSignals();

    AuthSession *senderSession = qobject_cast<SignOn::AuthSession *>(sender());
    if (senderSession) {
        d->identity->destroySession(senderSession);
        if (senderSession == d->authSession)
            d->authSession = NULL;
    }
    //TODO handle user cancel in case of url or captcha popup !!!!!

    //Connection errors should be handled by Connectivity, but their Ui
    //is not quite in the proper shape
    showInfoBanner(trIdFromSignonError((SignonErrType)err.type()));

    emit error(UnknownError, err.message());
}

void GenericAccountSetupContext::authenticationDone(const SignOn::SessionData &data)
{
    Q_UNUSED(data);
    Q_D(GenericAccountSetupContext);

    d->disconnectAuthSessionSignals();

    AuthSession *authSession = qobject_cast< SignOn::AuthSession *>(sender());
    if (authSession) {
        d->identity->destroySession(authSession);
        if (authSession == d->authSession)
            d->authSession = NULL;
    }

    d->contextIsValidated = true;

    emit validated();
}

void GenericAccountSetupContext::storeCredentialsError(const SignOn::Error &err)
{
    Q_D(GenericAccountSetupContext);

    qDebug() << "Error :" << err.type() << ":" << err.message();
    showInfoBanner(trIdFromSignonError(Error::StoreFailed));

    d->contextIsValidated = false;

    d->disconnectAuthSessionSignals();

    emit error(UnknownError, err.message());
}

void GenericAccountSetupContext::credentialsStored(const quint32 id)
{
    Q_D(GenericAccountSetupContext);
    qDebug() << "SSO stored id: " << id;
    account()->selectService(NULL);
    account()->setCredentialsId(id);

    d->credentialsStored = true;

    d->disconnectAuthSessionSignals();

    emit stored();
}

void GenericAccountSetupContext::credentialsReady(const quint32 id)
{
    Q_D(GenericAccountSetupContext);
    qDebug() << "SSO stored id: " << id;
    if (d->identity->id() != id) {
      qDebug() << "identity id do not match!";
    }

    d->credentialsStored = true;
    startAuthSession();
}

void GenericAccountSetupContext::networkSessionError()
{
    Q_D(GenericAccountSetupContext);

    d->disconnectAuthSessionSignals();

    qDebug() << "NETWORK SESSION ERROR";
    //todo - optimize so that this is sent only if there
    //is an ongoing auth session process
    emit error(UnknownError, QLatin1String("Network session error."));
}

} // namespace
