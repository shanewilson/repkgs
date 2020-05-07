[@genType]
type t;
[@genType]
let pp:
  t =>
  {
    .
    "cwd": string,
    "patterns": array(string),
  };
[@genType]
let v: (List.t(string), ~cwd: Path.t) => t;
[@genType]
let mmatch: (string, ~patterns: List.t(string)) => List.t(string);
[@genType]
let mmatches: (List.t(string), ~patterns: List.t(string)) => List.t(string);
[@genType]
let vmatch: t => List.t(Path.t);
[@genType]
let vmatches: List.t(t) => List.t(Path.t);
[@genType]
let vmatchesManifest: (List.t(t), ~manifest: string) => List.t(Path.t);