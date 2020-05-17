module Error = {
  type t = [ | `PackageMissingReqField(Path.t, string, string)];
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
    | (`Public, None) =>
      `PackageMissingReqField((
        path,
        "name",
        "Public packages need to be named",
      ))
      ->Error
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
let dependencies =
  fun
  | Publishable(p)
  | Private(p) =>
    switch (p.packageJson->PackageJson.dependencies) {
    | Some(dependencies) =>
      dependencies
      ->Js.Dict.keys
      ->Array.map(d
          // dependencies should be its own set
          =>
            ImportSet.Import.External({
              import: d,
              path: p.path,
              target: d->Path.v,
            })
          )
      ->ImportSet.fromArray
    | None => ImportSet.empty
    };

let publish =
  fun
  | Publishable(p) => Js.log("published " ++ p.name)
  | Private(_) => Js.log("cannot publish private package");