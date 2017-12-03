svg2pes
=======

A program which converts SVG to PES (a commonly used file format for
`embroidery machines <https://en.wikipedia.org/wiki/Machine_embroidery>`__
from `Brother <http://www.brother.com/index.htm>`__ and others).  This
project's goal is to be able to author
`embroidery <https://en.wikipedia.org/wiki/Embroidery#Machine>`__ designs
with `Inkscape <https://inkscape.org/>`__.

It is something like the opposite of `pesconvert
<https://git.kernel.org/cgit/linux/kernel/git/torvalds/pesconvert.git>`__.

This is work in progress.


Building svg2pes
----------------

You need:

- a C++14 compiler (GCC 5+ or clang)
- expat (``libexpat1-dev`` on Debian)

Quick walkthrough::

    ./autogen.sh
    ./configure
    make
    make install

This project uses
`autotools <https://en.wikipedia.org/wiki/GNU_build_system>`__.  If you
know autotools, you know how to tweak the build process.


Using svg2pes
-------------

Example::

    svg2pes test.svg test.pes

This command reads the file ``test.svg`` and then writes the file
``test.pes``.
