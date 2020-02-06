open Cmdliner;

let defaultCmd = {
  let doc = "workspace manager";

  (
    Term.(ret(const(_ => `Help((`Pager, None))) $ const())),
    Term.info(
      "repkgs",
      ~doc,
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

let _ = Term.eval_choice(defaultCmd, Commands.all, ~argv) |> Term.exit;
