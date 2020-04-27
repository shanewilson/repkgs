[@bs.deriving abstract]
type options = {
  enums: bool,
  esproposal_decorators: bool,
  esproposal_class_instance_fields: bool,
  esproposal_class_static_fields: bool,
  esproposal_export_star_as: bool,
  esproposal_optional_chaining: bool,
  esproposal_nullish_coalescing: bool,
  types: bool,
};
[@bs.module "flow-parser"]
external _parse: (string, options) => Js.Json.t = "parse";

let parse = s =>
  s->_parse(
    options(
      ~enums=true,
      ~esproposal_decorators=true,
      ~esproposal_class_instance_fields=true,
      ~esproposal_class_static_fields=true,
      ~esproposal_export_star_as=true,
      ~esproposal_optional_chaining=true,
      ~esproposal_nullish_coalescing=true,
      ~types=true,
    ),
  );