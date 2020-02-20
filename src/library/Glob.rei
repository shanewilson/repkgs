[@genType]
type t;
[@genType]
type t2;
[@genType]
let pp:
  t2 =>
  {
    .
    "cwd": string,
    "patterns": array(string),
  };
[@genType]
let v: (List.t(string), ~cwd: Path.t) => t2;
[@genType]
let mmatch: (string, ~patterns: List.t(string)) => List.t(string);
[@genType]
let vmatch: t2 => List.t(Path.t);
[@genType]
let vmatches: List.t(t2) => List.t(Path.t);
[@genType]
let vmatchesManifest: (List.t(t2), ~manifest: string) => List.t(Path.t);
[@genType]
let matches: (List.t(string), ~cwd: Path.t) => List.t(string);
[@genType]
let matchesPaths: (List.t(Path.t), ~cwd: Path.t) => List.t(Path.t);
[@genType]
let matchesPackages: (List.t(string), ~cwd: Path.t) => List.t(Path.t);