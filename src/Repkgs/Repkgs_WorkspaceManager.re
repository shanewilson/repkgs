module Bs = Repkgs_Bs;
module Utils = Repkgs_Utils;

module Compat = Repkgs_Compat;

module PackageJson = {
  [@decco.decode]
  type t = {
    name: string,
    version: option(string),
    [@decco.key "private"]
    private: option(bool),
    workspaces: option(array(string)),
  };
  let read = path =>
    path
    ->Utils.pathToManifest
    ->Node.Fs.readFileAsUtf8Sync
    ->Js.Json.parseExn
    ->t_decode
    ->Belt.Result.getExn;
};

[@genType]
type manifest = {
  name: string,
  absolute: string,
  type_: Compat.Common.workspaceType,
  packageJson: PackageJson.t,
};

[@genType]
let detectWorkspaceManager = Compat.detectWorkspaceManager;

[@genType]
let findRootDirectory = Compat.findRootDirectory;

[@genType]
let findWorkspaceDirectories = Compat.findWorkspaceDirectories;

[@genType]
let findWorkspaces = ws =>
  ws->Belt.Array.map(((absolute, type_)) => {
    Js.log(absolute);
    let packageJson = PackageJson.read(absolute);
    {name: packageJson.name, absolute, type_, packageJson};
  })
  |> Wonka.fromArray;