# Snapcraft GUI (development)

Graphical user interface to create snaps using snapcraft as backend, written in Qt.

# Build status and Coveralls status

![travis-ci](https://travis-ci.org/keshavbhatt/snapcraft-gui.svg?branch=master)
[![Coverage Status](https://coveralls.io/repos/github/keshavbhatt/snapcraft-gui/badge.svg?branch=master)](https://coveralls.io/github/keshavbhatt/snapcraft-gui?branch=master)

# How to install on Ubuntu 16.04 and Ubuntu Yakkety Yak (the development branch)

```
sudo add-apt-repository ppa:keshavnrj/snapcraft-gui-nightly

sudo apt update

sudo apt install snapcraft-gui

```

# IRC
If you are having trouble or would like to help out, join ##snapcraft-gui on freenode.

Kiwi IRC (IRC in your web browser) link: https://kiwiirc.com/client/irc.freenode.net/##snapcraft-gui

#Join the chat at gitter
[![Gitter chat](https://badges.gitter.im/gitterHQ/gitter.png)](https://gitter.im/Snapcraft-gui/Lobby)

# TODO list:

Snap package Manager (click package manager in file menu to see): [Started]
 - with local snap installation capabilities [Done]
 - allow remove installed snaps [Done]
 - allow obtain informatio about installed snaps by selecting them [Done]
 - allow check for snaps updates [Done]
 - allow update updatable snaps [target] //waiting for updatable packages in store

Snapcraft.yaml editor: [Started]
 - snapcraft's styled Yaml syntax highlightning [Done]
 - full editor functionalities [Done]
 - pastebin integration [Done]

Snap lifecycle commands: [Started]
 - clean [Done]
 - pull [Done]
 - build [Done]
 - stage [Done]
 - prime [Done]
 - snap [Done]

Snap Store: [Started]
 - search functionality [Done]
 - identify snap installed or not [Done]
 - install remove snaps from inside [target]

# Screenshots

![snapcraft-gui](https://github.com/keshavbhatt/snapcraft-gui/blob/master/screenshots/sc1.png?raw=true)
![snapcraft-gui](https://github.com/keshavbhatt/snapcraft-gui/blob/master/screenshots/sc2.png?raw=true)
![snapcraft-gui](https://github.com/keshavbhatt/snapcraft-gui/blob/master/screenshots/sc3.png?raw=true)

# How to build

To build the source package:

```
git clone https://github.com/keshavbhatt/snapcraft-gui.git
cd snapcraft-qt/
qmake
make
./snapcraft.gui
```

Make sure you installed dependencies listed in the dependencies file.
Few of them are here:

	tree // sudo apt install tree
	pastebinit // sudo apt install pastebinit 
	snapcraft //sudo apt install snapcraft 
