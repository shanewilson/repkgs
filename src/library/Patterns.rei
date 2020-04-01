module Error: {
  type t = [ | `ManagerManifestMissingPatterns(Path.t)];
  let handle: t => unit;
};
type t = List.t(Glob.t2);
let v: (Path.t, ~patterns: List.t(string)) => t;
let pp: t => string;