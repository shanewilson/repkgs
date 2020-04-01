[@bs.deriving abstract]
type t = {
  [@bs.optional]
  compact: bool,
  [@bs.optional]
  showHidden: bool,
  [@bs.optional]
  colors: bool,
  [@bs.optional]
  depth: Js.Nullable.t(int),
};
[@bs.module "util"] external inspect: ('a, t) => string = "inspect";