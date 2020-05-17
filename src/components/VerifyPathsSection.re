include Ink;

[@react.component]
let make = (~name, ~field) => {
  switch (field) {
  | [] => React.null
  | ls =>
    <Box flexDirection="column">
      <Border />
      <Border> <Color bold=true> name->React.string </Color> </Border>
      {ls
       ->List.map(file =>
           <Border key=file>
             <Color white=true>
               {("\t" ++ file ++ " .....  ")->React.string}
             </Color>
             <Color red=true> "no match"->React.string </Color>
           </Border>
         )
       ->List.toArray
       ->React.array}
    </Box>
  };
};