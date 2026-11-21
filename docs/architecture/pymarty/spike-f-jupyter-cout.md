# Spike F: std::cout redirect in Jupyter

Status: PARTIAL. The redirect is verified with pytest (`capsys`). A real Jupyter kernel was not run.

## Question

Does C++ output reach the notebook cell output (not the terminal) when `_debug.print_cout` is called, and does a
long computation print progressively?

## Experiment

- `python/tests/test_output.py`: two tests call `_debug.print_cout` under pytest `capsys`.
- Mechanism tested: `py::scoped_ostream_redirect` around `std::cout << text << '\n' << std::flush`.
- Environment: `/tmp/pymarty-venv` (Python 3.10.12). IPython is not installed in the venv. A system `jupyter`
  binary exists (`/usr/bin/jupyter`), but no `ipykernel` is available to the system Python 3.10, so no kernel was started.

## Result

- Both `test_output.py` tests pass (part of the 18/18 Python tests). Output reaches `sys.stdout`.
- Progressive output of a long computation was not measured.
- Real notebook behaviour (cell output widget, interleaving with Python prints) is unverified.

## Decision (provisional)

Use `py::scoped_ostream_redirect` for the `std::cout` path and flush after each line. Before Phase 6 relies on it,
run one notebook cell that calls a long C++ loop in a kernel with IPython installed. Record that run here.
