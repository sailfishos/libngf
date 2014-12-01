Name:       libngf

Summary:    Non-graphic feedback C-based client library
Version:    0.23
Release:    1
Group:      System/Libraries
License:    LGPL 2.1
URL:        https://github.com/nemomobile/libngf
Source0:    %{name}-%{version}.tar.gz
Source100:  libngf.yaml
Requires:   ngfd
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  pkgconfig(glib-2.0) >= 2.18.0
BuildRequires:  pkgconfig(dbus-1) >= 1.0.2
BuildRequires:  pkgconfig(dbus-glib-1)
BuildRequires:  pkgconfig(check)
BuildRequires:  doxygen

%description
This package contains the C-based client library for accessing
Non-graphic feedback services.


%package doc
Summary:    Non-graphic feedback client documentation
Group:      Documentation
Requires:   %{name} = %{version}-%{release}

%description doc
This package contains the client library API documentation.

%package client
Summary:    Non-graphic feedback test client
Group:      System/Libraries
Requires:   %{name} = %{version}-%{release}

%description client
Test client.

%package devel
Summary:    Non-graphic feedback C-based development package
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
%{summary}.

%prep
%setup -q -n %{name}-%{version}


%build
%autogen
doxygen doc/doxygen.cfg
make %{?jobs:-j%jobs}


%install
rm -rf %{buildroot}
%make_install

install -d %{buildroot}/usr/share/doc/libngf-doc/html/
install -m 644 doc/html/* %{buildroot}/usr/share/doc/libngf-doc/html/

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%doc COPYING
%{_libdir}/libngf0-*.so.*

%files doc
%defattr(-,root,root,-)
%doc COPYING
%{_docdir}/%{name}-doc/html/*

%files client
%defattr(-,root,root,-)
%doc COPYING
%{_bindir}/ngf-client

%files devel
%defattr(-,root,root,-)
%doc COPYING
%{_libdir}/libngf0.so
%{_includedir}/%{name}-1.0/%{name}/ngf.h
%{_includedir}/%{name}-1.0/%{name}/proplist.h
%{_includedir}/%{name}-1.0/%{name}/client.h
%{_libdir}/pkgconfig/libngf0.pc
