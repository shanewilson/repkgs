[@bs.val] [@bs.scope "require"]
external _resolve: string => string = "resolve";
[@bs.val] external _require: string => Js.t({..}) = "require";
module Error = {
  type t = [ | `RequireLocal(Js.Exn.t)];
};
let require = s =>
  try(s->Path.toString->_require->Ok) {
  | Js.Exn.Error(e) => `RequireLocal(e)->Error
  };
let resolve = s =>
  try(s->Path.toString->_resolve->Path.v->Ok) {
  | Js.Exn.Error(e) => `RequireLocal(e)->Error
  };