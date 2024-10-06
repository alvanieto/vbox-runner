#ifndef VBOXRUNNERS_VBOXCONFIGREADER_H
#define VBOXRUNNERS_VBOXCONFIGREADER_H

#include <QDateTime>
#include <QDir>
#include <QDomDocument>
#include <QIcon>
#include <QString>

struct VBoxMachine {
    QString name;
    QIcon icon;
};

class VBoxConfigReader
{
    QDateTime lastChecked;

    QString vboxdir;
    QHash<QString, QIcon> mOsTypeIcons;

public:
    QList<VBoxMachine> list;
    VBoxConfigReader()
    {
        vboxdir = QDir::homePath() + "/.VirtualBox/";
        if (!QDir(vboxdir).exists())
            vboxdir = QDir::homePath() + "/.config/VirtualBox/";

        /* Fill in OS type icon dictionary */
        // region icons
        // clang-format off
        static const char *kOSTypeIcons[][2] =
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
        // clang-format on
        // endregion icons
        for (const auto &kOSTypeIcon : kOSTypeIcons) {
            mOsTypeIcons.insert(kOSTypeIcon[0], QIcon::fromTheme(kOSTypeIcon[1]));
        }
    }

    ~VBoxConfigReader()
    {
    }

    void updateAsNeccessary()
    {
        QDir dir(vboxdir);
        if (lastChecked < QFileInfo(dir.filePath("VirtualBox.xml")).lastModified()) {
            list.clear();

            QFile f(dir.filePath("VirtualBox.xml"));

            QDomDocument d;
            if (!d.setContent(&f)) {
                qWarning() << "Could not parse file " << f.fileName();
                return;
            }

            QDomNodeList _dnlist = d.elementsByTagName("MachineEntry");
            const int _dnlistCount = _dnlist.count();
            for (int i = 0; i < _dnlistCount; ++i) {
                const QDomNode node = _dnlist.at(i);
                const QString ref = node.toElement().attribute("src");
                if (ref.isEmpty()) {
                    qWarning() << "MachineEntry with no src attribute";
                    continue;
                }

                QFile m(dir.filePath(ref));

                QDomDocument mspec;
                if (!mspec.setContent(&m)) {
                    qWarning() << "Could not parse machine file " << m.fileName();
                    continue;
                }

                const QDomNodeList _mlist = mspec.elementsByTagName("Machine");
                const int _mlistCount = _mlist.count();
                for (int j = 0; j < _mlistCount; ++j) {
                    const QDomNode mnode = _mlist.at(j);
                    VBoxMachine machine;
                    machine.name = mnode.toElement().attribute("name");
                    const QString type = mnode.toElement().attribute("OSType");
                    machine.icon = mOsTypeIcons.contains(type) ? mOsTypeIcons[type] : QIcon::fromTheme("virtualbox-ose");
                    list.append(machine);
                }
            }

            lastChecked = QDateTime::currentDateTime();
        }
    }
};

#endif // VBOXRUNNERS_VBOXCONFIGREADER_H
