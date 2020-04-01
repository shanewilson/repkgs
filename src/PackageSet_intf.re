module type M = {
  type t;
  let empty: t;
  let v: list(PackageJson.t) => t;
  let pp: t => string;
};