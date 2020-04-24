include Ink;

[@react.component]
let make = (~children=React.null) => {
  <Box>
    <Box marginRight=2> <Color white=true> "|"->React.string </Color> </Box>
    children
  </Box>;
};