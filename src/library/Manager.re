module Manager = {
  type t = {
    root: Path.t,
    patterns: List.t(Glob.t2),
  };
  let pp = t =>
    {
      "root": t.root->Path.pp,
      "patterns": t.patterns->List.map(Glob.pp)->List.toArray,
    }
    ->NodeUtils.inspect(
        NodeUtils.t(~colors=true, ~depth=Js.Nullable.null, ()),
      );

  let rec findPatterns = (globs, ~manifest, ~getPatterns) =>
    globs
    ->Glob.vmatchesManifest(~manifest)
    ->List.map(path =>
        switch (path->getPatterns) {
        | Ok(ps) =>
          [ps->Glob.v(~cwd=path)]->findPatterns(~manifest, ~getPatterns)
        | _ => []
        }
      )
    ->List.flatten
    ->List.concat(globs);
};
module Error = {
  type t = [
    | `ManagerManifestMissingPatterns(Path.t)
    | `ManifestParse(Path.t, Fs.Error.t)
    | `ManifestDecode(Path.t, Decco.decodeError)
  ];
  let handle = e =>
    switch (e) {
    | `ManifestDecode(p, err) =>
      Js.log2("path: ", p->Path.pp);
      Js.log2("decode error", err);
    | `ManifestParse(p, exn) =>
      Js.log2("path: ", p->Path.pp);
      ignore(Fs.Error.handle(exn));
    | `ManagerManifestMissingPatterns(p) =>
      Js.log2("manifest missing patterns: ", p)
    };
};
type t =
  | Stdin(Manager.t)
  | Single(Manager.t)
  | Pnpm(Manager.t)
  | Rush(Manager.t)
  | Lerna(Manager.t)
  | Yarn(Manager.t);

module Stdin = {
  let v = (root, ~patterns) => Stdin({root, patterns});
};

module Yarn = {
  module Manifest = PackageJson;

  let getPatterns = p =>
    switch (p->Manifest.get) {
    | Error(err) => err->Error
    | Ok(pkg) =>
      switch (pkg->Manifest.workspaces) {
      | Some(ws) =>
        switch (ws->Manifest.Workspaces.packages) {
        | [] => `ManagerManifestMissingPatterns(p)->Error
        | ps => ps->Ok
        }
      | None => `ManagerManifestMissingPatterns(p)->Error
      }
    };
  let v = (root, ~patterns) => Yarn({root, patterns});
};

module Lerna = {
  module Manifest = Manifest.LernaJson;

  let getPatterns = p =>
    switch (p->Manifest.get) {
    | Error(err) => err->Error
    | Ok(pkg) =>
      switch (pkg->Manifest.patterns) {
      | [] => `ManagerManifestMissingPatterns(p)->Error
      | pkgs => pkgs->Ok
      }
    };
  let v = (root, ~patterns) => Lerna({root, patterns});
};

module Pnpm = {
  module Manifest = Manifest.PnpmYaml;

  let getPatterns = p =>
    switch (p->Manifest.get) {
    | Error(err) => err->Error
    | Ok(pkg) =>
      switch (pkg->Manifest.patterns) {
      | [] => `ManagerManifestMissingPatterns(p)->Error
      | pkgs => pkgs->Ok
      }
    };
  let v = (root, ~patterns) => Pnpm({root, patterns});
};

module Rush = {
  module Manifest = Manifest.RushJson;

  let getPatterns = p =>
    switch (p->Manifest.get) {
    | Error(err) => err->Error
    | Ok(pkg) =>
      switch (pkg->Manifest.patterns) {
      | [] => `ManagerManifestMissingPatterns(p)->Error
      | pkgs => pkgs->Ok
      }
    };

  let v = (root, ~patterns) => Rush({root, patterns});
};

module Single = {
  let v = (root): t =>
    Single({root, patterns: [[""]->Glob.v(~cwd=root)]});
};

let v = (path, ~patterns) => {
  switch (patterns) {
  | [] =>
    switch (
      [[""]->Glob.v(~cwd=path)]
      ->Manager.findPatterns(
          ~manifest=Rush.Manifest.filename,
          ~getPatterns=Rush.getPatterns,
        )
    ) {
    | [_] =>
      switch (
        [[""]->Glob.v(~cwd=path)]
        ->Manager.findPatterns(
            ~manifest=Pnpm.Manifest.filename,
            ~getPatterns=Pnpm.getPatterns,
          )
      ) {
      | [_] =>
        switch (
          [[""]->Glob.v(~cwd=path)]
          ->Manager.findPatterns(
              ~manifest=Lerna.Manifest.filename,
              ~getPatterns=Lerna.getPatterns,
            )
        ) {
        | [_] =>
          switch (
            [[""]->Glob.v(~cwd=path)]
            ->Manager.findPatterns(
                ~manifest=Yarn.Manifest.filename,
                ~getPatterns=Yarn.getPatterns,
              )
          ) {
          | [_] => path->Single.v
          | ps => path->Yarn.v(~patterns=ps)
          }
        | ps => path->Lerna.v(~patterns=ps)
        }
      | ps => path->Pnpm.v(~patterns=ps)
      }
    | ps => path->Rush.v(~patterns=ps)
    }
  | _ => path->Stdin.v(~patterns)
  };
};

let root =
  fun
  | Stdin(x)
  | Single(x)
  | Pnpm(x)
  | Rush(x)
  | Lerna(x)
  | Yarn(x) => x.root;

let patterns =
  fun
  | Stdin(x)
  | Single(x)
  | Pnpm(x)
  | Rush(x)
  | Lerna(x)
  | Yarn(x) => x.patterns;

let pp =
  fun
  | Stdin(x) => Js.log2("Stdin::", x->Manager.pp)
  | Single(x) => Js.log2("Single::", x->Manager.pp)
  | Pnpm(x) => Js.log2("Pnpm::", x->Manager.pp)
  | Rush(x) => Js.log2("Rush::", x->Manager.pp)
  | Lerna(x) => Js.log2("Lerna::", x->Manager.pp)
  | Yarn(x) => Js.log2("Yarn::", x->Manager.pp);