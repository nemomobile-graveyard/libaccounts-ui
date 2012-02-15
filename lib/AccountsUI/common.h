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

#ifndef ACCOUNTSUI_COMMON_H
#define ACCOUNTSUI_COMMON_H

//Qt
#include <QtCore/QtGlobal>
#include <QString>

//SignOn
#include <SignOn/Error>

//M
#ifndef ACCOUNTSUI_DISABLE_DEPRECATED
#include <MInfoBanner>
#endif

#define ACCOUNTSUI_EXPORT Q_DECL_EXPORT
#define SOUND_INFO QLatin1String("warning_snd")
namespace AccountsUI {

typedef SignOn::Error::ErrorType SignonErrType;

/*!
 * @class ErrorMessageDisplayHelper
 * @headerfile AccountsUI/common.h \
 * AccountsUI/Common
 * @brief Base class for error display helpers
 * @see void GenericAccountSetupContext::installErrorMessageDisplayHelper()
 *
 * Use this class to support additional translation ids for error types,
 * rather than only the generic default ones the `trIdFromSignonError()`
 * implementation has to offer.
 * @note The GenericAccountSetupContext instance will take ownership of the
 * installed ErrorDisplayHelper */
class ACCOUNTSUI_EXPORT ErrorMessageDisplayHelper
{
    Q_DISABLE_COPY(ErrorMessageDisplayHelper)

public:
    enum ErrorContext {
        UserInputErr = 0,
        AccountValidationErr,
        AccountSavingErr
    };

    /*!
     * Basic constructor */
    explicit ErrorMessageDisplayHelper() {}
    /*!
     * @param text The text to be displayed
     * @param errContext The error's context
     * @param providerName The service provider associated with the account
     * @note This ought to be a blocking call. In the case of displaying
     * a modal dialog, this method must return upon dialog closure only.
     */
    virtual void displayMessage(const QString &text,
                                const ErrorContext errContext = AccountValidationErr,
                                const QString &providerName = QString::null);

    /*!
     * @param errCode The errCode
     * @param errContext The error's context
     * @param providerName The service provider associated with the account
     * @note This ought to be a blocking call. In the case of displaying
     * a modal dialog, this method must return upon dialog closure only.
     */
    virtual void displayMessage(const int errCode,
                                const ErrorContext errContext = AccountValidationErr,
                                const QString &providerName = QString::null);
};

/*!
 * @class ErrorTrHelper
 * @headerfile AccountsUI/common.h \
 * AccountsUI/Common
 * @brief Base class for error translation helpers
 * @see void GenericAccountSetupContext::installErrorTrHelper()
 * @see const QString trIdFromSignonError()
 *
 * Use this class to support additional translation ids for error types,
 * rather than only the generic default ones the `trIdFromSignonError()`
 * implementation has to offer.
 * @note The GenericAccountSetupContext instance will take ownership of the
 * installed ErrorTrHandler */
class ACCOUNTSUI_EXPORT ErrorTrHelper
{
    Q_DISABLE_COPY(ErrorTrHelper)

public:
    /*!
     * Basic constructor */
    explicit ErrorTrHelper();
    /*!
     * @param errType The error type for which a translation id will be
     * computed at the next `trId()` call. */
    void setErrorType(const int errType);
    /*!
     * Computes a translation id based on the currently set error type.
     * Base implementation resets the internal error type to `undefinedErr`
     * and returns an empty string. Should this method return an empty string.
     * Reimplement this to provide additional translation ids.
     * @returns translation for the error message corresponding to the
     *          previously set error type. */
    virtual QString trId();

protected:
    /*!
     * Defines the undefined error value. */
    const int undefinedErr;
    int errType;
};

/*!
 * @deprecated
 * Helper function. Maps SignOn error types to logical translation ids.
 * @param err the SignOn error type.
 * @param providerName the provider name.
 * @returns corresponding translation id string.
 */
ACCOUNTSUI_EXPORT
const QString trIdFromSignonError(const SignonErrType err,
                                  const QString& providerName = NULL);

/*!
 * @deprecated
 * @overload trIdFromSignonError(const SignonErrType err)
 */
ACCOUNTSUI_EXPORT
const QString trIdFromSignonError(const int err);

/*!
 * Helper function. Maps SignOn error types to logical translation ids.
 * @param err the SignOn error type.
 * @param providerName the provider name.
 * @param errTrHelper brings in additional error types support
 * @returns corresponding translation id string.
 */
ACCOUNTSUI_EXPORT
const QString trIdFromSignonError(const SignonErrType err,
                                  const QString& providerName,
                                  ErrorTrHelper *errTrHelper);

/*!
 * @overload trIdFromSignonError(const SignonErrType err)
 */
ACCOUNTSUI_EXPORT
const QString trIdFromSignonError(const int err,
                                  ErrorTrHelper *errHelper);

#ifndef ACCOUNTSUI_DISABLE_DEPRECATED
/*!
 * Displays an information banner.
 * @param text Text to be displayed.
 * @param type The type of the info banner
 * @param disapperTimeout Amount of seconds after which the banner disappears.
 *
 * @deprecated This function is deprecated and will be removed in a future
 * version.
 */
ACCOUNTSUI_EXPORT
void showInfoBanner(const QString &text,
                    const MInfoBanner::BannerType type = MInfoBanner::Information,
                    const quint32 disapperTimeout = 3000,
                    const QString &feedback = SOUND_INFO);
#endif

/*!
 * @returns The product name translation id.
 */
ACCOUNTSUI_EXPORT const QString productNameTr();

ACCOUNTSUI_EXPORT bool isChinaVariant();

}

#endif // ACCOUNTSUI_COMMON_H
