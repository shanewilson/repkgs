type packed = {
  files: ImportSet.t,
  imports: ImportSet.t,
};
type t = {
  pkg: Package.t,
  main: List.t(string),
  types: List.t(string),
  bin: List.t(string),
  files: List.t(string),
  errors: int,
  packed,
};
let v: Package.t => t;