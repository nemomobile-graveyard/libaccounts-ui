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

#include "settings-widgets.h"
#include "parser.h"

#include <QObject>
#include <QMetaObject>

/// Useful help functions:

// +-------------+
// | label       |
// | Widget....  |
// +-------------+
// @text is handed over.
static MLayout *wrapWithLabelAbove(QGraphicsLayoutItem *widget, const QString &text)
{
    MLayout *formElementLayout =  new MLayout();
    // only 1 policy!
    MLinearLayoutPolicy *formElementLayoutPolicy =
        new MLinearLayoutPolicy(formElementLayout, Qt::Vertical);
    formElementLayoutPolicy->setSpacing(0);

    formElementLayoutPolicy->addItem(new MLabel(text));
    formElementLayoutPolicy->addItem(widget);

    qDebug() << "created label" << text;
    return formElementLayout;
}

// this is common for 1-key value widgets:
static QString extractKey(const QDomElement &node)
{
    QString key;
    if (node.hasAttribute("key"))
        key = QString(node.attribute("key"));
    else
        key = QString(node.attribute("name"));
    return key;
}

// return the string-ID of the Label, for the @node.
static QString descriptionLabelText(const QDomElement &node, const QString &key)
{
    QString descriptionLabelString;
    if (node.hasAttribute("label")) {
        descriptionLabelString = qtTrId(node.attribute("label").toLatin1());
    } else {
        descriptionLabelString = QString(key);
        descriptionLabelString.replace(QLatin1Char('_'), QLatin1Char(' ')); // fixme!
    }
    // todo: translate
    return descriptionLabelString;
}


QGraphicsLayoutItem *ParameterText::ui(const QDomElement &node,
                                       GenericServiceSetupContext *context,
                                       Parser *parser,
                                       bool showAllSettings)
{
    Q_UNUSED(parser);
    Q_UNUSED(showAllSettings);
    qDebug() << Q_FUNC_INFO;
    // bind to the account-setting value:
    key = extractKey(node);
    context->addAccountSetting(this);
    if (context->settingsMap.contains(key)) {
        MTextEditModel *model = qobject_cast<MTextEditModel*>(context->settingsMap.value(key));
        editor = new MTextEdit(model, 0);
    } else {
        editor = new MTextEdit(MTextEditModel::SingleLine, QString());
        editor->setText(context->account()->valueAsString(key));
        context->settingsMap.insert(key, editor->model());
    }
    // tune the MWidget
    editor->setStyleName(QString(key));
    editor->setInputMethodCorrectionEnabled (false);
    QString tag = node.tagName();
    if (tag == "password") {
        editor->setEchoMode(MTextEditModel::Password);
    } else if (tag == "email") {
        editor->setContentType(M::EmailContentType);
    }
    // add a Label:
    if (node.hasAttribute("nolabel"))
        return editor;
    else {
        QGraphicsLayoutItem *withLabel =
        wrapWithLabelAbove(editor, descriptionLabelText(node, key));
        qDebug() << Q_FUNC_INFO;
        return withLabel;
    }
};


QGraphicsLayoutItem *ParameterInteger::ui(const QDomElement &node,
                                          GenericServiceSetupContext *context,
                                          Parser *parser,
                                          bool showAllSettings)
{
    Q_UNUSED(parser);
    Q_UNUSED(showAllSettings);
    key = extractKey(node);

    if (context->settingsMap.contains(key)) {
        MTextEditModel *model = qobject_cast<MTextEditModel*>(context->settingsMap.value(key));
        editor = new MTextEdit(model, 0);
    } else {
        editor = new MTextEdit(MTextEditModel::SingleLine, QString());
        editor->setText(context->account()->valueAsString(key));
        context->settingsMap.insert(key, editor->model());
    }

    editor->setStyleName(QString(key));
    editor->setContentType(M::NumberContentType);
    editor->setInputMethodCorrectionEnabled(false);

    context->addAccountSetting(this);

    QGraphicsLayoutItem *withLabel =
        wrapWithLabelAbove (editor, descriptionLabelText(node, key));
    return withLabel;
};

QGraphicsLayoutItem *ParameterBoolean::ui(const QDomElement &node,
                                          GenericServiceSetupContext *context,
                                          Parser *parser,
                                          bool showAllSettings)
{
    Q_UNUSED(parser);
    Q_UNUSED(showAllSettings);
    key = extractKey(node);
    context->addAccountSetting(this);
    // value:
    if (context->settingsMap.contains(key)) {
        MButtonModel *model = qobject_cast<MButtonModel*>(context->settingsMap.value(key));
        button = new MButton(0, model);
    } else {
        button = new MButton(0);
        bool checked = context->account()->valueAsBool(key);
        button->setChecked(checked);
        context->settingsMap.insert(key, button->model());
    }
    button->setStyleName(QString(key));
    button->setCheckable(true);

    // instead of label:
    button->setText(descriptionLabelText(node, key));
    return button;
}
