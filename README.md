ocaml-mathsat
=============

OCaml bindings for [MathSAT 5](http://mathsat.fbk.eu/)

Most of MathSAT's functionality required for checking satisfiability and
computing interpolants for linear rational/integer arithmetic is implemented.

Building
========

Requires MathSAT 5 to be installed in a place where gcc and ld can find it.
 Also requires MLGMPIDL and OUnit to be installed through ocamlfind.
* `make` builds the library and runs unit tests
* `make docs` builds documentation
* `make install` installs the library though ocamlfind

Links
=====
* [MathSAT 5 API reference](http://mathsat.fbk.eu/apireference.html)
* [MathSAT-ML](http://mathsat.fbk.eu/apireference.html)
  Another set of OCaml bindings for MathSAT 5 (via [Ctypes](https://github.com/ocamllabs/ocaml-ctypes)).  Currently doesn't support static linking.
