include Ink;

[@react.component]
let make = (~name, ~required, ~found) => {
  switch (ImportSet.diff(required, found)->ImportSet.toArray) {
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
               {("\t" ++ a->ImportSet.Import.toString)->React.string}
             </Color>
           </Border>
         )
       ->React.array}
    </Box>
  };
};