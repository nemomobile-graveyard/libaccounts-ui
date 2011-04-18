/*
 * This file is part of accounts-ui
 *
 * Copyright (C) 2009-2010 Nokia Corporation.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@nokia.com>
 * Contact: Lucian Horga <ext-lucian.horga@nokia.com>
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

//project
#include "credentialwidget.h"
#include "credentialwidgetview.h"

//Qt
#include <QDebug>

//M
#include <MTheme>
#include <MWidgetCreator>
#include <MLibrary>

M_LIBRARY

CredentialWidget::CredentialWidget(CredentialWidgetModel *model, QGraphicsItem *parent)
    : MWidgetController(model == 0 ? new CredentialWidgetModel : model, parent)
{
    MWidgetView *defaultWidgetView = MTheme::view(this);

    // looks at the .conf file and constructs the view there.
    Q_ASSERT_X(defaultWidgetView,
               "no CredentialWidgetView",
               "There is no available WidgetView class for this widget - is there an existing and correct conf file?");
    setView(defaultWidgetView);
    setStyleName("CredentialWidget");
}

CredentialWidget::~CredentialWidget()
{
}

void CredentialWidget::showEvent(QShowEvent *event)
{
    model()->setFocusOnPasswordField(true);
    MWidgetController::showEvent(event);
}

QString CredentialWidget::username()
{
    Q_ASSERT(model());
    return model()->username();
}

QString CredentialWidget::password()
{
    Q_ASSERT(model());
    return model()->password();
}

QString CredentialWidget::captchaText()
{
    Q_ASSERT(model());
    return model()->captchaText();
}

bool CredentialWidget::rememberPasswordChecked()
{
    Q_ASSERT(model());
    return model()->checkboxPressed();
}

bool CredentialWidget::enabled()
{
    Q_ASSERT(model());
    return model()->enabled();
}

void CredentialWidget::setUsername(const QString &aUserName)
{
    Q_ASSERT(model());
    model()->setUsername(aUserName);
}

void CredentialWidget::setPassword(const QString &aPassword)
{
    Q_ASSERT(model());
    model()->setPassword(aPassword);
}

void CredentialWidget::setCaptcha(const QImage &image)
{
    Q_ASSERT(model());
    model()->setCaptcha(image);
}

void CredentialWidget::setRememberPasswordChecked(bool checked)
{
    Q_ASSERT(model());
    model()->setCheckboxPressed(checked);
}

void CredentialWidget::setFocusOnUserNameField()
{
    CredentialWidgetView* cdView = NULL;
    cdView = const_cast<CredentialWidgetView*>(qobject_cast<const CredentialWidgetView*> (view()));
    if (NULL != cdView)
        cdView->setFocusOnUserNameField();
}

void CredentialWidget::setFocusOnPasswordField()
{
    CredentialWidgetView* cdView = NULL;
    cdView = const_cast<CredentialWidgetView*>(qobject_cast<const CredentialWidgetView*> (view()));
    if (NULL != cdView)
        cdView->setFocusOnPasswordField();
    model()->setFocusOnPasswordField(true);
}

void CredentialWidget::setFocusOnCaptchaField()
{
    Q_ASSERT(model());
    model()->setFocusOnCaptchaTextField(true);
}

void CredentialWidget::setEnabled(bool value)
{
    Q_ASSERT(model());
    model()->setEnabled(value);
}

void CredentialWidget::setInformativeNoteText(const QString &text)
{
    Q_ASSERT(model());
    model()->setInformativeNoteText(text);
}

M_REGISTER_WIDGET_NO_CREATE(CredentialWidget);
