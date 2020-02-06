open Cmdliner;

let man = [
  `S(Manpage.s_common_options),
  `S(Manpage.s_environment),
  `P("These environment variables affect the execution of $(mname):"),
  `S("MORE HELP"),
  `P("Use `$(mname) $(i,COMMAND) --help' for help on a single command."),
  `Noblank,
  `S(Manpage.s_bugs),
  `P("File bug reports at https://github.com/shanewilson/repkgs"),
];

let version = Package.version;

let sdocs = Manpage.s_common_options;

let exits = Library.Errors.(
  List.concat([
    all()
    |> List.map(_, envVar =>
         Term.exit_info(envVar.exit_code, ~doc=envVar.doc)
       ),
    Term.default_exits,
  ]));
