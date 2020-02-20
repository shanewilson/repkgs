open Jest;
open Expect;

describe("PackageJson", () => {
  describe("workspaces", () => {
    let obj = {|{"name":"ws","version":"0.0.0","workspaces":{"packages":["path/one","path/two"]}}|};
    let arr = {|{"name":"ws","version":"0.0.0","workspaces":["path/one","path/two"]}|};
    test("workspaces as {}", () => {
      obj
      ->Js.Json.parseExn
      ->PackageJson.decode
      ->Result.getExn
      ->PackageJson.encode
      ->Js.Json.stringify
      |> expect
      |> toEqual(obj)
    });
    test("workspaces as []", () => {
      arr
      ->Js.Json.parseExn
      ->PackageJson.decode
      ->Result.getExn
      ->PackageJson.encode
      ->Js.Json.stringify
      |> expect
      |> toEqual(obj)
    });
  })
});