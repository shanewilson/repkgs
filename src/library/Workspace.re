module Error = {
  type t = [
    | `WorkspaceErrors(
        List.t(
          [
            Fs.Error.t
            | Parse.Error.t
            | PackageJson.Error.t
            | Package.Error.t
          ],
        ),
      )
  ];
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