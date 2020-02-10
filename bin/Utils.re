let runCmd = fn => {
  let lwt = Library.Errors.handleErrors(fn)

  lwt
  |> Lwt_main.run
  |> (
    fun
    | Error(err_code) => exit(err_code)
    | Ok () => ()
  );
};