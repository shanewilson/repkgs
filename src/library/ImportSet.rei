module Import: {
  type import = string;
  type t =
    | Local(import, Path.t)
    | Unresolved(import, Path.t)
    | External(import);
  let v: (string, ~path: Path.t) => t;
  let toString: t => string;
};
type t;
let empty: t;
let fromArray: array(Import.t) => t;
let fromList: List.t(Import.t) => t;
let toArray: t => array(Import.t);
let toList: t => List.t(Import.t);
let diff: (t, t) => t;
let keepLocalImports: t => t;
let keepExternalImports: t => t;
let keepBrokenImports: t => t;