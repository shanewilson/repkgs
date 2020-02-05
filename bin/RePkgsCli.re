open Cmdliner;

let defaultCmd = {
  let doc = "";

  (
    Term.(ret(const(_ => `Help((`Pager, None))) $ const())),
    Term.info(
      "spin-cli",
      ~doc,
      ~envs=Man.envs,
      ~version=Man.version,
      ~exits=Man.exits,
      ~man=Man.man,
      ~sdocs=Man.sdocs,
    ),
  );
};

let argv =
  Sys.get_argv()
  |> Array.map(~f=arg =>
       switch (arg) {
       | "-v" => "--version"
       | x => x
       }
     );

let _ = Term.exit @@ Term.eval_choice(defaultCmd, Commands.all, ~argv);
