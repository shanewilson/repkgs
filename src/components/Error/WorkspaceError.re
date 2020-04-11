include Ink;

[@react.component]
let make = (~errors) => {
  <Box flexDirection="column">
    <Color whiteBright=true>
      "\nFound "->React.string
      <Color bold=true>
        {errors->List.length->Int.toString->React.string}
      </Color>
      " Error(s) trying to create the workspace\n\n"->React.string
    </Color>
    <React.Fragment>
      {errors
       ->List.mapWithIndex((i, error) =>
           <Box key={i->Int.toString}>
             {switch (error) {
              | `FsDoesNotExist(_)
              | `FsNotADirectory(_)
              | `FsNotAFile(_) => <FsError error />
              | `ParseJson(_)
              | `ParseYaml(_) => <ParseError error />
              | `PackageJsonDecode(_)
              | `PackageJsonParse(_) => <PackageJsonError error />
              | `PackageMissingReqField(_) => <PackageError error />
              }}
           </Box>
         )
       ->List.toArray
       ->React.array}
    </React.Fragment>
  </Box>;
};