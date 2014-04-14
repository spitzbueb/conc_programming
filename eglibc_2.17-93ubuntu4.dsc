-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA1

Format: 3.0 (quilt)
Source: eglibc
Binary: libc-bin, libc-dev-bin, glibc-doc, eglibc-source, locales, locales-all, nscd, multiarch-support, libc6, libc6-dev, libc6-dbg, libc6-prof, libc6-pic, libc6-udeb, libc6.1, libc6.1-dev, libc6.1-dbg, libc6.1-prof, libc6.1-pic, libc6.1-udeb, libc0.3, libc0.3-dev, libc0.3-dbg, libc0.3-prof, libc0.3-pic, libc0.3-udeb, libc0.1, libc0.1-dev, libc0.1-dbg, libc0.1-prof, libc0.1-pic, libc0.1-udeb, libc6-i386, libc6-dev-i386, libc6-sparc, libc6-dev-sparc, libc6-sparc64, libc6-dev-sparc64, libc6-s390, libc6-dev-s390, libc6-s390x, libc6-dev-s390x, libc6-amd64, libc6-dev-amd64, libc6-powerpc, libc6-dev-powerpc, libc6-ppc64, libc6-dev-ppc64, libc6-mipsn32, libc6-dev-mipsn32, libc6-mips64, libc6-dev-mips64, libc6-armhf, libc6-dev-armhf, libc6-armel, libc6-dev-armel, libc0.1-i386, libc0.1-dev-i386, libc6-x32, libc6-dev-x32, libc6-i686, libc6-xen, libc0.1-i686, libc0.3-i686, libc0.3-xen, libc6.1-alphaev67, libc6-loongson2f, libnss-dns-udeb, libnss-files-udeb
Architecture: any all
Version: 2.17-93ubuntu4
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Uploaders: Clint Adams <clint@debian.org>, Aurelien Jarno <aurel32@debian.org>, Adam Conrad <adconrad@0c3.net>
Homepage: http://www.eglibc.org
Standards-Version: 3.9.4
Vcs-Browser: http://svn.debian.org/wsvn/pkg-glibc/glibc-package/
Vcs-Svn: svn://svn.debian.org/pkg-glibc/glibc-package/
Build-Depends: gettext, dpkg-dev (>= 1.16.0), bzip2, xz-utils, file, quilt, autoconf, gawk, debhelper (>= 7.4.3), rdfind, symlinks, linux-libc-dev (>= 3.2.1) [linux-any], linux-libc-dev (>= 3.4) [amd64 i386 x32], libaudit-dev [linux-any], libcap-dev [linux-any], libselinux-dev [linux-any], mig (>= 1.3-2) [hurd-i386], hurd-dev (>= 20120520-2~) [hurd-i386], gnumach-dev (>= 2:1.3.99.dfsg.git20120923~) [hurd-i386], kfreebsd-kernel-headers [kfreebsd-any], binutils (>= 2.21), g++-4.8 [arm64], g++-4.7 (>= 4.7.2-11), g++-4.7-multilib [amd64 armhf armel i386 kfreebsd-amd64 mips mipsel powerpc ppc64 s390 s390x sparc x32]
Build-Depends-Indep: perl, po-debconf (>= 1.0)
Package-List: 
 eglibc-source deb devel optional
 glibc-doc deb doc optional
 libc-bin deb libs required
 libc-dev-bin deb libdevel optional
 libc0.1 deb libs required
 libc0.1-dbg deb debug extra
 libc0.1-dev deb libdevel optional
 libc0.1-dev-i386 deb libdevel optional
 libc0.1-i386 deb libs optional
 libc0.1-i686 deb libs extra
 libc0.1-pic deb libdevel optional
 libc0.1-prof deb libdevel extra
 libc0.1-udeb udeb debian-installer extra
 libc0.3 deb libs required
 libc0.3-dbg deb debug extra
 libc0.3-dev deb libdevel optional
 libc0.3-i686 deb libs extra
 libc0.3-pic deb libdevel optional
 libc0.3-prof deb libdevel extra
 libc0.3-udeb udeb debian-installer extra
 libc0.3-xen deb libs extra
 libc6 deb libs required
 libc6-amd64 deb libs optional
 libc6-armel deb libs optional
 libc6-armhf deb libs optional
 libc6-dbg deb debug extra
 libc6-dev deb libdevel optional
 libc6-dev-amd64 deb libdevel optional
 libc6-dev-armel deb libdevel optional
 libc6-dev-armhf deb libdevel optional
 libc6-dev-i386 deb libdevel optional
 libc6-dev-mips64 deb libdevel optional
 libc6-dev-mipsn32 deb libdevel optional
 libc6-dev-powerpc deb libdevel optional
 libc6-dev-ppc64 deb libdevel optional
 libc6-dev-s390 deb libdevel optional
 libc6-dev-s390x deb libdevel optional
 libc6-dev-sparc deb libdevel optional
 libc6-dev-sparc64 deb libdevel optional
 libc6-dev-x32 deb libdevel optional
 libc6-i386 deb libs optional
 libc6-i686 deb libs extra
 libc6-loongson2f deb libs extra
 libc6-mips64 deb libs optional
 libc6-mipsn32 deb libs optional
 libc6-pic deb libdevel optional
 libc6-powerpc deb libs optional
 libc6-ppc64 deb libs optional
 libc6-prof deb libdevel extra
 libc6-s390 deb libs optional
 libc6-s390x deb libs optional
 libc6-sparc deb libs optional
 libc6-sparc64 deb libs optional
 libc6-udeb udeb debian-installer extra
 libc6-x32 deb libs optional
 libc6-xen deb libs extra
 libc6.1 deb libs required
 libc6.1-alphaev67 deb libs extra
 libc6.1-dbg deb debug extra
 libc6.1-dev deb libdevel optional
 libc6.1-pic deb libdevel optional
 libc6.1-prof deb libdevel extra
 libc6.1-udeb udeb debian-installer extra
 libnss-dns-udeb udeb debian-installer extra
 libnss-files-udeb udeb debian-installer extra
 locales deb localization standard
 locales-all deb localization extra
 multiarch-support deb libs required
 nscd deb admin optional
Checksums-Sha1: 
 1ad4f2f403d987c96020feac1ccbe85f1da12f80 11655100 eglibc_2.17.orig.tar.xz
 99720295bc6b848026081dfdde83aad07d4a5455 1074444 eglibc_2.17-93ubuntu4.debian.tar.xz
Checksums-Sha256: 
 8901e71e395993034a786e47c9fcdac6c01f10fcf9dd4357c55093f3b778a63a 11655100 eglibc_2.17.orig.tar.xz
 9c7c35315c38486ae2a5fef673c992b1bc70ef803afada7846ce9fc0851d2fa8 1074444 eglibc_2.17-93ubuntu4.debian.tar.xz
Files: 
 9121f7d3ea2907ce2355cb8e184199ea 11655100 eglibc_2.17.orig.tar.xz
 f943ed974654a34a39849578316b19c9 1074444 eglibc_2.17-93ubuntu4.debian.tar.xz
Original-Maintainer: GNU Libc Maintainers <debian-glibc@lists.debian.org>
Testsuite: autopkgtest

-----BEGIN PGP SIGNATURE-----
Version: GnuPG v1.4.14 (GNU/Linux)

iEYEARECAAYFAlJYxe0ACgkQvjztR8bOoMnCCgCdEa4/b1nIx9tchNB5xeA6k/sj
A/8AoMoAOBvmymSu8bVQjibeBv8IprL/
=dzVe
-----END PGP SIGNATURE-----
