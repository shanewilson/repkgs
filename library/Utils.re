module Glob = Dune_glob.V1;

let greet = name =>
  switch (name) {
  | "error" => raise(Errors.Missing_env_var(name))
  | _ => "Hello " ++ name ++ "!"
  };