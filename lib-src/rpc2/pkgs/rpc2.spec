Summary: RPC2 library
Name: %{name}
Version: %{version}
Release: 1
Source: ftp://ftp.coda.cs.cmu.edu/pub/coda/src/@PACKAGE@-@VERSION@.tar.gz
License: LGPL
BuildRoot: %{_builddir}/%{name}-%{version}/Root
Group: Development/Libraries
%description
The RPC2 library.

%package devel
Summary: RPC2 library development files
Group: Development/Libraries
%description devel
Headers and static libraries for developing programs using the RPC2 library.

%changelog
* Thu Oct 28 1999 Jan Harkes <jaharkes@cs.cmu.edu>
- First stab at packaging the rpc2 library.

%prep
%setup -n @PACKAGE@-@VERSION@

%build
./configure --prefix=/usr
make

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT
make prefix=$RPM_BUILD_ROOT/usr install
chmod 755 $RPM_BUILD_ROOT/usr/lib/libfail.so.@LINUX_VERSION@
chmod 755 $RPM_BUILD_ROOT/usr/lib/librpc2.so.@LINUX_VERSION@
chmod 755 $RPM_BUILD_ROOT/usr/lib/libse.so.@LINUX_VERSION@

%clean
rm -rf $RPM_BUILD_ROOT

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%files
%defattr(-,root,root)
/usr/bin/filcon
/usr/lib/libfail.so.@LINUX_VERSION@
/usr/lib/librpc2.so.@LINUX_VERSION@
/usr/lib/libse.so.@LINUX_VERSION@

%files devel
%defattr(-,root,root)
/usr/bin/rp2gen
/usr/lib/libfail.a
/usr/lib/libfail.la
/usr/lib/libfail.so
/usr/lib/librpc2.a
/usr/lib/librpc2.la
/usr/lib/librpc2.so
/usr/lib/libse.a
/usr/lib/libse.la
/usr/lib/libse.so
%dir /usr/include/rpc2
/usr/include/rpc2/errors.h
/usr/include/rpc2/errorsdefs.h
/usr/include/rpc2/fail.h
/usr/include/rpc2/fcon.h
/usr/include/rpc2/multi.h
/usr/include/rpc2/rpc2.h
/usr/include/rpc2/rpc2_addrinfo.h
/usr/include/rpc2/se.h
/usr/include/rpc2/sftp.h
/usr/include/rpc2/secure.h
