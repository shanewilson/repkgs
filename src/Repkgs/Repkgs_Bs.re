module Yaml = {
  [@bs.module "js-yaml"] external read: string => Js.Json.t = "safeLoad";
};

module FastGlob = {
  [@bs.deriving abstract]
  type options = {
    [@bs.optional]
    cwd: string,
    [@bs.optional]
    absolute: bool,
  };

  [@bs.module "fast-glob"]
  external sync: (array(string), options) => array(string) = "sync";
};