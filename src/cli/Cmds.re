open Cmdliner;

module ListCmd = {
  let runCommand =
      (
        cwd,
        workspaces,
        includePrivate,
        nameOnly,
        nameIgnored,
        pathOnly,
        pathIgnored,
        fsOnly,
        fsIgnored,
      ) => {
    let workspace = cwd->Workspace.v(~workspaces);

    switch (workspace) {
    | Error(`WorkspaceErrors(es)) =>
      `WorkspaceErrors(es)->Workspace.Error.handle
    | Ok(wk) =>
      let _ =
        wk
        ->Workspace.packages
        ->Package.Filters.Include.includePrivate(~includePrivate)
        ->Package.Filters.Name.only(~nameOnly)
        ->Package.Filters.Name.ignored(~nameIgnored)
        ->Package.Filters.Path.only(~pathOnly)
        ->Package.Filters.Path.ignored(~pathIgnored)
        ->Package.Filters.Fs.only(~fsOnly)
        ->Package.Filters.Fs.ignored(~fsIgnored)
        ->List.sort((a, b) =>
            String.compare(a->Package.name, b->Package.name)
          )
        ->List.map(Package.name)
        ->List.map(Js.log);
      ();
    };
  };

  let cmd = {
    let doc = "list workspaces found in the current directory";

    (
      Term.(
        const(runCommand)
        $ Args.Common.cwd
        $ Args.Common.workspaces
        $ Args.Include.includePrivate
        $ Args.Name.only
        $ Args.Name.ignored
        $ Args.Path.only
        $ Args.Path.ignored
        $ Args.Fs.only
        $ Args.Fs.ignored
      ),
      Term.info("list", ~doc, ~sdocs=Manpage.s_common_options),
    );
  };
};

let cmds = [ListCmd.cmd];