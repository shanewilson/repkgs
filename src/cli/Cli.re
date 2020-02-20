open Cmdliner;
%raw
"process.argv.shift()";

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

let _ =
  try(
    switch (Term.eval_choice(defaultCmd, Cmds.cmds)) {
    | `Error(_) => exit(1)
    | _ => exit(0)
    }
  ) {
  | Js.Exn.Error(err) =>
    Js.log2("Unhandled error:", err);
    exit(1);
  | err =>
    Js.log2("Unhandled error:", err);
    exit(1);
  };