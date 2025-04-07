# evolution-latex-plugin
Plugin to write LaTeX equations in gnome Evolution mailer.

## Compilation with cmake

<pre>
 $ mkdir build
 $ cd build
 $ cmake -DCMAKE_INSTALL_PREFIX=~/.local/share/evolution/modules -DFORCE_INSTALL_PREFIX=ON ..
</pre>

## Compilation with qmake

<pre>
 $ qmake -r CONFIG=debug
 $ make
 $ make install
</pre>

## TODO

* improve behavior when clicking again on already converted text
* have a valgrind pass
