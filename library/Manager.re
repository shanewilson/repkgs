module PackageJson = {
  open Protocol_conv_yaml;
  [@deriving protocol(~driver=(module Yaml))]
  type t = {
    name: string,
    [@default None]
    version: option(string),
  };

  let manifest_file = Fpath.v("package.json");

  let read_parse_manifest = Fs.read_and_parse(~parser=of_yaml);
};

module Workspace = {
  type kind = Compat.Workspace.t;

  type t = {
    kind,
    name: string,
    packageJson: PackageJson.t,
  };
};

let find_workspaces = cwd => {
  let cwd = cwd |> Fs.normalize_cwd;

  let (wsmgr, root) =
    cwd
    |> Compat.detect_workspace_manager
    |> (
      fun
      | Some((w, d)) => (w, d)
      | None => raise(Errors.No_workspace_found)
    );

  Compat.find_workspace_dirs(wsmgr, root)
  |> List.map((kind: Compat.Workspace.t) =>
       (
         {
           let path = kind |> Compat.Workspace.to_path;
           let packageJson =
             Compat.Workspace.path_to_manifest(
               path,
               PackageJson.manifest_file,
             )
             |> PackageJson.read_parse_manifest;

           {kind, name: packageJson.name, packageJson};
         }: Workspace.t
       )
     );
};