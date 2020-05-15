type packed = {
  files: ImportSet.t,
  imports: ImportSet.t,
};
type t = {
  pkg: Package.t,
  main: List.t(string),
  bin: List.t(string),
  files: List.t(string),
  errorsFound: bool,
  packed,
};
let v: Package.t => t;