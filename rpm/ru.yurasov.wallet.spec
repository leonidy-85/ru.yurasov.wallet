# >> macros
# << macros


Name:       ru.yurasov.wallet
Summary:    Your Barcode Manager for discount card
Version:    1.1
Release:    12
Group:      Qt/Qt
License:    GPL
URL:        https://github.com/leonidy-85/wallet
Source0:    %{name}.%{version}.tar.bz2

Requires:   sailfishsilica-qt5 >= 0.10.9
Requires:   qr-filter-qml-plugin
BuildRequires:  libpng
BuildRequires:  desktop-file-utils
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(liblzma)
#BuildRequires:  bzip2-devel
BuildRequires:  desktop-file-utils

%description
Your Barcode Manager for discount card, using zint as backend for (bar)codes

%prep
%setup -q -n %{name}-%{version}

# >> setup
# << setup

%build
%qmake5
%make_build

%install
rm -rf %{buildroot}
%qmake5_install

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

