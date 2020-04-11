module Workspaces: {
  type t;
  let packages: t => List.t(string);
};
module Error: {
  type t = [
    | `PackageJsonDecode(Path.t, Decco.decodeError)
    | `PackageJsonParse(Path.t, Parse.Error.t)
  ];
};
type t;
let filename: string;
let name: t => option(string);
let private_: t => [> | `Private | `Public];
let workspaces: t => option(Workspaces.t);
let path: Path.t => Path.t;
let decode: Js.Json.t => Result.t(t, Decco.decodeError);
let encode: t => Js.Json.t;
let get: Path.t => Result.t(t, [> Fs.Error.t | Error.t]);