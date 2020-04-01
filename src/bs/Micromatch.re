[@bs.deriving abstract]
type t = {
  [@bs.optional]
  contains: bool,
};
[@bs.module "micromatch"]
external mm: (string, array(string), t) => array(string) = "default";