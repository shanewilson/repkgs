include Ink;

[@react.component]
let make = (~name, ~imports, ~files, ~path) => {
  switch (ImportSet.diff(imports, files)->ImportSet.toArray) {
  | [||] => React.null
  | arr =>
    <Box flexDirection="column">
      <Border>
        <Color cyan=true bold=true> name->React.string </Color>
      </Border>
      {arr
       ->Array.map(a =>
           <Border key={a->ImportSet.Import.toString}>
             <Color red=true>
               {("\t" ++ a->ImportSet.Import.pp(~cwd=path))->React.string}
             </Color>
           </Border>
         )
       ->React.array}
    </Box>
  };
};