# Snapcraft GUI (development)

Graphical user interface to create snaps using snapcraft as backend, written in Qt.

# Build status and Coveralls status

[![Build Status](https://travis-ci.org/snapcraft-gui/snapcraft-gui.svg?branch=master)](https://travis-ci.org/snapcraft-gui/snapcraft-gui)
[![Coverage Status](https://coveralls.io/repos/github/keshavbhatt/snapcraft-gui/badge.svg?branch=master)](https://coveralls.io/github/keshavbhatt/snapcraft-gui?branch=master)

#Install using debian Package

Download the latest release from here -
[Download link](https://github.com/snapcraft-gui/snapcraft-gui/releases)

# How to install on Ubuntu 16.04 and Ubuntu Yakkety Yak (the development branch)

```
We updating new reository here soon //old one was removed due to some reasons
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
![snapcraft-gui](https://github.com/keshavbhatt/snapcraft-gui/blob/master/screenshots/sc4.png?raw=true)

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
	
###Donation

I have recieved emails from lots of people to put a donation via bitcoin or paypal, which simply implies that they are willing to make donation to project so I decided to put a paypal button , am not able to get Bitcoin integrated here for some reason will look for it in future , so right now you can use Paypal if you wana make any doantion and wana support this project 

[![Snapcraft Donation](http://i.imgur.com/Vfl5DN7.png?1)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=V6M9WXWVH2RW4)
	
### Contributing

When submitting code to this project, please indicate that you certify you are able to contribute the code by adding a signed-off-by line at the end of your commit message (using your real name) such as:

Signed-off-by: Random J Developer <random@developer.example.org>

This indicates that your contribution abides to the following rules:

```
Developer Certificate of Origin
Version 1.1

Copyright (C) 2004, 2006 The Linux Foundation and its contributors.
660 York Street, Suite 102,
San Francisco, CA 94110 USA

Everyone is permitted to copy and distribute verbatim copies of this
license document, but changing it is not allowed.


Developer's Certificate of Origin 1.1

By making a contribution to this project, I certify that:

(a) The contribution was created in whole or in part by me and I
    have the right to submit it under the open source license
    indicated in the file; or

(b) The contribution is based upon previous work that, to the best
    of my knowledge, is covered under an appropriate open source
    license and I have the right under that license to submit that
    work with modifications, whether created in whole or in part
    by me, under the same open source license (unless I am
    permitted to submit under a different license), as indicated
    in the file; or

(c) The contribution was provided directly to me by some other
    person who certified (a), (b) or (c) and I have not modified
    it.

(d) I understand and agree that this project and the contribution
    are public and that a record of the contribution (including all
    personal information I submit with it, including my sign-off) is
    maintained indefinitely and may be redistributed consistent with
    this project or the open source license(s) involved.
```
