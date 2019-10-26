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

#include <KLocalizedString>
#include <krun.h>

#include "vbox.h"

struct VBoxMachine
{
    QString name;
    QIcon   icon;
};

class VBoxConfigReader
{
    QList< VBoxMachine > *list;
    QDateTime lastChecked;
    QMutex mutex;

    QString vboxdir;
    QHash< QString, QIcon > mOsTypeIcons;
public:
    VBoxConfigReader()
     : list( nullptr)
    {
        vboxdir = QDir::homePath() + "/.VirtualBox/";
        if ( ! QDir(vboxdir).exists() )
            vboxdir = QDir::homePath() + "/.config/VirtualBox/";

        /* Fill in OS type icon dictionary */
        static const char *kOSTypeIcons [][2] =
        {
            {"Other",           "vbox-runner/os_other"},
            {"DOS",             "vbox-runner/os_dos"},
            {"Netware",         "vbox-runner/os_netware"},
            {"L4",              "vbox-runner/os_l4"},
            {"Windows31",       "vbox-runner/os_win31"},
            {"Windows95",       "vbox-runner/os_win95"},
            {"Windows98",       "vbox-runner/os_win98"},
            {"WindowsMe",       "vbox-runner/os_winme"},
            {"WindowsNT4",      "vbox-runner/os_winnt4"},
            {"Windows2000",     "vbox-runner/os_win2k"},
            {"WindowsXP",       "vbox-runner/os_winxp"},
            {"WindowsXP_64",    "vbox-runner/os_winxp_64"},
            {"Windows2003",     "vbox-runner/os_win2k3"},
            {"Windows2003_64",  "vbox-runner/os_win2k3_64"},
            {"WindowsVista",    "vbox-runner/os_winvista"},
            {"WindowsVista_64", "vbox-runner/os_winvista_64"},
            {"Windows2008",     "vbox-runner/os_win2k8"},
            {"Windows2008_64",  "vbox-runner/os_win2k8_64"},
            {"Windows7",        "vbox-runner/os_win7"},
            {"Windows7_64",     "vbox-runner/os_win7_64"},
            {"WindowsNT",       "vbox-runner/os_win_other"},
            {"OS2Warp3",        "vbox-runner/os_os2warp3"},
            {"OS2Warp4",        "vbox-runner/os_os2warp4"},
            {"OS2Warp45",       "vbox-runner/os_os2warp45"},
            {"OS2eCS",          "vbox-runner/os_os2ecs"},
            {"OS2",             "vbox-runner/os_os2_other"},
            {"Linux22",         "vbox-runner/os_linux22"},
            {"Linux24",         "vbox-runner/os_linux24"},
            {"Linux24_64",      "vbox-runner/os_linux24_64"},
            {"Linux26",         "vbox-runner/os_linux26"},
            {"Linux26_64",      "vbox-runner/os_linux26_64"},
            {"ArchLinux",       "vbox-runner/os_archlinux"},
            {"ArchLinux_64",    "vbox-runner/os_archlinux_64"},
            {"Debian",          "vbox-runner/os_debian"},
            {"Debian_64",       "vbox-runner/os_debian_64"},
            {"OpenSUSE",        "vbox-runner/os_opensuse"},
            {"OpenSUSE_64",     "vbox-runner/os_opensuse_64"},
            {"Fedora",          "vbox-runner/os_fedora"},
            {"Fedora_64",       "vbox-runner/os_fedora_64"},
            {"Gentoo",          "vbox-runner/os_gentoo"},
            {"Gentoo_64",       "vbox-runner/os_gentoo_64"},
            {"Mandriva",        "vbox-runner/os_mandriva"},
            {"Mandriva_64",     "vbox-runner/os_mandriva_64"},
            {"RedHat",          "vbox-runner/os_redhat"},
            {"RedHat_64",       "vbox-runner/os_redhat_64"},
            {"Ubuntu",          "vbox-runner/os_ubuntu"},
            {"Ubuntu_64",       "vbox-runner/os_ubuntu_64"},
            {"Xandros",         "vbox-runner/os_xandros"},
            {"Xandros_64",      "vbox-runner/os_xandros_64"},
            {"Linux",           "vbox-runner/os_linux_other"},
            {"FreeBSD",         "vbox-runner/os_freebsd"},
            {"FreeBSD_64",      "vbox-runner/os_freebsd_64"},
            {"OpenBSD",         "vbox-runner/os_openbsd"},
            {"OpenBSD_64",      "vbox-runner/os_openbsd_64"},
            {"NetBSD",          "vbox-runner/os_netbsd"},
            {"NetBSD_64",       "vbox-runner/os_netbsd_64"},
            {"Solaris",         "vbox-runner/os_solaris"},
            {"Solaris_64",      "vbox-runner/os_solaris_64"},
            {"OpenSolaris",     "vbox-runner/os_opensolaris"},
            {"OpenSolaris_64",  "vbox-runner/os_opensolaris_64"},
            {"QNX",             "vbox-runner/os_qnx"},
        };
        for (auto & kOSTypeIcon : kOSTypeIcons)
        {
            mOsTypeIcons.insert (kOSTypeIcon[0],
                QIcon::fromTheme (kOSTypeIcon[1]));
        }
    }

