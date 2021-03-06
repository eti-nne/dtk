// Version: $Id$
//
//

// Commentary:
//
//

// Change Log:
//
//

// Code:

#pragma once

#include <QtCore>
#include <QDebug>

extern int logLevel;

#define LogError()   if (logLevel < 1) {} else qDebug() << "ERROR:"
#define LogWarning() if (logLevel < 1) {} else qDebug() << "WARNING:"
#define LogNormal()  if (logLevel < 2) {} else qDebug() << "Log:"
#define LogDebug()   if (logLevel < 3) {} else qDebug() << "Log:"

extern bool runStripEnabled;

class FrameworkInfo
{
public:
    QString frameworkDirectory;
    QString frameworkName;
    QString frameworkPath;
    QString binaryDirectory;
    QString binaryName;
    QString binaryPath;
    QString version;
    QString installName;
    QString deployedInstallName;
    QString sourceFilePath;
    QString destinationDirectory;
};

  bool operator==(const FrameworkInfo &a, const FrameworkInfo &b);
QDebug operator<<(QDebug debug, const FrameworkInfo &info);

class ApplicationBundleInfo
{
    public:
    QString path;
    QString binaryPath;
};

class DeploymentInfo
{
public:
    QString qtPath;
    QString pluginPath;
    QStringList deployedFrameworks;
};

inline QDebug operator<<(QDebug debug, const ApplicationBundleInfo &info);

void changeQtFrameworks(const QString appPath, const QString &qtPath, bool useDebugLibs);
void changeQtFrameworks(const QList<FrameworkInfo> frameworks, const QString &appBinaryPath, const QString &qtPath);

FrameworkInfo parseOtoolLibraryLine(const QString &line, bool useDebugLibs);
QString findAppBinary(const QString &appBundlePath);
QList<FrameworkInfo> getQtFrameworks(const QString &path, bool useDebugLibs);
QList<FrameworkInfo> getQtFrameworks(const QStringList &otoolLines, bool useDebugLibs);
QString copyFramework(const FrameworkInfo &framework, const QString path);
DeploymentInfo deployQtFrameworks(const QString &appBundlePath, bool useDebugLibs);
DeploymentInfo deployQtFrameworks(QList<FrameworkInfo> frameworks, const QString &bundlePath, const QString &binaryPath);
void createQtConf(const QString &appBundlePath);
void deployPlugins(const QString &appBundlePath, DeploymentInfo deploymentInfo, bool useDebugLibs);
void changeIdentification(const QString &id, const QString &binaryPath);
void changeInstallName(const QString &oldName, const QString &newName, const QString &binaryPath);
QString findAppBinary(const QString &appBundlePath);

// /////////////////////////////////////////////////////////////////
// dtk specific deployment
// /////////////////////////////////////////////////////////////////

void deployDtkPlugins(const QString &appBundlePath, DeploymentInfo deploymentInfo, const QString& pluginsPath);

/* **************************************************************
 * Credits
 * ************************************************************** */

/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
****************************************************************************/

//
// dtkDeploy.h ends here
