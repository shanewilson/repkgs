exception Missing_env_var(string);
exception Json_parse_error(Fpath.t, string);
exception Fs_dne(Fpath.t);
exception Fs_cwd(string);
exception Fs_error(Fpath.t, string);
exception No_workspace_found;

type error = {
  doc: string,
  exit_code: int,
};

let handleErrors = fn =>
  try%lwt(fn()) {
  | Missing_env_var(name) =>
    Console.error(
      <Pastel color=Pastel.Red>
        {"💥 Oops, it seems you don't have an environment variable named \""
         ++ name
         ++ "\". I need it to work!"}
      </Pastel>,
    );
    exit(201);
  | Fs_cwd(msg) =>
    let%lwt _ =
      Logs_lwt.err(m =>
        m("I could not determine the current working directory")
      );
    let%lwt _ = Logs_lwt.err(m => m("Here is the error:"));
    let%lwt _ = Logs_lwt.err(m => m("%s", msg));
    exit(1);
  | Fs_dne(path) =>
    let%lwt _ =
      Logs_lwt.err(m =>
        m("Looks like the following path does not exist:")
      );
    let%lwt _ = Logs_lwt.err(m => m("%s", path |> Fpath.to_string));
    exit(1);
  | Fs_error(path, msg) =>
    let%lwt _ =
      Logs_lwt.err(m =>
        m("Looks like there was a problem accessing the following path:")
      );
    let%lwt _ = Logs_lwt.err(m => m("%s", path |> Fpath.to_string));
    let%lwt _ = Logs_lwt.err(m => m("Here is the error:"));
    let%lwt _ = Logs_lwt.err(m => m("%s", msg));
    exit(1);
  | Json_parse_error(path, msg) =>
    let%lwt _ =
      Logs_lwt.err(m =>
        m("Looks like there was a problem parsing the following file:")
      );
    let%lwt _ = Logs_lwt.err(m => m("%s", path |> Fpath.to_string));
    let%lwt _ = Logs_lwt.err(m => m("Here is the error:\n\n%s", msg));
    exit(1);
  | No_workspace_found =>
    let%lwt _ =
      Logs_lwt.err(m =>
        m("I was not able to find a valid workspace.")
      );
    
    exit(1);

  | _ as exn =>
    Console.log(
      <Pastel color=Pastel.Red>
        {"💥 Oops, an unknown error occurred. You can file a bug reports at https://github.com/shanezilla/repkgs.\n"
         ++ "Here is the stack trace in case it helps:\n"}
      </Pastel>,
    );
    Printexc.record_backtrace(true);
    let%lwt _ = Logs_lwt.err(m => m("%s", Printexc.to_string(exn)));
    raise(exn);
  };

let all = () => [
  {doc: "on missing required environment variable.", exit_code: 201},
];