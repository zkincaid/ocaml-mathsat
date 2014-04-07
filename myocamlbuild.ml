open Ocamlbuild_plugin

let _ = dispatch begin function
   | After_rules ->
       (* ocaml compile flags *)
       flag ["ocaml"; "compile"] & S[A"-ccopt"; A"-O9";A"-package";A"gmp"];

       (* C compile flags *)
       flag ["c"; "compile"] & S[A"-cc"; A"gcc"; A"-ccopt"; A"-fPIC"];

       flag ["c"; "ocamlmklib"] & S[A"-lmathsat";A"-lgmp";A"-lstdc++"];

       dep ["link"; "ocaml"; "use_mathsat"] ["libmathsat_stubs.a"];

       (* this is used to link cmxs files *)
       flag ["link"; "ocaml"; "link_mathsat"] (A"libmathsat_stubs.a");

       flag ["ocaml"; "use_mathsat"; "link"; "library"; "byte"] & S[A"-dllib"; A"-lmathsat_stubs" ];

       flag ["ocaml"; "use_mathsat"; "link"; "library"; "native"] & S[A"-cclib"; A"-lmathsat_stubs"; ];
       flag ["ocaml"; "use_mathsat"; "link"; "library"; "native"] & S[A"-cclib"; A"-lmathsat";A"-cclib";A"-lgmp";A"-cclib";A"-lstdc++"];

       flag ["ocaml"; "use_mathsat"; "link"; "program"; "native"] & S[A"-ccopt"; A"-L."; A"mathsat.cmxa"];
       flag ["ocaml"; "use_mathsat"; "link"; "program"; "byte"] & S[A"-ccopt"; A"-L."; A"mathsat.cma"];
   | _ -> ()
end
