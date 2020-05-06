[@decco]
type t = {
  [@decco.key "type"]
  type_: string,
  [@decco.default [||]]
  body: array(t),
  value: option(string),
  source: option(t),
};

let decode = json => {
  switch (json->t_decode) {
  | Ok(ast) => ast
  | Error(_) =>
    Js.Json.parseExn("{\"type\":\"Error\"}") |> t_decode |> Result.getExn
  };
};