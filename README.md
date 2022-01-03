# Orcon15RF
I wanted to control my Orcon MVS 15 (RHBP) ventilation system from my Raspberry Pi, but couldn't find a solution without adding additional switches or even modifying the unit itself. Since I prefer to control the unit using RF instead, I decided to tackle the problem myself.

## Hardware
The script uses an Wemos D1 mini with a CC1101 module. The D1 mini is controlled by sending/receiving commands over the serial connection to a Raspberry Pi. Of course, you could easily modify the code to use Wifi instead. For information on how to wire the CC1101 module see [here](https://github.com/LSatan/SmartRC-CC1101-Driver-Lib/blob/master/img/Esp8266_CC1101.png).

## Software
To control the CC1101 module, [SmartRC-CC1101-Driver-Lib](https://github.com/LSatan/SmartRC-CC1101-Driver-Lib) is used. A small change to the driver was required to make everything work, so be sure to use the version that's in this repository. The Arduino project needed to flash the Wemos D1 mini simply sends sequences of bytes modulated in a way that the Orcon unit can understand (see [here](https://github.com/egbertbouman/Orcon15RF/blob/main/Orcon15RF/Orcon15RF.ino#L15-L38) for the settings used to recreate the signal). Most of the logic, however, is located in a small Python script. This makes the code easy to understand and extend (with for instance an Orcon WTW unit or possibly even a completely different protocol).

## Protocol
I used an SDR-RTL USB stick to listen to the RF signals that controlled the Orcon unit. The RF protocol itself looks pretty simple. There are no counters and all commands can simply be replayed (except maybe join/leave, I haven't tried). The protocol seems to be Manchester encoded and has a basic checksum.

However, since I didn't want to spend too much time on this project right now, I decided to just clone my existing remote instead. This is arguably easier for a user than registering a new remote, since it's not necessary to cut power to the Orcon unit.

## Usage
Assuming your Wemos D1 mini is connected to serial port COM6, you can clone your Orcon 15RF remote as follows:

```python orcon.py --port COM6 --clone --file orcon.json```

During the cloning process, you'll need to press all buttons on your remote one at a time. Press the button once as soon as the script tells you to.

Next, you'll be able to control your Orcon unit as follows:

```python orcon.py --port COM6 --cmd 3 --file orcon.json```

Now the Orcon should start running at fan speed 3.

## Todo

* Improve the cloning process
* Optional support for WiringPi in order to control the CC1101 module directly from the Raspberry Pi's SPI interface and remove the need for the Wemos D1 mini
* Optional support for controlling the Wemos D1 mini using a wifi connection
* Fully reverse engineer the RF protocol and allow for operation without cloning an existing remote
