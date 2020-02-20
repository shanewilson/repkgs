module Workspaces = {
  [@decco]
  type workspace = {packages: array(string)};
  let decodeAsArray = json => {
    switch (Decco.arrayFromJson(Decco.stringFromJson, json)) {
    | Ok(ls) => {packages: ls}->Ok
    | Error(err) => err->Error
    };
  };
  let decodeAsObject = json => {
    switch (workspace_decode(json)) {
    | Ok(ws) => ws->Ok
    | Error({message: "Not an object"}) => json->decodeAsArray
    | Error(err) => err->Error
    };
  };
  let encode = workspace_encode;
  let decode = decodeAsObject;
  let codec: Decco.codec(workspace) = (encode, decode);
  [@decco]
  type t = [@decco.codec codec] workspace;
  let packages = w => w.packages->List.fromArray;
};
[@decco]
type t = {
  name: option(string),
  [@decco.default false]
  private: bool,
  version: option(string),
  workspaces: option(Workspaces.t),
};
module Error = {
  type t = [
    | `PackageJsonDecode(Path.t, Decco.decodeError)
    | `PackageJsonParse(Path.t, Fs.Error.t)
  ];
  let handle = e =>
    switch (e) {
    | `PackageJsonDecode(p, err) =>
      Js.log2("path: ", p->Path.pp);
      Js.log2("decode error", err);
    | `PackageJsonParse(p, exn) =>
      Js.log2("path: ", p->Path.pp);
      ignore(Fs.Error.handle(exn));
    };
};
let filename = "package.json";
let name = p => p.name;
let private_ = p => p.private ? `Private : `Public;
let workspaces = p => p.workspaces;
let path = p => p->Path.append(filename->Path.v);
let decode = t_decode;
let encode = t_encode;
let get = s => {
  let p = s->path;
  switch (p->Fs.read) {
  | Error(err) =>
    err->Fs.Error.handle;
    err->Error;
  | Ok(s) =>
    switch (s->Fs.parseJson) {
    | Error(err) => `PackageJsonParse((p, err))->Error
    | Ok(json) =>
      switch (json->decode) {
      | Error(err) => `PackageJsonDecode((p, err))->Error
      | Ok(t) => t->Ok
      }
    }
  };
};