%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}

Name:       sailfishos-browser-searchengines
Summary:    Manipulate Sailfish Browser search engines
Version:    1.0.6
Release:    1
Group:      Qt/Qt
License:    WTFPL
URL:        https://github.com/CODeRUS/sailfishos-browser-searchengines
Source0:    %{name}-%{version}.tar.bz2
Requires:   sailfishsilica-qt5 >= 0.10.9
BuildRequires:  pkgconfig(sailfishapp) >= 1.0.2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(Qt5Network)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(systemd)
BuildRequires:  desktop-file-utils
BuildRequires:  sailfish-svg2png

%description
Short description of my Sailfish OS Application

%package ts-devel
Summary:    Manipulate Sailfish Browser search engines
Group:      Qt/Qt
BuildArch:  noarch
Requires:   %{name}

%description ts-devel
%{summary}.

%prep
%setup -q -n %{name}-%{version}

%build

%qtc_qmake5 \
    SPEC_UNITDIR=%{_unitdir}

%qtc_make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%qmake5_install

desktop-file-install --delete-original       \
  --dir %{buildroot}%{_datadir}/applications             \
   %{buildroot}%{_datadir}/applications/*.desktop

%post
dbus-send --system --type=method_call --dest=org.freedesktop.DBus / org.freedesktop.DBus.ReloadConfig

%files
%defattr(-,root,root,-)
%{_bindir}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png

%{_sysconfdir}/dbus-1/system.d/org.coderus.searchengines.conf
%{_datadir}/dbus-1/system-services/org.coderus.searchengines.service
%{_unitdir}/org.coderus.searchengines.service

%files ts-devel
%defattr(-,root,root,-)
%{_datadir}/translations/%{name}.ts
