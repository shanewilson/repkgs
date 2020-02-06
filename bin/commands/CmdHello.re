open Cmdliner;

let run = (~nameToGreet, ()) => {
  let greetings = Library.Utils.greet(nameToGreet);  

  Console.log(<Pastel> <Pastel color=Green> greetings </Pastel> </Pastel>);

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

  let runCommand = nameToGreet => run(~nameToGreet) |> Utils.runCmd;

  (
    Term.(const(runCommand) $ nameToGreet),
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