open Jest;
open Expect;

open Repkgs.Utils;

describe("Utils", () => {
  describe("pathToManifest", () => {
    let path = "dir1/**";
    let mani = path ++ "/package.json";
    test("add package.json to end of globs", () => {
      path |> pathToManifest |> expect |> toEqual(mani)
    });
    test("handle trailing slash", () => {
      path ++ "/" |> pathToManifest |> expect |> toEqual(mani)
    });
  });

  describe("normalizePatterns", () => {
    let static = [|"/path/to/package", "path/to/other-package"|];
    let dynamic = [|"/packages/**", "workspaces/*"|];

    test("add manifest to dynamic static patterns", () => {
      static
      |> normalizePatterns
      |> expect
      |> toEqual([|
           "/path/to/package/package.json",
           "path/to/other-package/package.json",
         |])
    });
    test("add manifest to dynamic patterns", () => {
      dynamic
      |> normalizePatterns
      |> expect
      |> toEqual([|"/packages/**/package.json", "workspaces/*/package.json"|])
    });
    test("handle mix of static and dynamic", () => {
      Belt.Array.concat(static, dynamic)
      |> normalizePatterns
      |> expect
      |> toEqual([|
           "/path/to/package/package.json",
           "path/to/other-package/package.json",
           "/packages/**/package.json",
           "workspaces/*/package.json",
         |])
    });
  });
});