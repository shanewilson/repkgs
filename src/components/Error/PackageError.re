include Ink;

[@react.component]
let make = (~error) => {
  switch (error) {
  | `PackageMissingReqField(p, f, r) =>
    <ErrorWrap>
      <Color red=true bold=true>
        "Missing Required Field"->React.string
      </Color>
      <ErrorBorder />
      <ErrorBorder>
        "Missing field in"->React.string
        <Color bold=true> " package.json "->React.string </Color>
        "located at:"->React.string
      </ErrorBorder>
      <ErrorBorder />
      <ErrorBorder>
        <Box textWrap="truncate-start"> {p->Path.toString->React.string} </Box>
      </ErrorBorder>
      <ErrorBorder />
      <ErrorBorder>
        <Color red=true bold=true>
          {("Missing field: " ++ f)->React.string}
        </Color>
      </ErrorBorder>
      <ErrorBorder />
      <ErrorBorder>
        <Color red=true bold=true> r->React.string </Color>
      </ErrorBorder>
    </ErrorWrap>
  | _ => ""->React.string
  };
};