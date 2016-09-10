#!/bin/bash

if [[ $LATESTQT == "yes" ]]; then
	echo "deb http://httpredir.debian.org/debian experimental main" >> /etc/apt/sources.list &&
	apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 7638D0442B90D010 8B48AD6246925553
	apt update &&
	apt -t experimental -y install build-essential qt5-default qttools5-dev-tools qttools5-dev qtbase5-dev qtchooser libqt5clucene5 libqt5designer5 libqt5designercomponents5 libqt5help5 libqt5core5a libqt5dbus5 libqt5designer5 libqt5designercomponents5 libqt5gui5 libqt5help5 libqt5network5 libqt5printsupport5 libqt5quickwidgets5 libqt5sql5 libqt5webkit5 libqt5widgets5 libqt5xml5 qtbase-abi-5-7-0 libqt5sql5-sqlite
else
	if [[ $LATESTQT == "no" ]]; then
		apt update &&
		apt -y install build-essential qt5-default qttools5-dev-tools qttools5-dev
	else
		exit 1
	fi
fi

cd snapcraft-qt &&
qmake -r &&
make -j2 &&
make check &&
qmake --version &&
