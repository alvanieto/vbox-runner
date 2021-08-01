/*
 *   Copyright (C) 2015 Alvaro Nieto Gil <alvaro.nieto@gmail.com>
 *   Copyright (C) 2009 Edward "Hades" Toroshchin <kde@hades.name>
 *   Copyright (C) 2008 Sun Microsystems, Inc.
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

#include <QDir>
#include <QtXml/QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QMutexLocker>
#include <QProcess>
#include <QIcon>
#include <QAction>
#include <QDebug>
#include <KSharedConfig>

#include <KLocalizedString>
#include <krun.h>

#include "vbox.h"
#include "VBoxConfigReader.h"

K_EXPORT_PLASMA_RUNNER_WITH_JSON(VBoxRunner, "vbox.json")

VBoxRunner::VBoxRunner(QObject *parent, const QVariantList &args)
        : Plasma::AbstractRunner(parent, args),
          rd(nullptr) {
    Q_UNUSED(args)

    rd = new VBoxConfigReader;
    setObjectName("VirtualBox Machines Runner");
    setSpeed(AbstractRunner::SlowSpeed);
}

void VBoxRunner::init() {
    // Custom config file to store the launch counts
    launchCountConfig = KSharedConfig::openConfig(QDir::homePath() + "/.config/krunnerplugins/vboxrunnerrc")->
            group("VBoxRunnerLaunchCounts");
    connect(this, &VBoxRunner::prepare, this, &VBoxRunner::prepareForMatchSession);

    auto headlessAction = new QAction(QIcon::fromTheme("vbox-runner/vrdp_16px"), i18n("Start Headless VM"));
    headlessAction->setData("headless");
    auto launchAction = addAction("vboxlaunch", QIcon::fromTheme("vbox-runner/state_running_16px"), i18n("Start VM"));
    launchAction->setData("launch");
    m_actions = {headlessAction, launchAction};
}

void VBoxRunner::prepareForMatchSession() {
    // Does not have to be thread save
    rd->updateAsNeccessary();
}

VBoxRunner::~VBoxRunner() {
    delete rd;
    rd = nullptr;
}

void VBoxRunner::match(Plasma::RunnerContext &context) {
    QString request = context.query();
    if (request.contains(overviewRegex))
        request = request.remove(overviewRegex);

    const int totalLaunches = launchCountConfig.readEntry("launches", 0);
    for (const VBoxMachine &m: *rd->list)
        if (m.name.contains(request, Qt::CaseInsensitive)) {
            Plasma::QueryMatch match(this);
            match.setType(request.compare(m.name, Qt::CaseInsensitive) ?
                          Plasma::QueryMatch::PossibleMatch : Plasma::QueryMatch::ExactMatch);
            match.setIcon(m.icon);
            match.setText(m.name);
#ifdef SHOW_RUNNING_STATE
            match.setSubtext( isRunning(m.name)? i18n( "VirtualBox virtual machine (running)" )
                                             : i18n( "VirtualBox virtual machine (stopped)" ) );
#endif
            match.setRelevance((double) launchCountConfig.readEntry(m.name).toInt() / totalLaunches);
            context.addMatch(match);
        }
}

void VBoxRunner::run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match) {
    Q_UNUSED(context)
    launchCountConfig.writeEntry("launches", launchCountConfig.readEntry("launches", 0) + 1);
    launchCountConfig.writeEntry(match.text(), launchCountConfig.readEntry(match.text(), 0) + 1);
    if (match.selectedAction() && match.selectedAction()->data() == "headless")
        KRun::runCommand(QString("VBoxHeadless -s \"%1\"").arg(match.text()), nullptr);
    else
        KRun::runCommand(QString("VBoxManage startvm \"%1\"").arg(match.text()), nullptr);
}

bool VBoxRunner::isRunning(const QString &name) {
    QProcess vbm;
    vbm.start("VBoxManage", QStringList() << "showvminfo" << "--machinereadable" << name);

    if (!vbm.waitForFinished(2000)) return false;

    const QByteArray info(vbm.readAllStandardOutput());
    for (const QByteArray &line: info.split('\n')) {
        const QList<QByteArray> data(line.split('"'));
        if (data[0] == "VMState=") return data[1] != "poweroff";
    }
    return false;
}

QList<QAction *> VBoxRunner::actionsForMatch(const Plasma::QueryMatch &match) {
    Q_UNUSED(match)

    return m_actions;
}

#include "vbox.moc"
