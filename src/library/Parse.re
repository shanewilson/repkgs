module Error = {
  type t = [ | `ParseJson(Js.Exn.t) | `ParseYaml(Js.Exn.t)];
};

let json = s =>
  try(s->Js.Json.parseExn->Ok) {
  | Js.Exn.Error(e) => `ParseJson(e)->Error
  };
let yaml = s =>
  try(s->Yaml.read->Ok) {
  | Js.Exn.Error(e) => `ParseYaml(e)->Error
  };