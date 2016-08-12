About TurboFEC
==============

TurboFEC is an implementation of LTE forward error correction encoders and
decoders. Included are convolutional codes, turbo codes, and associated rate
matching units that handle block interleaving, bit selection, and pruning.

Using x86 SIMD instructions, the convolutional and turbo decoders are currently
the fastest implementations openly available. If you believe the preceding
statement to be false, please contact the author to discuss further
optimizations.

Convolutional decoding uses Intel SSE3, SSE4, and AVX2 instructions if
available.

Turbo decoding requires Intel SSE3 or higher.

LTE specification and sections:

3GPP TS 36.212 *"LTE Multiplexing and channel coding"*

5.1.3.1 *"Tail biting convolutional code"*

5.1.3.2 *"Turbo encoding"*

5.1.3.1 *"Rate matching for turbo coded transport channels"*

5.1.3.2 *"Rate matching for convolutionally coded transport channels
         and control information"*

Compile
=======
```
$ autoreconf -i
$ ./configure
$ make
```

Test
====
```
$ make check
```

A collection of GSM, LTE, and other forward error correcting codes will
be tested.

```
=================================================
[+] Testing: GSM TCH/AHS 5.15
[.] Specs: (N=3, K=5, recursive, flushed, punctured)

[.] BER test:
[..] Testing:
[..] Input BER.......................... 0.007450
[..] Output BER......................... 0.000000
[..] Output FER......................... 0.000000

=================================================
[+] Testing: LTE PBCH
[.] Specs: (N=3, K=7, non-recursive, tail-biting, non-punctured)

[.] BER test:
[..] Testing:
[..] Input BER.......................... 0.007478
[..] Output BER......................... 0.000000
[..] Output FER......................... 0.000000

PASS: conv_test

=================================================
[+] Testing: 3GPP LTE turbo
[.] Specs: (N=2, K=4), Length 6144

[.] BER test:
[..] Testing:
[..] Input BER.......................... 0.007479
[..] Output BER......................... 0.000000
[..] Output FER......................... 0.000000

PASS: turbo_test
```

Install
=======
```
$ sudo make install
```
