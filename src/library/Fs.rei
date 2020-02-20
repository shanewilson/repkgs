[@genType]
type t;
[@genType]
module Error: {
  type t = [
    | `FsDoesNotExist(Path.t)
    | `FsNotAFile(Path.t)
    | `FsNotADirectory(Path.t)
    | `ParseJson(Js.Exn.t)
    | `ParseYaml(Js.Exn.t)
  ];
  let handle: t => unit;
};
[@genType]
let toPath: t => Path.t;
[@genType]
let toString: t => string;
[@genType]
let pp: t => string;
[@genType]
let exists: Path.t => Result.t(Path.t, [> Error.t]);
[@genType]
let isDirectory: Path.t => Result.t(Path.t, [> Error.t]);
[@genType]
let isFile: Path.t => Result.t(Path.t, [> Error.t]);
[@genType]
let v: Path.t => t;
[@genType]
let toDirectory: Path.t => Result.t(Path.t, [> Error.t]);
[@genType]
let read: Path.t => Result.t(string, [> Error.t]);
[@genType]
let contents: Path.t => Result.t(string, [> Error.t]);
[@genType]
let parseJson: string => Result.t(Js.Json.t, [> Error.t]);
[@genType]
let parseYaml: string => Result.t(Js.Json.t, [> Error.t]);