[@decco]
type t = {
  [@decco.key "type"]
  type_: string,
  [@decco.default [||]]
  body: array(t),
  value: option(string),
  source: option(t),
  expression: option(t),
  callee: option(t),
  name: option(string),
  arguments: option(list(t)),
  declarations: option(list(t)),
  init: option(t),
};

let decode = json => {
  switch (json->t_decode) {
  | Ok(ast) => ast
  | Error(_) => Js.Json.parseExn("{}") |> t_decode |> Result.getExn
  };
};

let rec parseRequires = (~ctx=None, oast): list(string) => {
  switch (oast, ctx) {
  | (None, _) => []
  | (Some(ast), Some(ctx)) =>
    switch (ast.type_) {
    | "Identifier" =>
      switch (ast.name->Option.getWithDefault("")) {
      | "require" =>
        switch (ctx.arguments) {
        | Some(aa) => aa->List.map(t => Some(t)->parseRequires)->List.flatten
        | None => []
        }
      | _ => []
      }
    | _ => []
    }
  | (Some(ast), _) =>
    switch (ast.type_) {
    | "Literal" => [ast.value->Option.getWithDefault("")]
    | "ImportDeclaration" => ast.source->parseRequires
    | "CallExpression" => ast.callee |> parseRequires(~ctx=Some(ast))
    | "ExpressionStatement" => ast.expression->parseRequires
    | "VariableDeclarator" => ast.init->parseRequires
    | "VariableDeclaration" =>
      switch (ast.declarations) {
      | Some(ds) => ds->List.map(t => Some(t)->parseRequires)->List.flatten
      | None => []
      }
    | _ => []
    }
  };
};