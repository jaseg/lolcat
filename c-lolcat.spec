# c-lolcat.spec

Name:           c-lolcat
Version:        1.5
Release:        1%{?dist}
Summary:        LOLcat utility for system information display
License:        MIT
URL:            https://github.com/jaseg/lolcat
Source0:        https://github.com/jaseg/lolcat/archive/refs/tags/v1.5.tar.gz

BuildRequires:  gcc, make
Requires:       bash, coreutils

%description
LOLcat is a humorous system information utility that displays system info with ASCII art.

%prep
%autosetup -n lolcat-1.5

%build
make CFLAGS="%{optflags} -g" LDFLAGS="%{optflags}" 

%install
install -D -m 0755 lolcat %{buildroot}/usr/local/bin/lolcat
install -D -m 0755 censor %{buildroot}/usr/local/bin/censor

%files
/usr/local/bin/lolcat
/usr/local/bin/censor

%changelog
* Thu Dec 21 2024 Your Name <your.email@example.com> - 1.5-1
- Initial RPM release for version 1.5
