open Cmdliner;

let run = (~cwd, ~include_worktree, ~since, ()) => {
  let project = Library.Manager.find_workspaces(cwd);

  project.workspaces
  |> Library.Filters.include_worktree_filter(~include_worktree)
  |> Library.Filters.since_filter(~root=project.root, ~since)
  |> Library.Filters.alpha_sort
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

  let runCommand = (_, cwd, include_worktree, since) =>
    run(~cwd, ~include_worktree, ~since) |> Utils.runCmd;

  (
    Term.(
      const(runCommand)
      $ Logger.args
      $ Args.cwd
      $ Args.include_worktree
      $ Args.since
    ),
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