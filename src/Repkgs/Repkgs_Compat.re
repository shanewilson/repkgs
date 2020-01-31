module Bs = Repkgs_Bs;
module Utils = Repkgs_Utils;

module Common = {
  let readJson = paths =>
    switch (paths->Utils.read) {
    | res => res->Js.Json.parseExn
    | exception _ => "{}"->Js.Json.parseExn
    };
  let readYaml = paths =>
    switch (paths->Utils.read) {
    | res => res->Bs.Yaml.read
    | exception _ => "{}"->Js.Json.parseExn
    };
  let parse = (json, decode) => json |> decode |> Belt.Result.getExn;
  let rec packages =
          (cwd, ~manifest, ~patterns, ~absolute=true, ~nested=false, ()) =>
    [|cwd, manifest|]
    ->patterns
    ->Utils.findPatternMatches(Bs.FastGlob.options(~cwd, ~absolute, ()))
    ->(
        workspaces =>
          nested
            ? switch (workspaces) {
              | [||] => [|cwd|]
              | _ =>
                workspaces
                ->Belt.Array.map(x =>
                    packages(x, ~manifest, ~patterns, ~absolute, ~nested, ())
                  )
                ->Belt.Array.concatMany
              }
            : workspaces
      );
};

module Pnpm = {
  [@decco.decode]
  type t = {
    [@decco.default [||]]
    packages: array(string),
  };

  let manifest = "pnpm-workspace.yaml";

  let read = Common.readYaml;

  let parse = json => json->Common.parse(t_decode);

  let patterns = path => path->read->parse->(m => m.packages);

  let packages = cwd => cwd->Common.packages(~manifest, ~patterns, ());
};

module Rush = {
  [@decco.decode]
  type project = {projectFolder: string};

  [@decco.decode]
  type t = {
    [@decco.default [||]]
    projects: array(project),
  };

  let manifest = "rush.json";

  let read = Common.readJson;

  let parse = json => json->Common.parse(t_decode);

  let patterns = path =>
    path->read->parse->(m => m.projects)->Belt.Array.map(p => p.projectFolder);

  let packages = cwd => cwd->Common.packages(~manifest, ~patterns, ());
};

module Yarn_V1 = {
  [@decco.decode]
  type t = {
    [@decco.default [||]]
    workspaces: array(string),
  };

  let manifest = "package.json";

  let read = Common.readJson;

  let parse = json => json->Common.parse(t_decode);

  let patterns = path => path->read->parse->(m => m.workspaces);

  let packages = cwd => cwd->Common.packages(~manifest, ~patterns, ());
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

  let packages = cwd =>
    cwd->Common.packages(~manifest, ~patterns, ~nested=true, ());
};