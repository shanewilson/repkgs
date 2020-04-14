module Error: {
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

type t;
let v:
  (Path.t, ~workspaces: List.t(List.t(string))) => Result.t(t, [> Error.t]);
let findRoot: string => Path.t;
let root: t => Path.t;
let packages: t => List.t(Package.t);