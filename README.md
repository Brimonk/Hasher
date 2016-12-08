# Hasher

This project aims to provide functionality, written in standard C that can
perform the exact same magic as the [hashcat](https://hashcat.net/hashcat/),
but without requiring OpenCL libraries, and proprietary devices. Getting hashcat
(for me anyways) after this OpenCL requirement has constantly been finickey,
as hashcat no longer supports CPU cracking.

-----

My personal goal is to get the most commonly used algorithms (the algos that I
personally would like to have access to) working on my 
[parallella](https://www.parallella.org/). This device (and the devices that
follow it) have a very promising future. Simply put, it is a co-processor
without on board cache, so each processor simply accesses the onboard RAM.
While this is slower in comparison, the potential exists to have upwards of
512 core cpus, of the same size. With more onboard RAM, this type of device
(even with the price tripled) has more potential than leading GPU crackers.

## Installation

Once you have the git repo cloned onto your machine, a simple

    make

will compile the project for you. Because there aren't a bunch of features.

## Hashing Algorithms Included

* NTLM

*More to come!*
