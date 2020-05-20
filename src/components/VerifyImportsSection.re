include Ink;

[@react.component]
let make = (~name, ~imports, ~path) => {
  switch (imports->ImportSet.toArray) {
  | [||] => React.null
  | arr =>
    <Box flexDirection="column">
      <Border />
      <Border> <Color bold=true> name->React.string </Color> </Border>
      {arr
       ->Array.map(a =>
           <Border key={a->ImportSet.Import.target->Path.toString}>
             <Color red=true>
               {(
                  "\t"
                  ++ a
                     ->ImportSet.Import.target
                     ->Path.relativize(~cwd=path)
                     ->Path.toString
                )
                ->React.string}
             </Color>
           </Border>
         )
       ->React.array}
    </Box>
  };
};