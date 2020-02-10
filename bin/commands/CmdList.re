open Cmdliner;

let run = (~cwd, ()) => {
  let _ =
    Library.Manager.find_workspaces(cwd)
    |> List.iter((x: Library.Manager.Workspace.t) =>
         Logs.app(m => m("%s", x.name))
       );

  Lwt.return_ok();
};

let cmd = {
  let doc = "list workspaces found in the current directory";

  let cwd = {
    let doc = "Run the program in directory $(docv).";
    Arg.(value & opt(dir, Sys.getcwd()) & info(["cwd"], ~docv="DIR", ~doc));
  };

  let runCommand = (_, cwd) => run(~cwd) |> Utils.runCmd;

  (
    Term.(const(runCommand) $ Logger.args $ cwd),
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