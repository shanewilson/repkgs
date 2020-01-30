module Bs = Repkgs_Bs;
module Utils = Repkgs_Utils;

module Common = {
  let readJson = paths => paths->Utils.read->Js.Json.parseExn;
  let readYaml = paths => paths->Utils.read->Bs.Yaml.read;
  let parse = (json, decode) => json |> decode |> Belt.Result.getExn;
  let packages = (cwd, manifest, patterns) =>
    [|cwd, manifest|]
    ->patterns
    ->Utils.findPatternMatches(
        Bs.FastGlob.options(~cwd, ~absolute=true, ()),
      );
};

module Pnpm = {
  [@decco.decode]
  type t = {packages: array(string)};

  let manifest = "pnpm-workspace.yaml";

  let read = Common.readYaml;

  let parse = json => json->Common.parse(t_decode);

  let patterns = path => path->read->parse->(m => m.packages);

  let packages = cwd => cwd->Common.packages(manifest, patterns);
};

module Rush = {
  [@decco.decode]
  type project = {projectFolder: string};

  [@decco.decode]
  type t = {projects: array(project)};

  let manifest = "rush.json";

  let read = Common.readJson;

  let parse = json => json->Common.parse(t_decode);

  let patterns = path =>
    path->read->parse->(m => m.projects)->Belt.Array.map(p => p.projectFolder);

  let packages = cwd => cwd->Common.packages(manifest, patterns);
};

module Yarn_V1 = {
  [@decco.decode]
  type t = {workspaces: array(string)};

  let manifest = "package.json";

  let read = Common.readJson;

  let parse = json => json->Common.parse(t_decode);

  let patterns = path => path->read->parse->(m => m.workspaces);

  let packages = cwd => cwd->Common.packages(manifest, patterns);
};

module Yarn_V2 = {
  [@decco.decode]
  type t = {
    [@decco.default [||]]
    workspaces: array(string),
  };

  let manifest = "package.json";

  let read = Common.readJson;

  let parse = json => json->Common.parse(t_decode);

  let patterns = path => path->read->parse->(m => m.workspaces);

  let rec packages = (cwd: string): array(string) =>
    cwd
    ->Common.packages(manifest, patterns)
    ->(
        workspaces =>
          switch (workspaces) {
          | [||] => [|cwd|]
          | _ => workspaces->Belt.Array.map(packages)->Belt.Array.concatMany
          }
      );
};