# >> macros
# << macros

%define __provides_exclude_from ^%{_datadir}/%{name}/lib/.*$
%define __requires_exclude ^libopencv.*$

Name:       ru.yurasov.wallet
Summary:    Your Barcode Manager for discount card
Version:    1.2
Release:    6
Group:      Qt/Qt
License:    MIT/BSD
URL:        https://github.com/leonidy-85/wallet
Source0:    %{name}.%{version}.tar.bz2

Requires:   sailfishsilica-qt5 >= 0.10.9
Requires:   qr-filter-qml-plugin
BuildRequires: libpng
BuildRequires: desktop-file-utils
BuildRequires: bzip2-devel
BuildRequires: pkgconfig(protobuf)
#BuildRequires: pkgconfig(liblzma)
BuildRequires: pkgconfig(nemonotifications-qt5)
BuildRequires: pkgconfig(Qt5Core)
BuildRequires: pkgconfig(Qt5Multimedia)
BuildRequires: pkgconfig(Qt5Qml)
BuildRequires: pkgconfig(Qt5Quick)
BuildRequires: pkgconfig(Qt5Concurrent)
BuildRequires: pkgconfig(Qt5Gui)
BuildRequires: pkgconfig(Qt5OpenGL)
BuildRequires: pkgconfig(Qt5DBus)
BuildRequires: cmake >= 2.6.3
BuildRequires:  desktop-file-utils

%description
Your Barcode Manager for discount card, using zint as backend for (bar)codes

%prep
%autosetup

%build
%qmake5 -r OPENCV_MFLAGS=%{?_smp_mflags}
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%make_install

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%defattr(644,root,root,-)
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png

%clean
rm -f translations/*.qm
rm -f *.o
rm -f moc_*
rm -f documentation.list
rm -f qrc_resources.*
rm -f Makefile

