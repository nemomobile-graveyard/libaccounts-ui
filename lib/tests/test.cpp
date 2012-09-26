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

#include "test.h"

#include <Accounts/Manager>
#include <AccountsUI/ServiceHelper>
#include <QtTest/QtTest>

using namespace Accounts;
using namespace AccountsUI;


void clearDb()
{
    QDir dbroot(QString(getenv("ACCOUNTS")));
    dbroot.remove(QString("accounts.db"));
}

void Test::initTestCase()
{
    //init
    setenv("ACCOUNTS", "/tmp/", TRUE);
    setenv("AG_SERVICES", SERVICES_DIR, TRUE);
    setenv("AG_PROVIDERS", PROVIDERS_DIR, TRUE);
    //clear database
    clearDb();
}

void Test::cleanupTestCase()
{

}


/* manager */
void Test::serviceHelperTest()
{
    Manager *manager = new Manager();

    Service service = manager->service("gmail");
    ServiceHelper *serviceHelper = new ServiceHelper(service);
    QVERIFY(serviceHelper != NULL);

    QCOMPARE(serviceHelper->service(), service);

    QCOMPARE(serviceHelper->prettyName(), QString("GMail"));

    QCOMPARE(serviceHelper->providerName(), QString("google"));

    delete serviceHelper;
    delete manager;
}


QTEST_MAIN(Test)
