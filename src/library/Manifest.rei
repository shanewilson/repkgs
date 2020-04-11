module Error: {
  type t = [
    | `ManifestParse(Path.t, Parse.Error.t)
    | `ManifestDecode(Path.t, Decco.decodeError)
  ];
};
module type Manifest = {
  type t;
  let filename: string;
  let patterns: t => List.t(string);
  let get: Path.t => Result.t(t, [> Fs.Error.t | Error.t]);
};
module LernaJson: Manifest;
module PnpmYaml: Manifest;
module RushJson: Manifest;
module YarnJson: Manifest;