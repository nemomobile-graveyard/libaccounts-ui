#ifndef ACCOUNTSETUPFINISHEDPAGE_H
#define ACCOUNTSETUPFINISHEDPAGE_H

#include <MApplicationPage>
#include "common.h"

namespace Accounts {
    class Account;
}

namespace AccountsUI {

class AbstractAccountSetupContext;
class AccountSetupFinishedPagePrivate;

/*!
 * @details AccountSetupFinishedPage is the page which is displayed
 * when accounts-ui is launched by any external application like email and
 * after successful creation of an account.
 */

class ACCOUNTSUI_EXPORT AccountSetupFinishedPage : public MApplicationPage
{
    Q_OBJECT

public:
    AccountSetupFinishedPage(Accounts::Account *account, QString serviceType);
    ~AccountSetupFinishedPage();

    virtual void createContent();

//    static AccountSetupFinishedPage *instance();

public slots:
    void goToApplication();
    void addMoreAccount();

Q_SIGNALS:
    void killAccountsUi();

private:
    AccountSetupFinishedPagePrivate* d_ptr;
    Q_DISABLE_COPY(AccountSetupFinishedPage)
    Q_DECLARE_PRIVATE(AccountSetupFinishedPage)

};

} //namespace

#endif // ACCOUNTSETUPFINISHEDPAGE_H
