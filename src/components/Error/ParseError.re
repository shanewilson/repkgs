include Ink;

[@react.component]
let make = (~error) => {
  switch (error) {
  | `ParseJson(exn)
  | `ParseYaml(exn) =>
    <Color red=true bold=true>
      {exn->Js.Exn.message->Option.getExn->React.string}
    </Color>
  | _ => ""->React.string
  };
};