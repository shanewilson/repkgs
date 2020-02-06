exception Missing_env_var(string);

type error = {
  doc: string,
  exit_code: int,
};

let handleErrors = fn =>
  try%lwt (fn()) {
  | Missing_env_var(name) =>
    Console.error(
      <Pastel color=Pastel.Red>
        {"😱  Oops, it seems you don't have an environment variable named \""
         ++ name
         ++ "\". I need it to work!"}
      </Pastel>,
    );
    Caml.exit(201);
  | _ as exn =>
    Console.log(
      <Pastel color=Pastel.Red>
        {"😱  Oops, an unknown error occurred. You can file a bug reports at https://github.com/shanezilla/repkgs.\n"
         ++ "Here is the stack trace in case it helps:\n"}
      </Pastel>,
    );

    raise(exn);
  }

let all = () => [
  {doc: "on missing required environment variable.", exit_code: 201},
];