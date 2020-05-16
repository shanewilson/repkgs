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
        since,
        sinceBranch,
        sinceLatestTag,
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
        since
        sinceBranch
        sinceLatestTag
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
        $ Args.Changed.since
        $ Args.Changed.sinceBranch
        $ Args.Changed.sinceLatestTag
      ),
      Term.info("list", ~doc, ~sdocs=Manpage.s_common_options),
    );
  };
};

module VerifyFilesCmd = {
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
        since,
        sinceBranch,
        sinceLatestTag,
      ) => {
    Ink.render(
      <VerifyFiles
        cwd
        workspaces
        includePrivate
        nameOnly
        nameIgnored
        pathOnly
        pathIgnored
        fsOnly
        fsIgnored
        since
        sinceBranch
        sinceLatestTag
      />,
    );
  };

  let cmd = {
    let doc = "verify the files you expect to ship exist";

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
        $ Args.Changed.since
        $ Args.Changed.sinceBranch
        $ Args.Changed.sinceLatestTag
      ),
      Term.info("verify-files", ~doc, ~sdocs=Manpage.s_common_options),
    );
  };
};

module VerifyDepsCmd = {
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
        since,
        sinceBranch,
        sinceLatestTag,
      ) => {
    Ink.render(
      <VerifyDeps
        cwd
        workspaces
        includePrivate
        nameOnly
        nameIgnored
        pathOnly
        pathIgnored
        fsOnly
        fsIgnored
        since
        sinceBranch
        sinceLatestTag
      />,
    );
  };

  let cmd = {
    let doc = "verify the required dependencies are included";

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
        $ Args.Changed.since
        $ Args.Changed.sinceBranch
        $ Args.Changed.sinceLatestTag
      ),
      Term.info("verify-deps", ~doc, ~sdocs=Manpage.s_common_options),
    );
  };
};

let cmds = [ListCmd.cmd, VerifyFilesCmd.cmd, VerifyDepsCmd.cmd];