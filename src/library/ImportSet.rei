module Import: {
  type import = {
    path: Path.t,
    target: Path.t,
    import: string,
  };
  type t =
    | Local(import)
    | Unresolved(import)
    | External(import);
  let v: (string, ~path: Path.t) => t;
  let path: t => Path.t;
  let target: t => Path.t;
  let import: t => string;
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