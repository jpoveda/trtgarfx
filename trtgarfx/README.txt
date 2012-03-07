
   TRTGARF, Simulation classes for the ATLAS TRT(CERN)

Required Libraries:

    Ionization and Drift requires Garfield++

    Signal processing with the ASDBLR may requires a working Spice (currently HSpice) if
     signals tested are not within ordinary boundaries (low threshold, single particle ionisation)

Provided C++ class libraries:

  Signals

    Provides a signal container in Signals\Signal.*

    Signals are streamer components and can be extended
    behave like a specific electronic component.
    For example a Highpass in Signals\Highpass.*

    Signals\SignalSet.* is a container for sets
    of signals.

    Signals\SignalPairSet.* is a container for
    signal pairs (and more). Used to find coefficients
    for differential equations.

  SSH

    Wrapper class for libssh

  TRT

    Wrapper for Garfield++

  TRTElectronics

    Pieces of the ASDBLR

  Tools

    ASCII streamers, Exceptions and other things
    the C++ "standard library" lacks.

Programs:

  cs.cpp

    Simulates streamer signals using HSpice. If you wish individual signals, please create the debs subdirectory.

  *mc.cpp

    Find coefficients for the individual parts of the ASDBLR

  h.cpp

    Simulate effect of pile up for two pions

  gtail.cpp

    Find coefficients for the ion tail

  