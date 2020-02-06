/** Run @repkgs/cli binary with the given arguments and return the standard output. */
let run = args => {
  let arguments = args |> Array.append([|"esy", "start"|]);

  Lwt_process.pread_chars(("", arguments))
  |> Lwt_stream.to_string
  |> Lwt_main.run;
};
