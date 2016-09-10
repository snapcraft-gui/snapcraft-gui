# Snapcraft-gui (development)
Graphical user interface to create snaps using snapcraft as backend , written in qt.

#Travis-CI build status for Ubuntu 16.04.1 LTS xenial

![travis-ci](https://travis-ci.org/keshavbhatt/snapcraft-gui.svg?branch=master)

#Install on Ubuntu 16.04 and above

	sudo add-apt-repository ppa:keshavnrj/snapcraft-gui-nightly

	sudo apt update
	
	sudo apt install snapcraft-gui

#Current task:
Snap package Manager (click package manager in file menu to see): [Started]

	-with local snap installation capabilities [Done]

	-allow remove installed snaps [Done]

	-allow obtain informatio about installed snaps by selecting them [Done]

	-allow check for snaps updates [Done]

	-allow update updatable snaps [target] //waiting for updatable packages in store

Snapcraft.yaml editor: [Started]
	
	-snapcraft's styled Yaml syntax highlightning [Done]
	
	-full editor functionalities [Done]
	
	-pastebin integration [Done]
	
Snap lifecycle commands: [Started]
	
	-clean [done]
	
	-pull[done]
	
	-build[done]
	
	-stage[done]
	
	-prime[done]
	
	-snap[done]

Snap Store: [Started]
	
	-search funtionality[Done]
	
	-identify snap installed or not[Done]
	
	-install remove snaps from inside[target]
	

#NOTE:
Am implementing/adding each feature one by one so, all the features will not work. see the commits log to check what will work and what will not.


#ScreenShot

![snapcraft-gui](https://github.com/keshavbhatt/snapcraft-gui/blob/master/screenshots/sc1.png?raw=true)
![snapcraft-gui](https://github.com/keshavbhatt/snapcraft-gui/blob/master/screenshots/sc2.png?raw=true)
![snapcraft-gui](https://github.com/keshavbhatt/snapcraft-gui/blob/master/screenshots/sc3.png?raw=true)

#Build
	
	Built on Ubuntu 16.04 using Qt version 5.5.1 in /usr/lib/x86_64-linux-gnu
	
	git clone https://github.com/keshavbhatt/snapcraft-gui.git
	
	cd snapcraft-qt/
	 
	qmake
	
	make
	
	./snapcraft.gui
	
If Compiled with no errors, type ./snapcraft-qt to run
Make sure you installed dependencies listed in dependencies file. 

#dependencies

	See dependencies file in the source tree above

