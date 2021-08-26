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

#include <KRunner/AbstractRunner>

class VBoxConfigReader;

class VBoxRunner : public Plasma::AbstractRunner {

Q_OBJECT

public:
    VBoxRunner(QObject *parent, const QVariantList &args);

    ~VBoxRunner() override;

    void match(Plasma::RunnerContext &context) override;

    void run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match) override;

    QList<QAction *> actionsForMatch(const Plasma::QueryMatch &match) override;

private:
    bool isRunning(const QString &name);

    VBoxConfigReader *rd;
    KConfigGroup launchCountConfig;
    QList<QAction *> m_actions;
    const QRegExp overviewRegex = QRegExp("^vm ?", Qt::CaseInsensitive);

protected Q_SLOTS:

    void init() override;

    void prepareForMatchSession();
};

#endif
