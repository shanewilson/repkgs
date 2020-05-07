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
  arguments: option(array(t)),
  declarations: option(array(t)),
  init: option(t),
};

let decode = json => {
  switch (json->t_decode) {
  | Ok(ast) => ast
  | Error(_) => Js.Json.parseExn("{}") |> t_decode |> Result.getExn
  };
};

let parseRequires = ast => {
  switch (ast.type_) {
  | "ImportDeclaration" =>
    switch (ast.source) {
    | Some(s) =>
      switch (s.value) {
      | Some(v) => v
      | None => ""
      }
    | None => ""
    }
  | "ExpressionStatement" =>
    switch (ast.expression) {
    | Some(e) =>
      switch (e.callee) {
      | Some(c) =>
        switch (c.type_) {
        | "Identifier" =>
          switch (c.name) {
          | Some(n) =>
            switch (n) {
            | "require" =>
              switch (e.arguments) {
              | Some(aa) =>
                switch (aa[0]) {
                | Some(a) =>
                  switch (a.value) {
                  | Some(v) => v
                  | None => ""
                  }
                | None => ""
                }
              | None => ""
              }
            | _ => ""
            }
          | None => ""
          }
        | _ => ""
        }
      | None => ""
      }
    | None => ""
    }
  | "VariableDeclaration" =>
    switch (ast.declarations) {
    | Some(ds) =>
      switch (ds[0]) {
      | Some(d) =>
        switch (d.init) {
        | Some(e) =>
          switch (e.callee) {
          | Some(c) =>
            switch (c.type_) {
            | "Identifier" =>
              switch (c.name) {
              | Some(n) =>
                switch (n) {
                | "require" =>
                  switch (e.arguments) {
                  | Some(aa) =>
                    switch (aa[0]) {
                    | Some(a) =>
                      switch (a.value) {
                      | Some(v) => v
                      | None => ""
                      }
                    | None => ""
                    }
                  | None => ""
                  }
                | _ => ""
                }
              | None => ""
              }
            | _ => ""
            }
          | None => ""
          }
        | None => ""
        }
      | None => ""
      }
    | None => ""
    }
  | _ => ""
  };
};