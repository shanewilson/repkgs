module Error = {
  type t = [ | `ManagerManifestMissingPatterns(Path.t)];
  let handle = e =>
    switch (e) {
    | `ManagerManifestMissingPatterns(p) =>
      Js.log2("manifest missing patterns: ", p)
    };
};

type t = List.t(Glob.t2);

module type TPatterns = {let readPatterns: t => t;};

module Patterns = (M: Manifest.Manifest) : TPatterns => {
  let readManifest = p =>
    switch (p->M.get) {
    | Error(err) => err->Error
    | Ok(pkg) =>
      switch (pkg->M.patterns) {
      | [] => `ManagerManifestMissingPatterns(p)->Error
      | pkgs => pkgs->Ok
      }
    };
  let rec readPatterns = globs =>
    globs
    ->Glob.vmatchesManifest(~manifest=M.filename)
    ->List.map(path =>
        switch (path->readManifest) {
        | Ok(ps) => [ps->Glob.v(~cwd=path)]->readPatterns
        | _ => []
        }
      )
    ->List.flatten
    ->List.concat(globs);
};

module Yarn = Patterns(Manifest.YarnJson);
module Lerna = Patterns(Manifest.LernaJson);
module Pnpm = Patterns(Manifest.PnpmYaml);
module Rush = Patterns(Manifest.RushJson);

let rec findPatterns = (p, ~fps) =>
  switch (fps) {
  | [] => p
  | [fn, ...fps] =>
    switch (p->fn) {
    | [_] => p->findPatterns(~fps)
    | ps => ps
    }
  };

let v = (path, ~patterns) => {
  let rootPattern = [[""]->Glob.v(~cwd=path)];
  switch (patterns) {
  | [] =>
    rootPattern->findPatterns(
      ~fps=[
        Rush.readPatterns,
        Pnpm.readPatterns,
        Lerna.readPatterns,
        Yarn.readPatterns,
      ],
    )
  | _ => [patterns->Glob.v(~cwd=path)]
  };
};

let pp = t =>
  t
  ->List.map(Glob.pp)
  ->List.toArray
  ->NodeUtils.inspect(
      NodeUtils.t(~colors=true, ~depth=Js.Nullable.null, ()),
    );