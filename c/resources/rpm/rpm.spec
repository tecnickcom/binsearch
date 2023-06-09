# SPEC file

%global c_vendor    %{_vendor}
%global gh_owner    %{_owner}
%global gh_cvspath  %{_cvspath}
%global gh_project  %{_project}

Name:      %{_package}
Version:   %{_version}
Release:   %{_release}%{?dist}
Summary:   Fast binary search for columnar data formats.

Group:     Development/Libraries
License:   %{_docpath}/LICENSE
URL:       https://%{gh_cvspath}/%{gh_project}

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-%(%{__id_u} -n)

Provides:  %{gh_project} = %{version}

%description
Provides C header-only files for:
binsearch, a fast binary search for columnar data formats.

%build
#(cd %{_current_directory} && make build)

%install
rm -rf $RPM_BUILD_ROOT
(cd %{_current_directory} && make install DESTDIR=$RPM_BUILD_ROOT)

#%clean
#rm -rf $RPM_BUILD_ROOT
#(cd %{_current_directory} && make clean)

%files
%attr(-,root,root) %{_libpath}
%attr(-,root,root) %{_docpath}
%docdir %{_docpath}

%changelog
* Fri Jun 09 2023 Tecnick.com <info@tecnick.com> 9.3.0-1
- First RPM package.
