open Jest;
open Expect;

let testPackageCount = (dirs, finished, stream) => {
  let idx = ref(0);
  stream
  |> Wonka.onEnd((.) => {
       expect(idx^) |> toEqual(List.length(dirs)) |> finished
     })
  |> Wonka.forEach((. _) => idx := idx^ + 1);
};

let () = {
  describe("Pnpm", () => {
    testAsync("streamPackageDirs", finished => {
      let dirs = [
        "__fixtures__/pnpm",
        "__fixtures__/pnpm/workspace-a/package-a",
        "__fixtures__/pnpm/workspace-a/package-b",
        "__fixtures__/pnpm/workspace-b/package-a",
        "__fixtures__/pnpm/workspace-b/package-b",
      ];
      ignore(
        Repkgs.Pnpm.streamPackageDirs("__fixtures__/pnpm")
        |> testPackageCount(dirs, finished),
      );
    })
  });
  describe("Rush", () => {
    testAsync("streamPackageDirs", finished => {
      let dirs = [
        "workspace-a/package-a",
        "workspace-a/package-b",
        "workspace-b/package-a",
        "workspace-b/package-b",
      ];
      ignore(
        Repkgs.Rush.streamPackageDirs("__fixtures__/rush/rush.json")
        |> testPackageCount(dirs, finished),
      );
    })
  });
  describe("Yarn_V1", () => {
    testAsync("streamPackageDirs", finished => {
      let dirs = [
        "workspace-a/package-a",
        "workspace-a/package-b",
        "workspace-b/package-a",
        "workspace-b/package-b",
      ];
      ignore(
        Repkgs.Yarn_V1.streamPackageDirs("__fixtures__/yarn")
        |> testPackageCount(dirs, finished),
      );
    })
  });
  describe("Yarn_V2", () => {
    testAsync("streamPackageDirs", finished => {
      let dirs = [
        ".",
        "workspace-a",
        "workspace-b",
        "workspace-a/package-a",
        "workspace-a/package-b",
        "workspace-b/package-a",
        "workspace-b/package-b",
      ];
      ignore(
        Repkgs.Yarn_V2.streamPackageDirs("__fixtures__/berry")
        |> testPackageCount(dirs, finished),
      );
    })
  });
};