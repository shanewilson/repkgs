[@decco]
type t = {
  [@decco.key "type"]
  type_: string,
  value: option(string),
  source: option(t),
  expression: option(t),
  callee: option(t),
  name: option(string),
  arguments: option(list(t)),
  declarations: option(list(t)),
  init: option(t),
  property: option(t),
};
[@decco]
type program = {
  [@decco.key "type"]
  type_: string,
  [@decco.default [||]]
  // body can be array or object or array ....
  body: array(t),
};

let programDecode = json => {
  Js.log(
    json->NodeUtils.inspect(
      NodeUtils.t(~colors=true, ~depth=Js.Nullable.null, ()),
    ),
  );
  switch (json->program_decode) {
  | Ok(ast) => ast
  | Error(e) =>
    Js.log2("e_start", e);
    Js.Json.parseExn("{\"type\": \"Error\"}")
    |> program_decode
    |> Result.getExn;
  };
};

let decode = json => {
  switch (json->t_decode) {
  | Ok(ast) => ast
  | Error(e) =>
    Js.log2("e_t", e);
    Js.Json.parseExn("{\"type\": \"Error\"}") |> t_decode |> Result.getExn;
  };
};

let rec parseRequires = (~octx=None, oast: option(t)): list(string) => {
  switch (oast, octx) {
  | (None, _) => []
  | (Some(ast), Some(ctx)) =>
    switch (ast.type_) {
    | "MemberExpression" => ast.property |> parseRequires(~octx)
    | "Identifier" =>
      switch (ast.name) {
      | Some("require")
      | Some("resolve") =>
        switch (ctx.arguments) {
        | Some(aa) => aa->List.map(t => Some(t)->parseRequires)->List.flatten
        | None => []
        }
      | _ => []
      }
    | _ => []
    }
  | (Some(ast), None) =>
    switch (ast.type_) {
    | "Literal" =>
      switch (ast.value) {
      | Some(v) => [v]
      | None => []
      }
    | "ImportDeclaration" => ast.source->parseRequires
    | "CallExpression" => ast.callee->parseRequires(~octx=oast, _)
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