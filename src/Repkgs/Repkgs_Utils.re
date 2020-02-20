module Bs = Repkgs_Bs;

let read = paths => paths->Node.Path.join->Node.Fs.readFileAsUtf8Sync;

let pathToManifest = glob => glob->Node.Path.join2("/package.json");

let normalizePatterns = patterns => Belt.Array.map(patterns, pathToManifest);

let findPatternMatches = (patterns, options) =>
  patterns
  ->normalizePatterns
  ->(
      ps => {
        Js.log2("pat?", ps);
        ps;
      }
    )
  ->Bs.FastGlob.sync(options)
  ->Belt.Array.map(Node.Path.dirname)
  ->(
      d => {
        Js.log2("d?", d);
        d;
      }
    );