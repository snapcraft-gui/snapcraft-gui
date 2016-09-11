#!/bin/bash

echo "Are we using the latest Qt version? $LATESTQT"
echo "What version of Ubuntu are we using? $DISTRO"

if [[ $LATESTQT == "yes" ]]; then
	echo "deb http://httpredir.debian.org/debian experimental main" >> /etc/apt/sources.list &&
	echo "deb http://httpredir.debian.org/debian unstable main" >> /etc/apt/sources.list &&
	apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 7638D0442B90D010 8B48AD6246925553 &&
	apt update &&
	apt download libicu57 qttranslations5-l10n libjpeg62-turbo mesa-utils qt5-gtk-platformtheme libcups2 libwebp6 qtchooser
	if [[ $DISTRO == "xenial" ]]; then
		apt download libgssapi-krb5-2
	fi
	sed -i "s/deb http:\/\/httpredir.debian.org\/debian unstable/#deb http:\/\/httpredir.debian.org\/debian unstable/" /etc/apt/sources.list &&
	apt update &&
	apt -t experimental -y install build-essential qt5-default qttools5-dev-tools qttools5-dev qtbase5-dev qtchooser libqt5clucene5 libqt5designer5 libqt5designercomponents5 libqt5help5 libqt5core5a libqt5dbus5 libqt5designer5 libqt5designercomponents5 libqt5gui5 libqt5help5 libqt5network5 libqt5printsupport5 libqt5quickwidgets5 libqt5sql5 libqt5webkit5 libqt5widgets5 libqt5xml5 qtbase-abi-5-7-0 libqt5sql5-sqlite ./*.deb &&
	cd snapcraft-qt &&
	qmake -r &&
	make -j2 &&
	make check &&
	qmake --version
else
	if [[ $LATESTQT == "no" ]]; then
		apt update &&
		apt -y install build-essential qt5-default qttools5-dev-tools qttools5-dev &&
		cd snapcraft-qt &&
		qmake -r &&
		make -j2 &&
		make check &&
		qmake --version
	else
		exit 1
	fi
fi
