import resolve from '@rollup/plugin-node-resolve';
import commonjs from '@rollup/plugin-commonjs';
import { terser } from 'rollup-plugin-terser'

export default {
  input: 'src/RepkgsCli.bs.js',
  output: {
    file: 'bundle.js',
    format: 'cjs'
  },
  plugins: [
    commonjs(),
    resolve({
      // pass custom options to the resolve plugin

      preferBuiltins: true
    }),
    terser()
  ],

  // indicate which modules should be treated as external
  external: ['fast-glob', 'js-yaml', 'path', 'fs']
};
