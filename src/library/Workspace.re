// module Version = {
//   type t =
//     | Empty
//     | VersionFromJson(string)
//     | VersionFromCommit(string)
//     | VersionFromRegistry(string);

//   let empty = Empty;
//   let fromJson = s => VersionFromJson(s);
//   let fromCommit = s => VersionFromCommit(s);
//   let fromRegistry = s => VersionFromRegistry(s);
// };

// module Package = {
//   type t = {
//     name: string,
//     version: Version.t,
//     credentials: string,
//   };
// };

module Error = {
  type t = [
    | `WorkspaceErrors(
        List.t([ Fs.Error.t | PackageJson.Error.t | Package.Error.t]),
      )
  ];
  let handle = e =>
    switch (e) {
    | `WorkspaceErrors(es) =>
      Js.log("Workspace Errors: ");
      let _ =
        es->List.map(e =>
          switch (e) {
          | `FsDoesNotExist(p) => Fs.Error.handle(`FsDoesNotExist(p))
          | `FsNotADirectory(p) => Fs.Error.handle(`FsNotADirectory(p))
          | `FsNotAFile(p) => Fs.Error.handle(`FsNotAFile(p))
          | `ParseJson(exn) => Fs.Error.handle(`ParseJson(exn))
          | `ParseYaml(exn) => Fs.Error.handle(`ParseYaml(exn))
          | `PackageJsonDecode(p, err) =>
            PackageJson.Error.handle(`PackageJsonDecode((p, err)))
          | `PackageJsonParse(p, exn) =>
            PackageJson.Error.handle(`PackageJsonParse((p, exn)))
          | `ManifestDecode(p, err) =>
            Manager.Error.handle(`ManifestDecode((p, err)))
          | `ManifestParse(p, exn) =>
            Manager.Error.handle(`ManifestParse((p, exn)))
          | `ManagerManifestMissingPatterns(p) =>
            Manager.Error.handle(`ManagerManifestMissingPatterns(p))
          | `PackageMissingReqField(p, f) =>
            Package.Error.handle(`PackageMissingReqField((p, f)))
          }
        );
      ();
    };
};

type t = {
  root: Path.t,
  packages: List.t(Package.t),
};
let root = t => t.root;
let packages = t => t.packages;

let splitErrors = xs =>
  xs->List.reduce(([], []), ((oks, errs), r) =>
    switch (r) {
    | Ok(ok) => ([ok, ...oks], errs)
    | Error(err) => (oks, [err, ...errs])
    }
  );
let findPackages = patterns =>
  patterns
  ->Glob.vmatchesManifest(~manifest=PackageJson.filename)
  ->List.map(Package.v);
let v = (cwd, ~workspaces) => {
  let root = ""->Path.resolve(~cwd=cwd->Path.v);
  let patterns = root->Patterns.v(~patterns=workspaces->List.flatten);

  let packagesR = patterns->findPackages;
  let (packages, errors) = packagesR->splitErrors;
  switch (errors) {
  | [] => {root, packages}->Ok
  | _ => `WorkspaceErrors(errors)->Error
  };
};