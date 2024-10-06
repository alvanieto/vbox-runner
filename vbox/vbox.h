/*
 *   Copyright (C) 2009 Edward "Hades" Toroshchin <kde@hades.name>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef VBOXRUNNER_H
#define VBOXRUNNER_H

#include "VBoxConfigReader.h"
#include <KConfigGroup>
#include <KRunner/AbstractRunner>
#include <krunner_version.h>

#if KRUNNER_VERSION_MAJOR == 6
#include <KRunner/Action>
#else
#include <QAction>
#endif

class QAction;

class VBoxRunner : public KRunner::AbstractRunner
{
    Q_OBJECT

public:
    VBoxRunner(QObject *parent, const KPluginMetaData &data, const QVariantList &args);

    void match(KRunner::RunnerContext &context) override;
    void run(const KRunner::RunnerContext &context, const KRunner::QueryMatch &match) override;
    void init() override;

private:
    bool isRunning(const QString &name);

    VBoxConfigReader rd;
    KConfigGroup launchCountConfig;
#if KRUNNER_VERSION_MAJOR == 5
    QList<QAction *> m_actions;
#else
    KRunner::Actions m_actions;
#endif

    const QRegularExpression overviewRegex{QStringLiteral("^vm ?")};

    void prepareForMatchSession();
};

#endif
