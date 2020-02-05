open Cmdliner;

let man = [
  `S(Manpage.s_common_options),
  `S(Manpage.s_environment),
  `P("These environment variables affect the execution of $(mname):"),
  `S("MORE HELP"),
  `P("Use `$(mname) $(i,COMMAND) --help' for help on a single command."),
  `Noblank,
  `S(Manpage.s_bugs),
  `P("File bug reports at https://github.com/Me/spin-cli"),
];

let version = Package.version;

let envs =
  Library.Config.all
  |> Library.Errors.handle_errors
  |> List.map(_, envVar =>
       Library.Config.(
         Term.env_info(
           ~doc=
             Printf.sprintf(
               "%s\ndefaults to \"%s\"",
               envVar.doc,
               envVar.default,
             ),
           envVar.name,
         )
       )
     );

let sdocs = Manpage.s_common_options;

let exits =
  List.concat([
    Library.Errors.all()
    |> List.map(_, envVar =>
         Library.Errors.(Term.exit_info(envVar.exit_code, ~doc=envVar.doc))
       ),
    Term.default_exits,
  ]);
