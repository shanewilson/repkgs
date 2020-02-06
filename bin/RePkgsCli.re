open Cmdliner;

let defaultCmd = {
  let doc = "workspace manager";

  (
    Term.(ret(const(_ => `Help((`Pager, None))) $ const()) $ Logger.args),
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

let _ = switch (Term.eval_choice(defaultCmd, Commands.all)) {
  | `Error(err_code) => Caml.exit(1)
  | _ => Caml.exit(Logs.err_count() > 0 ? 1 : 0)
  };