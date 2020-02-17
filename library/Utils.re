let greet = name =>
  switch (name) {
  | "error" => raise(Errors.Missing_env_var(name))
  | _ => "Hello " ++ name ++ "!"
  };