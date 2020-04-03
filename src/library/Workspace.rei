module Error: {
  type t = [
    | `WorkspaceErrors(
        List.t([ Fs.Error.t | PackageJson.Error.t | Package.Error.t]),
      )
  ];
  let handle: t => unit;
};

type t;
let v:
  (string, ~workspaces: List.t(List.t(string))) => Result.t(t, [> Error.t]);
let root: t => Path.t;
let packages: t => List.t(Package.t);