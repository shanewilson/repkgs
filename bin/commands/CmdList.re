open Cmdliner;

let run = (~cwd, ~include_worktree, ()) => {
  let _ =
    Library.Manager.find_workspaces(cwd)
    |> List.filter((pkg: Library.Manager.Workspace.t) =>
         switch (include_worktree, pkg.kind) {
         | (true, _)
         | (_, Package(_)) => true
         | (false, _) => false
         }
       )
    |> List.fast_sort(
         (a: Library.Manager.Workspace.t, b: Library.Manager.Workspace.t) =>
         String.compare(
           a.kind |> Library.Compat.Workspace.to_path |> Fpath.to_string,
           b.kind |> Library.Compat.Workspace.to_path |> Fpath.to_string,
         )
       )
    |> List.iter((x: Library.Manager.Workspace.t) =>
         switch (x.kind) {
         | Root(_) => Logs.app(m => m("%s (root)", x.name))
         | WorkTree(_) => Logs.app(m => m("%s (worktree)", x.name))
         | Package(_) => Logs.app(m => m("%s", x.name))
         }
       );

  Lwt.return_ok();
};

let cmd = {
  let doc = "list workspaces found in the current directory";

  let cwd = {
    let doc = "Run the program in directory $(docv).";
    Arg.(
      value
      & opt(dir, Library.Fs.get_cwd() |> Fpath.to_string)
      & info(["cwd"], ~docv="DIR", ~doc)
    );
  };

  let include_worktree = {
    let doc = "Include worktree/root dir in output.";
    Arg.(value & flag & info(["include-worktree"], ~doc));
  };

  let runCommand = (_, cwd, include_worktree) =>
    run(~cwd, ~include_worktree) |> Utils.runCmd;

  (
    Term.(const(runCommand) $ Logger.args $ cwd $ include_worktree),
    Term.info(
      "list",
      ~doc,
      ~version=Man.version,
      ~exits=Man.exits,
      ~man=Man.man,
      ~sdocs=Man.sdocs,
    ),
  );
};