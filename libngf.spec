Name:    libngf
Version: 0.13
Release: 1
Summary: Non-graphic feedback C-based client library
Group:   System/Libraries
License: Nokia proprietary
#URL:
Source0: %{name}-%{version}.tar.gz

BuildRequires: pkgconfig(glib-2.0) >= 2.18.0
BuildRequires: pkgconfig(dbus-1) >= 1.0.2
BuildRequires: pkgconfig(dbus-glib-1)
BuildRequires: pkgconfig(check)
BuildRequires: doxygen

%description
This package contains the C-based client library for accessing
Non-graphic feedback services.

%package devel
Summary: Non-graphic feedback C-based development package
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}

%description devel  
%{summary}

%package doc
Summary: Non-graphic feedback client documentation
Group: Documentation

%description doc
This package contains the client library API documentation.

%package client
Summary: Non-graphic feedback test client
Group: System/Libraries
Requires: %{name} = %{version}-%{release}

%description client
Test client.

%prep
%setup -q

%build
%autogen
doxygen doc/doxygen.cfg
make %{?_smp_mflags}

%install
make DESTDIR=%{buildroot} install
install -d %{buildroot}/usr/share/doc/libngf-doc/html/
install -m 644 doc/html/* %{buildroot}/usr/share/doc/libngf-doc/html/
rm %{buildroot}/%{_libdir}/libngf0.a

%post -p /sbin/ldconfig  
   
%postun -p /sbin/ldconfig  

%files
%defattr(-,root,root,-)
%doc COPYING
%{_libdir}/libngf0-0.1.so.*

%files devel
%defattr(-,root,root,-)
%doc COPYING
%{_libdir}/libngf0.so
%{_includedir}/%{name}-1.0/%{name}/ngf.h
%{_includedir}/%{name}-1.0/%{name}/proplist.h
%{_includedir}/%{name}-1.0/%{name}/client.h
%{_libdir}/libngf0.la
%{_libdir}/pkgconfig/libngf0.pc

%files doc
%defattr(-,root,root,-)
%doc COPYING
%{_docdir}/%{name}-doc/html/*

%files client
%defattr(-,root,root,-)
%doc COPYING
%{_bindir}/ngf-client
