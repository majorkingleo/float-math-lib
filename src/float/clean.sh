#!/usr/bin/env bash

make distclean
rm -f INSTALL Makefile Makefile.in src/Makefile.in
rm -rf aclocal.m4 ar-lib autom4te.cache compile config.guess config.log config.status config.sub configure depcomp .deps src/.deps
rm -f install-sh missing 
