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

* check that the editor can handle the converted text and disable button if not
* add a config panel
* check that the required executables are available
* improve behavior when clicking again on already converted text
* cleanup unused example code
* have a valgrind pass

