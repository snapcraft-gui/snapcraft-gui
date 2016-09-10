#!/bin/bash

if [[ $LATESTQT == "yes" ]]; then
	echo "deb-src http://httpredir.debian.org/debian experimental main" >> /etc/apt/sources.list &&
	apt update &&
	apt -t experimental -y install build-essential qt5-default qttools5-dev-tools qttools5-dev
else
	if [[ $LATESTQT == "no" ]]; then
		apt update &&
		apt -y install build-essential qt5-default qttools5-dev-tools qttools5-dev &&
	else
		exit 1
	fi
fi

cd snapcraft-qt &&
qmake -r &&
make -j2 &&
make check &&
qmake --version &&
