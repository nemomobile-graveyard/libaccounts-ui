#ifndef GENERICACCOUNTWIDGET_H
#define GENERICACCOUNTWIDGET_H

#include <QDeclarativeItem>
#include <QDomDocument>
#include <QDeclarativeEngine>

#include <AccountsUI/abstract-account-setup-context.h>

namespace AccountsUI
{
class GenericAccountWidgetPrivate;

class GenericAccountWidget : public QDeclarativeItem
{
    Q_OBJECT
public:
    explicit GenericAccountWidget(AbstractAccountSetupContext *context);
    ~GenericAccountWidget();

    QGraphicsWidget *widget();
    void setDomDocument(QDomDocument providerAccountDocument);
    QDomDocument domDocument();
    const QString username() const;
    const QString password() const;
    const QString providerName() const;
    bool rememberMe() const;
    QDeclarativeEngine *engine();

private slots:
    void registerNew();
    void forgotPassword(QString link);
    void signIn();
    void validateCredentials();
    void validationOver();
    void errorSlot();
    void openServiceSelectionPage();

Q_SIGNALS:
    void validateSignal();
    void navigateToServiceSelectionPage();

private:
    GenericAccountWidgetPrivate *d_ptr;
    Q_DISABLE_COPY(GenericAccountWidget)
    Q_DECLARE_PRIVATE(GenericAccountWidget);

};
}

#endif // GENERICACCOUNTWIDGET_H
