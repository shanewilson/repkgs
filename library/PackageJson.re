open Protocol_conv_yaml;
[@deriving protocol(~driver=(module Yaml))]
type t = {
  name: string,
  [@default false]
  private: bool,
  [@default None]
  version: option(string),
};

let manifest_file = Fpath.v("package.json");

let read_parse_manifest = Fs.read_and_parse(~parser=of_yaml);