module Compat = Repkgs_Compat;

module Bs = Repkgs_Bs;

module Utils = Repkgs_Utils;

[@genType]
let streamPackageDirs = cwd =>
  Compat.Pnpm.packages(cwd)
  ->Utils.findPatternMatches(Bs.FastGlob.options(~cwd, ()))
  ->Wonka.fromArray;