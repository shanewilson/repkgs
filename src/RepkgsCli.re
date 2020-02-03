open Cmdliner;
%raw
"process.argv.shift()";

module Cmds = RepkgsCli_Cmds;

Term.(eval_choice(Cmds.DefaultCmd.cmd, Cmds.cmds)->exit);