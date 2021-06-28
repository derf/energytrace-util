# energytrace-util – MSP430 EnergyTrace CLI

Some MSP430 launchpads such as the [MSP430FR4133
LaunchPad](http://www.ti.com/tool/msp-exp430fr4133) include the so-called
"EnergyTrace™ technology". It consists of a software-controlled DC-DC converter
that can measure the energy it is delivering. See [Automatic Energy Model
Generation with MSP430
EnergyTrace](https://ess.cs.uos.de/static/papers/Friesel-2021-CPSIoTBench.pdf)
for accuracy figures and some odds and ends.

The "official" way to make use of this feature is TI's *Code Composer Studio*
IDE. For scripting purposes, EnergyTrace measurements can also be obtained from
TI's closed source MSP430 API. **energytace-util** exposes this API to the
command line.

## Usage

```
energytrace <timeout>
```

If timeout is **0**, run indefinitely. In any case, measurements can be
terminated gracefully by sending SIGTERM or SIGINT to **energytrace**
(i.e., by pressing Ctrl+C).

Measurement data is written to stdout.

# Output Format

Data is written to stdout in 4 columns:

1. Timestamps (µs) since start of the measurement,
2. Mean current (nA) over the last ten to thousand intervals,
3. Mean voltage (mV) with details unknown, and
4. Cumulative energy (nJ) since start of the measurement.

Note that the EnergyTrace hardware and firmware only measure time, voltage, and
energy. Current readings are calculated by the MSP430 client library and
heavily filtered. Differentiating and low-pass filtering the energy
measurements leads to far more accurate readings than the provided current
data.

Debug information is prefixed with a `#`, so it is ignored by gnuplot and the
likes.

## Dependencies

You'll need MSP430 debug stack (libmsp430.so) and the usual things like make
and a not too recent gcc (version 8 works fine). As building the MSP430 debug
stack is a bit difficult at this time, this fork relies on the precompiled
libmsp430.so provided by TI in its
[MSP430Flasher](http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430Flasher/latest/index_FDS.html)
tool. The following instructions are known to work with 32-bit MSP430Flasher
1.3.7 and 64-bit (x64) MSP430Flasher 1.3.15.

## Build

Download and extract the MSP430Flasher archive. Here, we assume that it has
been extracted to `/opt/MSP430Flasher_1.3.15`, please adjust the path
accordingly.

```
$ make MSP430FLASHER=/opt/MSP430Flasher_1.3.15
$ ./energytrace <measurement duration in seconds> > energytrace.log
```

Add `-m32` to CFLAGS if you're using a 32-bit libmsp430.so variant.

Use your favourite tool for visualizing and processing the recorded data.

If you want to use EnergyTrace++ (with CPU state data), refer to
energytracepp.c. Note that this decreases the sampling rate from ~3.7 to
~1.1 kHz and may significantly delay CPU state transitions, especially when
waking up from low-power modes.
