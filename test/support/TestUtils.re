/** Create a new temporary directory.

    The directory name will be prefixed with "spin-cli-test-" and will be suffixed with a timestamp. */
let getTempDir = name => {
  Printf.sprintf(
    "spin-cli-test-%s-%s",
    name,
    Unix.time() |> Float.to_int |> Int.to_string,
  )
  |> Caml.Filename.concat(Caml.Filename.get_temp_dir_name());
};

/** Run @spin/cli binary with the given arguments and return the standard output. */
let run = args => {
  let arguments = args |> Array.append([|"esy", "start"|]);

  let env =
    Unix.environment()
    |> Array.append([|
         Printf.sprintf(
           "%s=%s",
           Library.Config.SPIN_CLI_CACHE_DIR.name,
           getTempDir("cache_dir"),
         ),
       |]);

  Lwt_process.pread_chars(~env, ("", arguments))
  |> Lwt_stream.to_string
  |> Lwt_main.run;
};
