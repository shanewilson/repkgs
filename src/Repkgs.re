[@bs.deriving abstract]
type execaOptions = {
  [@bs.optional]
  cwd: string,
  [@bs.optional]
  env: Js.Dict.t(string),
};

type execaChildProcess = {
  command: string,
  exitCode: int,
  stdout: string,
  stderr: string,
  isCanceled: bool,
  killed: bool,
  timedOut: bool,
  failed: bool,
};

[@bs.module "execa"]
external execa:
  (string, array(string), execaOptions) => Js.Promise.t(execaChildProcess) =
  "default";

module Pnpm = {
  type pnpmManifest = {packages: option(array(string))};
  type packageJson = {
    name: string,
    version: string,
    private: bool,
  };
  type pnpmProject = {
    dir: string,
    manifest: packageJson,
  };
  type findPackagesOptions = {
    includeRoot: bool,
    patterns: option(array(string)),
  };

  [@bs.module "read-yaml"] external readYaml: string => pnpmManifest = "sync";

  [@bs.module "find-packages"]
  external findPackages:
    (string, findPackagesOptions) => Js.Promise.t(array(pnpmProject)) =
    "default";

  [@bs.module "@pnpm/constants"]
  external manifestFilename: string = "WORKSPACE_MANIFEST_FILENAME";

  let requirePackagesManifest = dir =>
    Node.Path.join2(dir, manifestFilename) |> readYaml;

  let findWorkspacePackages = workspaceRoot => {
    findPackages(
      workspaceRoot,
      {
        includeRoot: true,
        patterns: (workspaceRoot |> requirePackagesManifest).packages,
      },
    );
  };

  let streamPackageDirs = cwd =>
    cwd
    |> findWorkspacePackages
    |> Wonka.fromPromise
    |> Wonka.map((. v) => v |> Wonka.fromArray)
    |> Wonka.mergeAll
    |> Wonka.map((. v) => v.dir);
};

module Rush = {
  type rushConfigurationProject = {
    projectFolder: string,
    projectRelativeFolder: string,
  };
  type rushConfiguration = {projects: array(rushConfigurationProject)};
  type rushConfigurationModule = {
    loadFromConfigurationFile: string => rushConfiguration,
  };

  [@bs.module "@microsoft/rush-lib"]
  external rushConfiguration: rushConfigurationModule = "RushConfiguration";

  let streamPackageDirs = cwd =>
    cwd
    |> rushConfiguration.loadFromConfigurationFile
    |> Wonka.fromValue
    |> Wonka.map((. v) => v.projects |> Wonka.fromArray)
    |> Wonka.mergeAll
    |> Wonka.map((. v) => v.projectRelativeFolder);
};

module Yarn_V1 = {
  type cmd = {
    type_: string,
    data: string,
  };

  let location = Json.Decode.field("location", Json.Decode.string);
  let decodeWorkspace = Json.Decode.dict(location);

  let decodeCmd = json =>
    Json.Decode.{
      type_: json |> field("type", string),
      data: json |> field("data", string),
    };

  let streamPackageDirs = cwd => {
    execa(
      "yarn",
      [|"workspaces", "info", "--json"|],
      execaOptions(~cwd, ()),
    )
    |> Wonka.fromPromise
    |> Wonka.map((. v) => v.stdout |> Json.parseOrRaise |> decodeCmd)
    |> Wonka.map((. v) => v.data |> Json.parseOrRaise |> decodeWorkspace)
    |> Wonka.map((. v) => Js.Dict.values(v) |> Wonka.fromArray)
    |> Wonka.mergeAll
    |> Wonka.map((. v) => v);
  };
};

module Yarn_V2 = {
  type workspace = {location: string};

  let decodeWorkspace = json =>
    Json.Decode.{location: json |> field("location", string)};

  let streamPackageDirs = cwd => {
    execa(
      "yarn",
      [|"workspaces", "list", "--json"|],
      execaOptions(~cwd, ()),
    )
    |> Wonka.fromPromise
    |> Wonka.map((. v) => v.stdout)
    |> Wonka.map((. v) => v |> String.split_on_char('\n') |> Wonka.fromList)
    |> Wonka.mergeAll
    |> Wonka.map((. v) => v |> Json.parseOrRaise |> decodeWorkspace)
    |> Wonka.map((. v) => v.location);
  };
};

let streamPackageDirs = cwd =>
  cwd
  |> Pnpm.findWorkspacePackages
  |> Wonka.fromPromise
  |> Wonka.map((. v) => v |> Wonka.fromArray)
  |> Wonka.mergeAll
  |> Wonka.map((. v: Pnpm.pnpmProject) => v.dir);