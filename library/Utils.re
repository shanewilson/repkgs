module Filename = {
  let ensure_trailing = s =>
    switch (Caml.String.get(s, Caml.String.length(s) - 1)) {
    | '/' => s
    | _ => s ++ "/"
    };
};

let greet = name => switch(name) {
  | "error" => raise(Errors.Missing_env_var(name))
  | _ => "Hello " ++ name ++ "!"
}
