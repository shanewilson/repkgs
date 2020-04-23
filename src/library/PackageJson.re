let decodeWithFallback =
    (
      json,
      ~decode: 'a => Result.t('b, Decco.decodeError),
      ~fallback: 'a => Result.t('b, Decco.decodeError),
      ~error,
    )
    : Result.t('b, Decco.decodeError) => {
  switch (json->decode) {
  | Ok(r) => r->Ok
  | Error({message}) when message === error => json->fallback
  | Error(err) => err->Error
  };
};
module Workspaces = {
  [@decco]
  type workspace = {packages: array(string)};
  let decodeFromArray = json => {
    switch (Decco.arrayFromJson(Decco.stringFromJson, json)) {
    | Ok(ls) => {packages: ls}->Ok
    | Error(err) => err->Error
    };
  };
  let encode = workspace_encode;
  let decode =
    decodeWithFallback(
      ~decode=workspace_decode,
      ~fallback=decodeFromArray,
      ~error="Not an object",
    );
  let codec: Decco.codec(workspace) = (encode, decode);
  [@decco]
  type t = [@decco.codec codec] workspace;
  let packages = w => w.packages->List.fromArray;
};

module Bin = {
  [@decco.encode]
  // forcing bin as array, when actually it will be either dict or string in package.json
  // because if it is a string it is auto-converted to a dict of {package_name: bin_string}
  // and we don't have access to package name here
  // also for now we don't care - we really just want the file names
  type bin = array(string);
  let decodeFromString = json => {
    switch (json->Decco.stringFromJson) {
    | Ok(s) => [|s|]->Ok
    | Error(err) => err->Error
    };
  };
  let decodeFromDict = json => {
    switch (Decco.dictFromJson(Decco.stringFromJson, json)) {
    | Ok(d) => d->Js.Dict.values->Ok
    | Error(err) => err->Error
    };
  };
  let encode = bin_encode;
  let decode =
    decodeWithFallback(
      ~decode=decodeFromDict,
      ~fallback=decodeFromString,
      ~error="Not a dict",
    );
  let codec: Decco.codec(bin) = (encode, decode);
  [@decco]
  type t = [@decco.codec codec] bin;
};

[@decco]
type t = {
  name: option(string),
  [@decco.default false]
  private: bool,
  version: option(string),
  main: option(string),
  files: option(array(string)),
  beep: option(Bin.t),
  bin: option(Bin.t),
  devDependencies: option(Js.Dict.t(string)),
  dependencies: option(Js.Dict.t(string)),
  peerDependencies: option(Js.Dict.t(string)),
  workspaces: option(Workspaces.t),
};
module Error = {
  type t = [
    | `PackageJsonDecode(Path.t, Decco.decodeError)
    | `PackageJsonParse(Path.t, Parse.Error.t)
  ];
};
let filename = "package.json";
let name = p => p.name;
let private_ = p => p.private ? `Private : `Public;
let main = p => p.main;
let files = p => p.files->Option.flatMap(arr => Some(arr->List.fromArray));
let bin = p => p.bin->Option.flatMap(arr => Some(arr->List.fromArray));
let workspaces = p => p.workspaces;
let devDependencies = p => p.devDependencies;
let dependencies = p => p.dependencies;
let peerDependencies = p => p.peerDependencies;
let path = p => p->Path.append(filename->Path.v);
let decode = t_decode;
let encode = t_encode;
let get = s => {
  let p = s->path;
  switch (p->Fs.read) {
  | Error(err) => err->Error
  | Ok(s) =>
    switch (s->Parse.json) {
    | Error(err) => `PackageJsonParse((p, err))->Error
    | Ok(json) =>
      switch (json->decode) {
      | Error(err) => `PackageJsonDecode((p, err))->Error
      | Ok(t) => t->Ok
      }
    }
  };
};