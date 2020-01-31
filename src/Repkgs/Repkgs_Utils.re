module Bs = Repkgs_Bs;

let read = paths => paths->Node.Path.join->Node.Fs.readFileAsUtf8Sync;

let pathToManifest = glob => glob->Node.Path.join2("/package.json");

let normalizePatterns = patterns => Belt.Array.map(patterns, pathToManifest);

let findPatternMatches = (patterns, options) =>
  patterns
  ->normalizePatterns
  ->Bs.FastGlob.sync(options)
  ->Belt.Array.map(Node.Path.dirname);