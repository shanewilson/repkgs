module Import: {
  type t =
    | Local(Path.t)
    | Unresolved(Path.t)
    | External(string);
  let v: (string, ~path: Path.t) => t;
  let toString: t => string;
  let pp: (t, ~cwd: Path.t) => string;
};
type t;
let fromArray: array(Import.t) => t;
let fromList: List.t(Import.t) => t;
let toArray: t => array(Import.t);
let toList: t => List.t(Import.t);
let diff: (t, t) => t;
let keepLocalImports: t => t;
let keepExternalImports: t => t;
let keepBrokenImports: t => t;