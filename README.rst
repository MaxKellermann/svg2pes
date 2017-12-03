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
- expat (`libexpat1-dev` on Debian)
- `Meson 0.37 <http://mesonbuild.com/>`__ and `Ninja <https://ninja-build.org/>`__

Run Meson::

 meson . output

Compile and install::

 ninja -C output
 ninja -C output install


Using svg2pes
-------------

Example::

    svg2pes test.svg test.pes

This command reads the file ``test.svg`` and then writes the file
``test.pes``.
