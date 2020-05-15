let gatherFilesFromJson: Package.t => ImportSet.t;
let findImports: ImportSet.t => ImportSet.t;
let filesInPack: ImportSet.t => ImportSet.t;
let filesRequiredByPack: ImportSet.t => ImportSet.t;