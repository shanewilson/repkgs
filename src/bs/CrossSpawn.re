[@bs.module "cross-spawn"]
external sync:
  (string, array(string), Node.Child_process.option) =>
  Node.Child_process.spawnResult =
  "sync";