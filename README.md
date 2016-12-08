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

## Usage

hasher takes in a wordlist from `stdin` (pipe in your newline delimited 
wordlist) and (for now) on `stdout` the output format looks like this:

    hash:plaintext

This allows you to pipe the output into something like 'grep' to actually
get the output. An example run might be something like this:

    echo -n "hello" | ./hasher | grep 066DDFD4EF0E9CD7C256FE77191EF43C

The thought is that when the hash pops out, you've got the plain text.

## Hashing Algorithms Included

* NTLM

*More to come!*
