--- reverse polish notation command line calculator ---

This is a stupid reverse polish notation command line calculator tool,
that uses GMP (GNU Multiple Precision Arithmetic Library) for its
calculations.

It doesn't take any command line arguments; it is designed only for
interactive operation. It provides the 4 basic arithmetic functions,
plus a few stack manipulation commands.

Commands:
  + - * /   Arithmetic ops
  d         Duplicate top of stack
  s         Swap top two items
  x         Pop stack
  c         Clear stack
  ? help    Show this help message
  q         Quit

Problems with common calculators:

  - Whatever calculator your desktop environment comes with:
    Ridiculous UI that tries to duplicate the interface of a physical
    desktop calculator. May or may not perform calculations with
    arbitrary precision arithmetic (e.g., old versions of windows
    calculator didn't, newer versions do, I don't know what the
    various open source options do).

  - Python, etc: good language, flexible, but floating point numbers
    are only double precision. It has a decimal library for exact
    decimal calculations, but that makes simple arithmetic much more
    verbose.

  - 'dc' tool: simple, uses arbitrary precision arithmetic, but poor
    interactive mode operation (you have to keep telling it to print
    the stack out, or be very careful about remembering).

  - 'bc' tool: simple, uses arbitrary precision arithmetic, but again
    pretty poor interface for interactive work.
