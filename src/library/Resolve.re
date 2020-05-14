[@bs.val] [@bs.scope "require"]
external _resolve: string => string = "resolve";
module Error = {
  type t = [ | `RequireLocal(Js.Exn.t)];
};
let resolve = s =>
  try(s->Path.toString->_resolve->Path.v->Ok) {
  | Js.Exn.Error(e) => `RequireLocal(e)->Error
  };