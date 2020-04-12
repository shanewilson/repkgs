include Ink;

[@react.component]
let make = (~error) => {
  switch (error) {
  | `WorkspaceErrors(errors) => <WorkspaceError errors />
  };
};