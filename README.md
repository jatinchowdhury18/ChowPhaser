# CHOW Phaser

[![Build Status](https://travis-ci.com/jatinchowdhury18/ChowPhaser.svg?token=Ub9niJrqG1Br1qaaxp7E&branch=master)](https://travis-ci.com/jatinchowdhury18/ChowPhaser)

CHOW Phaser is an open-source phaser effect,
based *very* loosely on the classic Shulte Compact
Phasing "A".

<img src="https://github.com/jatinchowdhury18/ChowPhaser/blob/master/res/screenshot_stereo.PNG?raw=true" alt="Pic" height="350">


## Installation

To install, simply download the
[latest release](https://github.com/jatinchowdhury18/ChowPhaser/releases).
If you would like to try the most recent updates
(potentially unstable), see the
[`bin/`](./bin) folder.

## Usage

CHOW Phaser has both a stereo and mono version.

CHOW Phaser Mono is designed to accept a mono signal
as input and generate a stereo signal as output. Any
stereo input to the plugin will first be summed
to mono before applying any processing.

CHOW Phaser Stereo accepts a stereo signal, and processes
each channel entirely separately. To link the controls
between the left and right channels, hold the SHIFT key
while moving one of the knobs.

The phaser's processing is broken up into two stages:
feedback and modulation. The amount of processing done
by each stage can be controlled separately with the
"feedback" and "modulation" knobs. Both stages share the
same  LFO, which is controlled by the "depth" and "frequency" 
parameters, as well as the "skew" knob. The "10x" parameter
simply multiplies the LFO frequency by 10. The "stages" 
parameter controls filter stages are used in the modulation
section, while "drive", "dirt", and "thrash" add various
styles of nonlinear processing to the feedback stage.

## Building

To build from scratch, you must have CMake installed.

```bash

# Clone the repository
$ git clone https://github.com/jatinchowdhury18/CHOWPhaser.git
$ cd CHOWPhaser

# initialize and set up submodules
$ git submodule update --init --recursive

# build with CMake
$ cmake -Bbuild
$ cmake --build build --config Release
```

## License

CHOWPhaser is open source, and is licensed under the BSD 3-clause license.
Enjoy!
