let setupReporter = (style_renderer, level) => {
  Fmt_tty.setup_std_outputs(~style_renderer?, ());
  Logs.set_level(level);
  Logs.set_reporter(Logs_fmt.reporter());
  ();
};

let args =
  Cmdliner.(
    Term.(
      const(setupReporter)
      $ Fmt_cli.style_renderer(~docs=Cmdliner.Manpage.s_common_options, ())
      $ Logs_cli.level(
          ~docs=Cmdliner.Manpage.s_common_options,
          ~env=Arg.env_var("REPKGS__LOG"),
          (),
        )
    )
  );