About TurboFEC
==============

TurboFEC is an implementation of LTE forward error correction encoders and
decoders. Included are convolutional codes, turbo codes, and associated rate
matching units that handle block interleaving, bit selection, and pruning.

Convolutional decoding uses Intel SSE3, SSE4, and AVX2 instructions if
available.

Turbo decoding requires Intel SSE3 or higher.

LTE specification and sections:

3GPP TS 36.212 "LTE Multiplexing and channel coding"

5.1.3.1 "Tail biting convolutional code"
5.1.3.2 "Turbo encoding"
5.1.3.1 "Rate matching for turbo coded transport channels"
5.1.3.2 "Rate matching for convolutionally coded transport channels
         and control information"

Compile
=======

$ ./autoreconf -i
$ ./configure
$ make

Test
====

$ make check

A collection of GSM, LTE, and other forward error correcting codes will
be tested.

=================================================
[+] Testing: GSM xCCH
[.] Specs: (N=2, K=5, non-recursive, flushed, not punctured)

[.] BER test:
[..] Testing:
[..] Input BER.......................... 0.007508
[..] Output BER......................... 0.000000
[..] Output FER......................... 0.000000 

Install
=======

$ sudo make install
