[@bs.deriving abstract]
type t = {
  [@bs.optional]
  contains: bool,
};
[@bs.module "micromatch"]
external mm: (array(string), array(string), t) => array(string) = "default";