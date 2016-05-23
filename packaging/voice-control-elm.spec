Name:		voice-control-elm
Summary:	voice-control-elm library
Version:	0.2.1
Release:	1
Group:		Graphics & UI Framework/Voice Framework
License:	Apache-2.0
URL:		N/A
Source0:	%{name}-%{version}.tar.gz
Source1001: %{name}.manifest
Source1002: %{name}-devel.manifest
BuildRequires:	cmake
BuildRequires:	edje-tools
BuildRequires:	pkgconfig(appcore-efl)
BuildRequires:	pkgconfig(capi-base-common)
BuildRequires:  pkgconfig(capi-system-info)
BuildRequires:  pkgconfig(cynara-client)
BuildRequires:  pkgconfig(cynara-session)
BuildRequires:	pkgconfig(dlog)
BuildRequires:	pkgconfig(ecore)
BuildRequires:	pkgconfig(eina)
BuildRequires:	pkgconfig(edje)
BuildRequires:	pkgconfig(elementary)
BuildRequires:	pkgconfig(evas)
BuildRequires:	pkgconfig(libtzplatform-config)
BuildRequires:	pkgconfig(libxml-2.0)
BuildRequires:	pkgconfig(voice-control-widget)
BuildRequires:	gettext-tools

%description
voice-control-elm library that provides advanced Voice Driven Control over UI

%package devel
Summary:	voice-control-elm library development headers
Group:		Development/Libraries
Requires:	 %{name} = %{version}

%description devel
voice-control-elm library development headers

%prep
%setup -q
cp %{SOURCE1001} %{SOURCE1002} .

%build
export LDFLAGS+="-Wl,--rpath=%{_libdir} -Wl,--hash-style=both -Wl,--as-needed,-lgcov"
rm -rf objdir
mkdir objdir

%{?SRPOL_DEBUG:
(cd objdir && cmake .. -DVERSION=%{version} \
	-DCMAKE_INSTALL_PREFIX=%{_prefix} \
	-DLIBDIR=%{_libdir} \
	-DINCLUDEDIR=%{_includedir} \
	-DTZ_SYS_RO_SHARE=%TZ_SYS_RO_SHARE \
	-DTZ_SYS_BIN=%TZ_SYS_BIN \
	-DCMAKE_BUILD_TYPE=Debug \
	-DSRPOL_DEBUG="1")
}
%{!?SRPOL_DEBUG:
(cd objdir && cmake .. -DVERSION=%{version} \
	-DCMAKE_INSTALL_PREFIX=%{_prefix} \
	-DLIBDIR=%{_libdir} \
	-DINCLUDEDIR=%{_includedir} \
	-DTZ_SYS_RO_SHARE=%TZ_SYS_RO_SHARE \
	-DTZ_SYS_BIN=%TZ_SYS_BIN \
	-DCMAKE_BUILD_TYPE=Debug \
	-DSRPOL_DEBUG="0")
}


%install
rm -rf %{buildroot}
mkdir -p %{buildroot}%{TZ_SYS_RO_SHARE}/license
install LICENSE %{buildroot}%{TZ_SYS_RO_SHARE}/license/%{name}

(cd objdir &&
%make_install)

%clean
rm -rf %{buildroot}

%post

mkdir -p %{TZ_SYS_RO_SHARE}/voice

%files
%manifest %{name}.manifest
%attr(644,root,root)
%defattr(-,root,root,-)
%{_libdir}/*.so
%{_libdir}/*.so.*
%{_datadir}/*
%{TZ_SYS_RO_SHARE}/locale/*
%{TZ_SYS_RO_SHARE}/voice/vc-elm/1.0/voice-control-elm-config.xml

%files devel
%manifest %{name}-devel.manifest
%{_includedir}/voice_control_elm.h
%{_libdir}/pkgconfig/%{name}.pc
