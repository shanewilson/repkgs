[@bs.module "ink"] [@react.component]
external make:
  (
    ~width: int=?,
    ~height: int=?,
    ~minWidth: int=?,
    ~minHeight: int=?,
    ~paddingTop: int=?,
    ~paddingBottom: int=?,
    ~paddingLeft: int=?,
    ~paddingRight: int=?,
    ~paddingX: int=?,
    ~paddingY: int=?,
    ~padding: int=?,
    ~marginTop: int=?,
    ~marginBottom: int=?,
    ~marginLeft: int=?,
    ~marginRight: int=?,
    ~marginX: int=?,
    ~marginY: int=?,
    ~margin: int=?,
    ~flexGrow: int=?,
    ~flexShrink: int=?,
    ~flexDirection: string=?,
    ~flexBasis: int=?,
    ~alignItems: string=?,
    ~justifyContent: string=?,
    ~textWrap: string=?,
    ~children: React.element
  ) =>
  React.element =
  "Box";