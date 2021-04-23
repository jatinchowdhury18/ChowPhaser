# CHOW Phaser

[![Download Latest](https://img.shields.io/badge/download-latest-yellow.svg)](https://github.com/jatinchowdhury18/ChowPhaser/releases/latest)
[![CI](https://github.com/jatinchowdhury18/ChowPhaser/actions/workflows/cmake.yml/badge.svg)](https://github.com/jatinchowdhury18/ChowPhaser/actions/workflows/cmake.yml)
[![License](https://img.shields.io/badge/License-BSD-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)
[![Downloads](https://img.shields.io/github/downloads/jatinchowdhury18/ChowPhaser/total)](https://somsubhra.github.io/github-release-stats/?username=jatinchowdhury18&repository=ChowPhaser&page=1&per_page=30)

CHOW Phaser is an open-source phaser effect,
based *very* loosely on the classic Shulte Compact
Phasing "A". Check out the video demo on
[YouTube](https://youtu.be/QuDWKLaUi_Q).

<img src="https://github.com/jatinchowdhury18/ChowPhaser/blob/master/res/screenshot_stereo.PNG?raw=true" alt="Pic" height="450">


## Installation

To install, you can download the latest release from
[our website](https://chowdsp.com/products.html#phaser).
If you would like to try the most recent updates
(potentially unstable), see our
[Nightly Builds Page](https://chowdsp.com/nightly.html#phaser).
Linux users can also find builds available on the
[Open Build Service](https://build.opensuse.org/package/show/home:kill_it:JUCE/ChowPhaser),
courtesy of Konstantin Voinov.

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

## Credits

- GUI Design - [Margus Mets](mailto:hello@mmcreative.eu)
- GUI Framework - [Plugin GUI Magic](https://github.com/ffAudio/PluginGUIMagic)

## License

CHOWPhaser is open source, and is licensed under the BSD 3-clause license.
Enjoy!
