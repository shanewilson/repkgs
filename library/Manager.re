module Workspace = {
  type kind = Compat.Workspace.t;

  type t = {
    kind,
    name: string,
    packageJson: PackageJson.t,
  };
};

type t = {
  manager: Compat.t,
  root: Fpath.t,
  workspaces: list(Workspace.t),
};

let find_workspaces = cwd => {
  let cwd = cwd |> Fs.normalize_cwd;

  let (wsmgr, root) =
    Compat.managers
    |> Compat.detect_workspace_manager(~cwd)
    |> (
      fun
      | Some((w, d)) => (w, d)
      | None => raise(Errors.No_workspace_found)
    );

  {
    manager: wsmgr,
    root,
    workspaces:
      Compat.find_workspace_dirs(wsmgr, root)
      |> List.map((kind) =>
           (
             {
               let path = kind |> Compat.Workspace.to_path;

               let manifest =
                 Compat.Workspace.path_to_manifest(
                   path,
                   PackageJson.manifest_file,
                 );

               let packageJson =
                 switch (Fs.exists(manifest)) {
                 | Ok(File(p)) => p |> PackageJson.read_parse_manifest
                 | _ => {name: "root", private: true, version: None}
                 };

               {kind, name: packageJson.name, packageJson};
             }: Workspace.t
           )
         ),
  };
};