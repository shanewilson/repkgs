open Cmdliner;
%raw
"process.argv.shift()";

module Cmds = RepkgsCli_Cmds;

// Js.log(PackageJson.empty->PackageJson.pp ++ "1");

// let xx: string =
//   (x: Package.t) =>
//     switch (x) {
//     | Package(x) => "p"
//     | PrivatePackage => "pp"
//     };

Term.(eval_choice(Cmds.DefaultCmd.cmd, Cmds.cmds)->exit);