[@genType]
type t;
[@genType]
let v: string => t;
[@genType]
let toString: t => string;
[@genType]
let pp: t => string;
[@genType]
let pps: List.t(t) => array(string);
[@genType]
let append: (t, t) => t;
[@genType]
let addSeg: (t, string) => t;
[@genType]
let type_: t => [> | `Homedir | `Tmpdir | `Path];
[@genType]
let resolve: (~cwd: t, string) => t;
[@genType]
let relativize: (~cwd: t, t) => t;
[@genType]
let parent: t => t;
[@genType]
let segs: t => List.t(string);
[@genType]
let homedir: unit => t;
[@genType]
let tmpdir: unit => t /* let cwd: unit => t*/ /* [@genType*/;