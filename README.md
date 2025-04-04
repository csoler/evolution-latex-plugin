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

