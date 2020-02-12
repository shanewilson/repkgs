open Cmdliner;

let run = (~nameToGreet, ()) => {
  let greetings = Library.Utils.greet(nameToGreet);  
  let%lwt _ = Logs_lwt.app(m => m("%s", greetings));
  let%lwt _ = Logs_lwt.info(m => m("%s", greetings));
  let%lwt _ = Logs_lwt.warn(m => m("%s", greetings));
  let%lwt _ = Logs_lwt.err(m => m("%s", greetings));
  let%lwt _ = Logs_lwt.debug(m => m("%s", greetings));

  Lwt.return_ok();
};

let cmd = {
  let doc = "Print a hello world message";

  let nameToGreet = {
    let doc = "The person to greet.";
    Arg.(
      required & pos(0, some(string), None) & info([], ~docv="NAME", ~doc)
    );
  };

  let runCommand = (_, nameToGreet) => run(~nameToGreet) |> Utils.runCmd;

  (
    Term.(const(runCommand) $ Logger.args $ nameToGreet),
    Term.info(
      "hello",
      ~doc,
      ~version=Man.version,
      ~exits=Man.exits,
      ~man=Man.man,
      ~sdocs=Man.sdocs,
    ),
  );
};