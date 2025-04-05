# evolution-latex-plugin
Plugin to write LaTeX equations in gnome Evolution mailer.

## Compilation with cmake

 $ mkdir build
 $ cd build
 $ cmake -DCMAKE_INSTALL_PREFIX=~/.local/share/evolution/modules -DFORCE_INSTALL_PREFIX=ON ..

## Compilation with qmake

 $ qmake -r CONFIG=debug
 $ make
 $ make install

## TODO

* properly rename plugin and relevant files
* check that the editor can handle the converted text and disable button if not
* setup a proper icon
* implement proper error handling
* add a config panel
* check that the required executables are available
* improve behavior when clicking again on already converted text
* cleanup unused example code
* have a valgrind pass

