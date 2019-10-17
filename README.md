Some MSP430 launchpads such as the [MSP430FR4133 LaunchPad](http://www.ti.com/tool/msp-exp430fr4133)
include the so-called "EnergyTrace™ technology". It consists of a 
software-controlled DC-DC converter that can measure the energy it is 
delivering. Looks like TI has patents on that:
[US20130154594](http://www.google.com/patents/US20130154594),
[US20140253096](http://www.google.com/patents/US20140253096)

The "official" way to make use of this feature is TI's "Code 
Composer Studio" IDE. Since I don't use CCS, installing CCS just for 
measuring current consumption seems a bit silly.
Fortunately there's a better way: TI provides an open-source library 
for communicating with some of their MSP430 programmers. As well as 
debugging control and programming the library also gives access to the 
EnergyTrace feature. So I wrote a small program based on an example 
that reads the EnergyTrace measurements.

# Output Format
Data is written to stdout in 4 columns:

 1. Time in seconds
 2. Current in amps
 3. Voltage in volts
 4. Energy in Joules

Debug information gets prefixed with a `#`, so it gets ignored by 
gnuplot and the like. For some reason, differentiating and low-pass 
filtering the energy measurements leads to more accurate readings than 
the current measurement itself.

# Dependencies
You'll need MSP430 debug stack (libmsp430.so) and the usual things like make
and gcc. As building the MSP430 debug stack is a bit difficult at this time,
this fork realies on the precompiled libmsp430.so provided by TI in its
[MSP430Flasher](http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430Flasher/latest/index_FDS.html)
tool. The following instructions are known to work with 32-bit MSP430Flasher
1.3.7 and 64-bit (x64) MSP430Flasher 1.3.15.

# How do I build and run?

Download and extract the MSP430Flasher archive. Here, we will assume that
it has been extracted to `/opt/MSP430Flasher_1.3.15`, please adjust the
path accordingly.

```
$ make MSP430FLASHER=/opt/MSP430Flasher_1.3.15
$ ./energytrace <measurement duration in seconds> > energytrace.log
```

Add `-m32` to CFLAGS if you're using a 32-bit libmsp430.so variant.

Use your favourite tool for visualizing and processing the recorded data.

```
$ gnuplot

        G N U P L O T
        Version 5.2 patchlevel 2    last modified 2017-11-01 

        Copyright (C) 1986-1993, 1998, 2004, 2007-2017
        Thomas Williams, Colin Kelley and many others

        gnuplot home:     http://www.gnuplot.info
        faq, bugs, etc:   type "help FAQ"
        immediate help:   type "help"  (plot window: hit 'h')

Terminal type is now 'qt'
gnuplot> plot "energytrace.log" 
gnuplot> 
```

# Conclusion
EnergyTrace comes in really handy for measuring the power consumption of 
MCUs as it saves you from fiddling with current shunts, differential 
amplifiers and oscilloscopes.
Event when you're just using EnergyTrace, the MSP430FR4133 LaunchPad is 
pretty good value at $14.

This little program has proven to be useful during the development of 
[pluto](https://github.com/carrotIndustries/pluto).
