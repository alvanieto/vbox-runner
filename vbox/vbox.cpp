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

#include <KIO/CommandLauncherJob>
#include <KSharedConfig>
#include <QAction>
#include <QDebug>
#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QIcon>
#include <QMutexLocker>
#include <QProcess>
#include <krunner_version.h>

#include <KLocalizedString>

#include "VBoxConfigReader.h"
#include "vbox.h"

K_PLUGIN_CLASS_WITH_JSON(VBoxRunner, "vbox.json")

VBoxRunner::VBoxRunner(QObject *parent, const KPluginMetaData &data, const QVariantList &args)
#if KRUNNER_VERSION_MAJOR == 6
    : KRunner::AbstractRunner(parent, data)
#else
    : KRunner::AbstractRunner(parent, data, args)
#endif
{
#if KRUNNER_VERSION_MAJOR == 6
    Q_UNUSED(args)
#endif
}

void VBoxRunner::init()
{
    // Custom config file to store the launch counts
    launchCountConfig = KSharedConfig::openConfig(QDir::homePath() + "/.config/krunnerplugins/vboxrunnerrc")->group("VBoxRunnerLaunchCounts");
    connect(this, &VBoxRunner::prepare, this, [this]() {
        rd.updateAsNeccessary();
    });

#if KRUNNER_VERSION_MAJOR == 6
    KRunner::Action headlessAction("headless", "vbox-runner/vrdp_16px", i18n("Start Headless VM"));
    KRunner::Action launchAction("vboxlaunch", "vbox-runner/state_running_16px", i18n("Start VM"));
    m_actions = {headlessAction, launchAction};
#else
    auto headlessAction = new QAction(QIcon::fromTheme("vbox-runner/vrdp_16px"), i18n("Start Headless VM"));
    headlessAction->setData("headless");
    auto launchAction = new QAction(QIcon::fromTheme("vbox-runner/state_running_16px"), i18n("Start VM"));
    launchAction->setData("launch");
    m_actions = {headlessAction, launchAction};
#endif
}

void VBoxRunner::match(KRunner::RunnerContext &context)
{
    QString request = context.query();
    if (request.contains(overviewRegex))
        request = request.remove(overviewRegex);

    const int totalLaunches = launchCountConfig.readEntry("launches", 0);
    for (const VBoxMachine &m : rd.list)
        if (m.name.contains(request, Qt::CaseInsensitive)) {
            KRunner::QueryMatch match(this);
#if KRUNNER_VERSION_MAJOR == 6
            match.setCategoryRelevance(request.compare(m.name, Qt::CaseInsensitive) //
                                           ? KRunner::QueryMatch::CategoryRelevance::Highest
                                           : KRunner::QueryMatch::CategoryRelevance::High);
#else
            match.setType(request.compare(m.name, Qt::CaseInsensitive) ? KRunner::QueryMatch::PossibleMatch : KRunner::QueryMatch::ExactMatch);
#endif
            match.setIcon(m.icon);
            match.setText(m.name);
#ifdef SHOW_RUNNING_STATE
            match.setSubtext(isRunning(m.name) ? i18n("VirtualBox virtual machine (running)") : i18n("VirtualBox virtual machine (stopped)"));
#endif
            match.setRelevance((double)launchCountConfig.readEntry(m.name).toInt() / totalLaunches);
            context.addMatch(match);
        }
}

void VBoxRunner::run(const KRunner::RunnerContext &context, const KRunner::QueryMatch &match)
{
    Q_UNUSED(context)
    launchCountConfig.writeEntry("launches", launchCountConfig.readEntry("launches", 0) + 1);
    launchCountConfig.writeEntry(match.text(), launchCountConfig.readEntry(match.text(), 0) + 1);
    QString command;
#if KRUNNER_VERSION_MAJOR == 6
    if (match.selectedAction() && match.selectedAction().id() == "headless")
#else
    if (match.selectedAction() && match.selectedAction()->data() == "headless")
#endif
        command = QString("VBoxHeadless -s \"%1\"").arg(match.text());
    else {
        command = QString("VBoxManage startvm \"%1\"").arg(match.text());
    }
    KIO::CommandLauncherJob *job = new KIO::CommandLauncherJob(command, this);
    job->start();
}

bool VBoxRunner::isRunning(const QString &name)
{
    QProcess vbm;
    vbm.start("VBoxManage", QStringList() << "showvminfo" << "--machinereadable" << name);

    if (!vbm.waitForFinished(2000))
        return false;

    const QByteArray info(vbm.readAllStandardOutput());
    for (const QByteArray &line : info.split('\n')) {
        const QList<QByteArray> data(line.split('"'));
        if (data[0] == "VMState=")
            return data[1] != "poweroff";
    }
    return false;
}

#include "moc_vbox.cpp"
#include "vbox.moc"
