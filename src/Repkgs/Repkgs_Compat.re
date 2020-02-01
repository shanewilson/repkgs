module Bs = Repkgs_Bs;
module Utils = Repkgs_Utils;

// find root

module Common = {
  [@genType]
  type matched = {
    absolute: string,
    worktree: bool,
  };

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

  let rec findClosestWorktree = (path, patterns) => {
    let cwd = Node.Path.resolve(path, "");
    let ws = cwd->patterns;

    switch (cwd) {
    | cwd when cwd == Node.Path.parse(cwd)##root =>
      Belt.Result.Error("No Workspace Found")
    | _ =>
      switch (ws) {
      | [||] =>
        findClosestWorktree(
          cwd->Node.Path.join2("..")->Node.Path.normalize,
          patterns,
        )
      | _ => Belt.Result.Ok(cwd)
      }
    };
  };

  let rec findHighestWorktree = (path, patterns, ~closest=None, ~depth=3, ()) => {
    let cwd = Node.Path.resolve(path, "");
    Js.log(cwd);
    Js.log(depth);
    let ws = cwd->patterns;

    switch (cwd) {
    | cwd when depth === 0 || cwd == Node.Path.parse(cwd)##root =>
      switch (closest) {
      | Some(closest) => Belt.Result.Ok(closest)
      | None => Belt.Result.Error("No Workspace Found")
      }
    | _ =>
      let fhw =
        findHighestWorktree(
          cwd->Node.Path.join2("..")->Node.Path.normalize,
          patterns,
        );
      switch (ws) {
      | [||] => fhw(~closest, ~depth=depth - 1, ())
      | _ => fhw(~closest=Some(cwd), ~depth=3, ())
      };
    };
  };

  let rec packages = (cwd, ~patterns, ~nested=false, ()) => {
    let ws =
      cwd
      ->patterns
      ->Utils.findPatternMatches(
          Bs.FastGlob.options(~cwd, ~absolute=true, ()),
        );

    nested
      ? switch (ws) {
        | [||] => [|{absolute: cwd, worktree: false}|]
        | _ =>
          ws
          ->Belt.Array.map(w => packages(w, ~patterns, ~nested, ()))
          ->Belt.Array.concatMany
          ->Belt.Array.concat([|{absolute: cwd, worktree: true}|])
        }
      : Belt.Array.map(ws, w => {absolute: w, worktree: false});
  };
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

  let patterns = path => [|path, manifest|]->read->parse->(m => m.packages);

  let findRoot = path => path->Common.findClosestWorktree(patterns);

  let packages = cwd => cwd->Common.packages(~patterns, ());

  let detect = cwd => cwd->findRoot->Belt.Result.isOk;
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
    [|path, manifest|]
    ->read
    ->parse
    ->(m => m.projects)
    ->Belt.Array.map(p => p.projectFolder);

  let findRoot = path => path->Common.findClosestWorktree(patterns);

  let packages = cwd => cwd->Common.packages(~patterns, ());

  let detect = cwd => cwd->findRoot->Belt.Result.isOk;
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

  let patterns = path => [|path, manifest|]->read->parse->(m => m.workspaces);

  let findRoot = path => path->Common.findHighestWorktree(patterns, ());

  let packages = cwd => cwd->Common.packages(~patterns, ());

  let detect_v1 = res => {
    switch ([|res->Belt.Result.getExn, ".yarnrc.yml"|]->Utils.read) {
    | _ => false
    | exception _ => true
    };
  };

  let detect = cwd => {
    switch (cwd->findRoot) {
    | res when res->Belt.Result.isOk => res->detect_v1
    | _ => false
    };
  };
};

module Yarn_V2 = {
  type t = Yarn_V1.t;

  let manifest = Yarn_V1.manifest;

  let read = Yarn_V1.read;

  let parse = Yarn_V1.parse;

  let patterns = Yarn_V1.patterns;

  let findRoot = path => path->Common.findHighestWorktree(patterns, ());

  let packages = cwd => cwd->Common.packages(~patterns, ~nested=true, ());

  let detect = cwd => {
    switch (cwd->findRoot) {
    | res when res->Belt.Result.isOk => !res->Yarn_V1.detect_v1
    | _ => false
    };
  };
};