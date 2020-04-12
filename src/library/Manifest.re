module Error = {
  type t = [
    | `ManifestParse(Path.t, Parse.Error.t)
    | `ManifestDecode(Path.t, Decco.decodeError)
  ];
};

module type Config = {
  type t;
  let filename: string;
  let patterns: t => List.t(string);
  let path: Path.t => Path.t;
  let t_decode: Js.Json.t => Result.t(t, Decco.decodeError);
  let parse: string => Result.t(Js.Json.t, Parse.Error.t);
};

module type Manifest = {
  type t;
  let filename: string;
  let patterns: t => List.t(string);
  let get: Path.t => Result.t(t, [> Fs.Error.t | Error.t]);
};

module Manifest = (M: Config) : Manifest => {
  type t = M.t;
  let filename = M.filename;
  let patterns = M.patterns;
  let get = s => {
    let p = s->M.path;
    switch (p->Fs.read) {
    | Error(err) => err->Error
    | Ok(s) =>
      switch (s->M.parse) {
      | Error(err) => `ManifestParse((p, err))->Error
      | Ok(json) =>
        switch (json->M.t_decode) {
        | Error(err) => `ManifestDecode((p, err))->Error
        | Ok(t) => t->Ok
        }
      }
    };
  };
};

module Yarn_ = {
  type t = PackageJson.t;
  let t_decode = PackageJson.decode;
  let filename = PackageJson.filename;
  let patterns = p =>
    switch (p->PackageJson.workspaces) {
    | Some(ws) => ws->PackageJson.Workspaces.packages
    | None => []
    };
  List.fromArray;
  let path = p => p->Path.append(filename->Path.v);
  let parse = Parse.json;
};
module YarnJson = Manifest(Yarn_);

module Lerna_ = {
  [@decco.decode]
  type t = {
    [@decco.default [||]]
    packages: array(string),
  };
  let filename = "lerna.json";
  let patterns = p => p.packages->List.fromArray;
  let path = p => p->Path.append(filename->Path.v);
  let parse = Parse.json;
};
module LernaJson = Manifest(Lerna_);

module Pnpm_ = {
  [@decco.decode]
  type t = {
    [@decco.default [||]]
    packages: array(string),
  };
  let filename = "pnpm-workspace.yaml";
  let patterns = p => p.packages->List.fromArray;
  let path = p => p->Path.append(filename->Path.v);
  let parse = Parse.yaml;
};
module PnpmYaml = Manifest(Pnpm_);

module Rush_ = {
  [@decco.decode]
  type project = {
    packageName: string,
    projectFolder: string,
  };
  [@decco.decode]
  type t = {
    [@decco.default [||]]
    projects: array(project),
  };
  let filename = "rush.json";
  let patterns = p =>
    p.projects->Array.map(p => p.projectFolder)->List.fromArray;
  let path = p => p->Path.append(filename->Path.v);
  let parse = Parse.json;
};
module RushJson = Manifest(Rush_);