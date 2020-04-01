[@bs.deriving abstract]
type t = {
  [@bs.optional]
  cwd: string,
  [@bs.optional]
  absolute: bool,
  [@bs.optional]
  baseNameMatch: bool,
  [@bs.optional]
  ignore: array(string),
};
[@bs.module "fast-glob"]
external sync: (array(string), t) => array(string) = "sync";