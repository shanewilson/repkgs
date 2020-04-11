include Ink;

[@react.component]
let make = (~error) => {
  switch (error) {
  | `FsDoesNotExist(p)
  | `FsNotADirectory(p)
  | `FsNotAFile(p) =>
    <ErrorWrap>
      <Color red=true bold=true> "File system Error"->React.string </Color>
      <ErrorBorder />
      <ErrorBorder>
        "There was a problem reading from the path:"->React.string
      </ErrorBorder>
      <ErrorBorder />
      <ErrorBorder>
        <Box textWrap="truncate-start"> {p->Path.toString->React.string} </Box>
      </ErrorBorder>
      <ErrorBorder />
      <ErrorBorder>
        <Color red=true bold=true>
          {switch (error) {
           | `FsDoesNotExist(_) => "Path does not exist"
           | `FsNotADirectory(_) => "Path not a directory"
           | `FsNotAFile(_) => "Path not a file"
           | _ => ""
           }}
          ->React.string
        </Color>
      </ErrorBorder>
    </ErrorWrap>
  | _ => ""->React.string
  };
};