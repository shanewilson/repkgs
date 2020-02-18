open Cmdliner;

let run =
    (
      ~cwd,
      ~include_worktree,
      ~include_private,
      ~since,
      ~since_branch,
      ~since_latest_tag,
      ~include_patterns,
      ~exclude_patterns,
      ~only_fs_patterns,
      ~ignore_fs_patterns,
      (),
    ) => {
  let project = Library.Manager.find_workspaces(cwd);

  project.workspaces
  |> Library.Filters.include_filter(~include_worktree, ~include_private)
  |> Library.Filters.Name.include_matching_packages(
       ~patterns=include_patterns,
     )
  |> Library.Filters.Name.exclude_matching_packages(
       ~patterns=exclude_patterns,
     )
  |> Library.Filters.Since.since_filter(
       ~root=project.root,
       ~since,
       ~since_branch,
       ~since_latest_tag,
     )
  |> Library.Filters.Fs.only(~patterns=only_fs_patterns)
  |> Library.Filters.Fs.ignore(~patterns=ignore_fs_patterns)
  |> Library.Filters.alpha_sort
  |> List.iter((x: Library.Manager.Workspace.t) =>
       switch (x.kind) {
       | Root(_) => Logs.app(m => m("%s (root)", x.name))
       | WorkTree(_) => Logs.app(m => m("%s (worktree)", x.name))
       | PrivatePackage(_) => Logs.app(m => m("%s (private)", x.name))
       | Package(_) => Logs.app(m => m("%s", x.name))
       }
     );

  Lwt.return_ok();
};

let cmd = {
  let doc = "list workspaces found in the current directory";

  let runCommand =
      (
        _,
        cwd,
        include_worktree,
        include_private,
        since,
        since_branch,
        since_latest_tag,
        exclude_patterns,
        include_patterns,
        only_fs_patterns,
        ignore_fs_patterns,
      ) =>
    run(
      ~cwd,
      ~include_worktree,
      ~include_private,
      ~since,
      ~since_branch,
      ~since_latest_tag,
      ~exclude_patterns,
      ~include_patterns,
      ~only_fs_patterns,
      ~ignore_fs_patterns,
    )
    |> Utils.runCmd;

  (
    Term.(
      const(runCommand)
      $ Logger.args
      $ Args.cwd
      $ Args.Include.include_worktree
      $ Args.Include.include_private
      $ Args.Since.since
      $ Args.Since.since_branch
      $ Args.Since.since_latest_tag
      $ Args.Name.excluded
      $ Args.Name.included
      $ Args.Fs.only
      $ Args.Fs.ignore
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