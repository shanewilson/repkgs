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
    Ink.render(
      <ListPackages
        cwd
        workspaces
        includePrivate
        nameOnly
        nameIgnored
        pathOnly
        pathIgnored
        fsOnly
        fsIgnored
      />,
    );
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