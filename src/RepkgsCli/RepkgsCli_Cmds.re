open Cmdliner;
open Arg;

module Verb = {
  type t =
    | Normal
    | Quiet
    | Verbose
    | Debug;

  let str =
    fun
    | Normal => "normal"
    | Quiet => "quiet"
    | Verbose => "verbose"
    | Debug => "debug";

  let quiet = {
    let doc = "Suppress informational output.";
    (Quiet, info(["q", "quiet"], ~docs=Manpage.s_common_options, ~doc));
  };

  let verbose = {
    let doc = "Give verbose output.";
    (Verbose, info(["v", "verbose"], ~docs=Manpage.s_common_options, ~doc));
  };

  let debug = {
    let doc = "Give only debug output.";
    (Debug, info(["debug"], ~docs=Manpage.s_common_options, ~doc));
  };

  let term = {
    last & vflag_all([Normal], [quiet, verbose, debug]);
  };
};

module Common = {
  type t = {verb: Verb.t};
  let opts = (verb: Verb.t): t => {
    {verb: verb};
  };

  let term = {
    Term.(const(opts) $ Verb.term);
  };

  let print = (oc, opts) => {
    Printf.fprintf(oc, "verbosity = %s\n", Verb.str(opts.verb));
  };
};

module DefaultCmd = {
  let cmd = {
    let doc = "workspace manager";
    let man = [
      `S(Manpage.s_common_options),
      `P("These options are common to all commands."),
      `S("HELP"),
      `P("Use `$(mname) $(i,COMMAND) --help` for help on a single command."),
    ];

    (
      Term.(ret(const(_ => `Help((`Pager, None))) $ Common.term)),
      Term.info(
        "repkgs",
        ~version="v1.0.4",
        ~doc,
        ~sdocs=Manpage.s_common_options,
        ~man,
      ),
    );
  };
};

module ListCmd = {
  let the_code_stuff = (copts, cwd) => {
    Printf.printf("%acwd = %s\n", Common.print, copts, cwd);
    let wsmgr =
      cwd
      |> Repkgs.WorkspaceManager.detectWorkspaceManager
      |> Belt.Result.getExn;
    let dirs = cwd |> Repkgs.WorkspaceManager.findWorkspaceDirectories(wsmgr);
    let ws = dirs |> Repkgs.WorkspaceManager.findWorkspaces;

    ws
    |> Wonka.forEach((. ws: Repkgs.WorkspaceManager.manifest) =>
         Printf.printf("%s\n", ws.name)
       );
  };

  module Args = {
    let cwd = {
      let doc = "Run the program in directory $(docv).";
      // https://github.com/ELLIOTTCABLE/bs-cmdliner/issues/1
      value
      & opt(string, Node.Path.resolve(Sys.getcwd(), ""))
      & info(["cwd"], ~docv="DIR", ~doc);
    };
  };

  let cmd = {
    let doc = "list workspaces found in the current directory";

    (
      Term.(const(the_code_stuff) $ Common.term $ Args.cwd),
      Term.info("list", ~doc, ~sdocs=Manpage.s_common_options),
    );
  };
};

let cmds = [ListCmd.cmd];