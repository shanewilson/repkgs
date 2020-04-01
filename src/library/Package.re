module Error = {
  type t = [ | `PackageMissingReqField(Path.t, string)];

  let handle = err =>
    switch (err) {
    | `PackageMissingReqField(p, f) =>
      Js.log2("package.json at:", p);
      Js.log2("Missing field:", f);
    };
};
type pkg = {
  name: string,
  path: Path.t,
  packageJson: PackageJson.t,
};

type t =
  | Publishable(pkg)
  | Private(pkg);

let name =
  fun
  | Publishable(pkg)
  | Private(pkg) => pkg.name;
let path =
  fun
  | Publishable(pkg)
  | Private(pkg) => pkg.path;
let packageJson =
  fun
  | Publishable(pkg)
  | Private(pkg) => pkg.packageJson;
let v = (path: Path.t) => {
  switch (path->PackageJson.get) {
  | Ok(json) =>
    switch (json->PackageJson.private_, json->PackageJson.name) {
    | (`Private, _) =>
      Private({
        name:
          switch (json->PackageJson.name) {
          | Some(n) => n
          | None => ""
          },
        path,
        packageJson: json,
      })
      ->Ok
    | (`Public, Some(name)) =>
      Publishable({name, path, packageJson: json})->Ok
    | (`Public, None) => `PackageMissingReqField((path, "name"))->Error
    }
  | Error(err) => err->Error
  };
};
let pp =
  fun
  | Publishable(p)
  | Private(p) => {
      "name": p.name,
      "path": p.path->Path.pp,
      "packageJson": p.packageJson,
    };
let publish =
  fun
  | Publishable(p) => Js.log("published " ++ p.name)
  | Private(_) => Js.log("cannot publish private package");

module Filters = {
  let only =
    fun
    | [] => false
    | _ => true;
  let ignored = xs => !xs->only;
  let triggerOn = (p, ~xs, ~fn) =>
    switch (xs) {
    | [] => true
    | _ => p->fn
    };
  module Include = {
    let includePrivate = (ps, ~includePrivate) =>
      ps->List.keep(
        fun
        | Publishable(_) => true
        | Private(_) => includePrivate,
      );
  };
  module Name = {
    let only = (ps, ~nameOnly) =>
      ps->List.keep(
        triggerOn(~xs=nameOnly, ~fn=p =>
          p->name->Glob.mmatch(~patterns=nameOnly)->only
        ),
      );
    let ignored = (ps, ~nameIgnored) =>
      ps->List.keep(
        triggerOn(~xs=nameIgnored, ~fn=p =>
          p->name->Glob.mmatch(~patterns=nameIgnored)->ignored
        ),
      );
  };
  module Path = {
    let only = (ps, ~pathOnly) =>
      ps->List.keep(
        triggerOn(~xs=pathOnly, ~fn=p =>
          p->path->Path.toString->Glob.mmatch(~patterns=pathOnly)->only
        ),
      );
    let ignored = (ps, ~pathIgnored) =>
      ps->List.keep(
        triggerOn(~xs=pathIgnored, ~fn=p =>
          p->path->Path.toString->Glob.mmatch(~patterns=pathIgnored)->ignored
        ),
      );
  };
  module Fs = {
    let only = (ps, ~fsOnly) =>
      ps->List.keep(
        triggerOn(~xs=fsOnly, ~fn=p =>
          fsOnly->Glob.v(~cwd=p->path)->Glob.vmatch->only
        ),
      );
    let ignored = (ps, ~fsIgnored) =>
      ps->List.keep(
        triggerOn(~xs=fsIgnored, ~fn=p =>
          fsIgnored->Glob.v(~cwd=p->path)->Glob.vmatch->ignored
        ),
      );
  };
};