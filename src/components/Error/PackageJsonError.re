include Ink;

[@react.component]
let make = (~error) => {
  switch (error) {
  | `PackageJsonDecode(p, (err: Decco.decodeError)) =>
    let steps =
      String.split_on_char('.', err.path)
      ->List.keep(s => s->String.length > 0)
      ->List.toArray;
    let count = steps->Array.length - 1;
    <ErrorWrap>
      <Color red=true bold=true> "Decode Error"->React.string </Color>
      <ErrorBorder />
      <ErrorBorder>
        "Issue decoding"->React.string
        <Color bold=true> " package.json "->React.string </Color>
        "located at:"->React.string
      </ErrorBorder>
      <ErrorBorder />
      <ErrorBorder>
        <Box textWrap="truncate-start"> {p->Path.toString->React.string} </Box>
      </ErrorBorder>
      <ErrorBorder />
      <ErrorBorder> <Color bold=true> "{"->React.string </Color> </ErrorBorder>
      {steps
       ->Array.mapWithIndex((i, s) =>
           <ErrorBorder key={i->Int.toString}>
             <Box marginLeft={2 + i * 2}>
               <Color bold=true>
                 {j|"$s": |j}->React.string
                 {i === count
                    ? <Color red=true>
                        {err.value
                         ->Js.Json.stringifyAny
                         ->Option.getExn
                         ->React.string}
                      </Color>
                    : "{"->React.string}
               </Color>
             </Box>
           </ErrorBorder>
         )
       ->React.array}
      {steps
       ->Array.mapWithIndex((i, _) =>
           <ErrorBorder key={i->Int.toString}>
             <Box key={i->Int.toString} marginLeft={(count - i) * 2}>
               <Color bold=true> "}"->React.string </Color>
             </Box>
           </ErrorBorder>
         )
       ->React.array}
      <ErrorBorder />
      <ErrorBorder>
        <Color red=true bold=true>
          {("Error: " ++ err.message)->React.string}
        </Color>
      </ErrorBorder>
    </ErrorWrap>;

  | `PackageJsonParse(p, err) =>
    <ErrorWrap>
      <Color red=true bold=true> "Parse Error"->React.string </Color>
      <ErrorBorder />
      <ErrorBorder>
        "Issue parsing"->React.string
        <Color bold=true> " package.json "->React.string </Color>
        "located at:"->React.string
      </ErrorBorder>
      <ErrorBorder />
      <ErrorBorder>
        <Box textWrap="truncate-start"> {p->Path.toString->React.string} </Box>
      </ErrorBorder>
      <ErrorBorder />
      <ErrorBorder> <ParseError error=err /> </ErrorBorder>
    </ErrorWrap>
  | _ => ""->React.string
  };
};