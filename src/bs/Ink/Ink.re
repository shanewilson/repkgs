[@bs.module "ink"]
external render: ReasonReact.reactElement => unit = "render";

[@bs.deriving abstract]
type boxProps = {
  [@bs.optional]
  flexDirection: string,
  [@bs.optional]
  children: ReasonReact.reactElement,
  [@bs.optional]
  key: string,
};
[@bs.module "ink"] external box: boxProps => ReasonReact.reactElement = "Box";

module Box = Ink_Box;
module Color = Ink_Color;