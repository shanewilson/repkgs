[@genType]
module Error: {type t = [ | `ParseJson(Js.Exn.t) | `ParseYaml(Js.Exn.t)];};
[@genType]
let json: string => Result.t(Js.Json.t, [> Error.t]);
[@genType]
let yaml: string => Result.t(Js.Json.t, [> Error.t]);