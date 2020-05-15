module Error: {type t = [ | `RequireLocal(Js.Exn.t)];};
let resolve: Path.t => Result.t(Path.t, [> | `RequireLocal(Js.Exn.t)]);