    ~VBoxConfigReader()
    {
        delete list;
        list = nullptr;
    }

    void updateAsNeccessary()
    {
        QMutexLocker _ml( &mutex );
        QDir dir( vboxdir );
        if( !list || lastChecked < QFileInfo( dir.filePath( "VirtualBox.xml" ) ).lastModified() )
        {
            delete list;
            list = new QList< VBoxMachine >;

            QFile f( dir.filePath( "VirtualBox.xml" ) );

            QDomDocument d;
            if( !d.setContent( &f ) )
            {
                qWarning() << "Could not parse file " << f.fileName();
                return;
            }

            QDomNodeList _dnlist = d.elementsByTagName( "MachineEntry" );
            const int _dnlistCount = _dnlist.count();
            for( int i = 0; i < _dnlistCount; ++i )
            {
                QDomNode node = _dnlist.at( i );
                QString ref = node.toElement().attribute( "src" );
                if( ref.isEmpty() )
                {
                    qWarning() << "MachineEntry with no src attribute";
                    continue;
                }

                QFile m( dir.filePath( ref ) );

                QDomDocument mspec;
                if( !mspec.setContent( &m ) )
                {
                    qWarning() << "Could not parse machine file " << m.fileName();
                    continue;
                }

                QDomNodeList _mlist = mspec.elementsByTagName( "Machine" );
                const int _mlistCount =_mlist.count();
                for( int j = 0; j < _mlistCount; ++j )
                {
                    QDomNode mnode = _mlist.at( j );
                    VBoxMachine machine;

                    machine.name = mnode.toElement().attribute( "name" );

                    QString type = mnode.toElement().attribute( "OSType" );
                    machine.icon = mOsTypeIcons.contains( type )?
                        mOsTypeIcons[ type ] : QIcon::fromTheme( "virtualbox-ose" );

                    (*list) << machine;
                }
            }

            lastChecked = QDateTime::currentDateTime();
        }
    }

    QList< VBoxMachine > machines()
    {
        updateAsNeccessary();
        if( !list )
            return QList< VBoxMachine >();

        return *list;
    }
};

VBoxRunner::VBoxRunner( QObject *parent, const QVariantList& args )
    : Plasma::AbstractRunner( parent, args ),
      rd( nullptr )
{
    Q_UNUSED(args)

    rd = new VBoxConfigReader;
    setObjectName( "VirtualBox Machines Runner" );
    setSpeed( AbstractRunner::SlowSpeed );
}

VBoxRunner::~VBoxRunner()
{
    delete rd;
    rd = nullptr;
}

void VBoxRunner::match(Plasma::RunnerContext &context)
{
    QString request = context.query();
    if( request.startsWith( "vm ", Qt::CaseInsensitive ) )
        request.remove(0, 3);
    if( request.isEmpty() )
        return;

    QList< Plasma::QueryMatch > matches;
    for( const VBoxMachine& m: rd->machines() )
        if( m.name.contains( request, Qt::CaseInsensitive ) )
        {
            Plasma::QueryMatch match( this );
            match.setType( request.compare(m.name, Qt::CaseInsensitive)?
                            Plasma::QueryMatch::PossibleMatch
                            : Plasma::QueryMatch::ExactMatch );
            match.setIcon( m.icon );
            match.setText( m.name );
            match.setSubtext( isRunning(m.name)? i18n( "VirtualBox virtual machine (running)" )
                                               : i18n( "VirtualBox virtual machine (stopped)" ) );
            matches << match;
        }

    context.addMatches(matches);
}

void VBoxRunner::run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match)
{
    Q_UNUSED(context)

    if(match.selectedAction() && match.selectedAction()->data() == "headless")
        KRun::runCommand( QString( "VBoxHeadless -s \"%1\"" ).arg( match.text() ), nullptr );
    else
        KRun::runCommand( QString( "VBoxManage startvm \"%1\"" ).arg( match.text() ), nullptr );
}

bool VBoxRunner::isRunning(const QString& name)
{
    QProcess vbm;
    vbm.start( "VBoxManage", QStringList() << "showvminfo" << "--machinereadable" << name );

    if(!vbm.waitForFinished(2000)) return false;

    QByteArray info(vbm.readAllStandardOutput());
    for(const QByteArray &line: info.split('\n'))
    {
        QList<QByteArray> data(line.split('"'));
        if(data[0] == "VMState=")
            return data[1] != "poweroff";
    }
    return false;
}

QList<QAction*> VBoxRunner::actionsForMatch(const Plasma::QueryMatch &match)
{
    Q_UNUSED(match)

    QList<QAction*> ret;

    if(!action("vboxlaunch"))
    {
        (addAction("vboxlaunch", QIcon::fromTheme("vbox-runner/state_running_16px"), i18n("Start VM")))->setData("launch");
        (addAction("vboxheadless", QIcon::fromTheme("vbox-runner/vrdp_16px"), i18n("Start Headless VM")))->setData("headless");
    }
    ret << action("vboxlaunch") << action("vboxheadless");
    return ret;
}

#include "moc_vbox.cpp"
