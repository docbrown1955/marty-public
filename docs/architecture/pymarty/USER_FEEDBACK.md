# Repository layout

I want the following structure in `pymarty`, other files should be removed (except if I forgot something important)
```
src/pymarty/ # (not python/pymarty)
|-- __init__.py # Global imports (as pymarty.X)
|-- _pymarty.py # If needed global objects/functions (maybe the Config class, version, error handling and any global info?)
|-- symbolic/
|   |-- __init__.py  # Imports
|   |-- _symbolic.py # Actual definitions
|-- group_theory/
|   |-- __init__.py
|   |-- _group_theory.py
|-- diracology/
|   |-- __init__.py
|   |-- _diracology_.py
|-- calculation/
|   |-- __init__.py
|   |-- _calculation_.py
|-- simulation/
|   |-- __init__.py
|   |-- _simulation_.py
tests/cpp/
|-- (existing cpp tests)
tests/python/
|-- (existing pymarty tests)
```

With that, we should remove namespace classes as `class modeling` as the namespace will
be provided by the package layout.

Also I would like top-level folders
```
usage_examples/cpp
|-- (existing cpp usage examples)
usage_exampels/python/
|-- (existing usage pymarty examples, including notebooks)
builtin_models/cpp/
|-- (existing cpp models)
builtin_models/python/
|-- (existing pymarty models)
```
I'm not a big fan of 'builtin_models', but 'models' seems a bit vague. To be discussed.

Finally, renaming `dep/` to `third-party/` would be clearer.

In summary:
 - restructure pymarty layout itself
 - move pymarty in src/
 - move pymarty tests in tests/
 - merge usage examples in usage_examples/
 - merge builtin models in builtin_models/
 - rename dep/ into third-party/

# Usage examples

I'm thinking usage examples my be better as notebooks (`.ipynb`) but maybe not all.
For example latex outputs are easier to show in a notebook?

Existing pymarty examples were great to define specs and easy review from my part, now I'm thinking we should 
make them more readable and maybe focus on being a bit more compact. Print more
results and assert less. Prefer nice printing when possible, such as latex, diagrams etc.

We have for now one per module, might be better to have less but more complex examples.
I would suggest:
 - Keep one for `symbolic` (pure expression manip, but let's rediscuss the example)
 - Keep one for `diracology` (same, can be used standalone, current example is fine, but let's review)
 - Keep one for `group_theory` (same as above)
 - Have two examples for overall `pymarty`
    * the `gettingStarted` example (including model building)
    * a more complex example based on the existing SM and generating more complex calculations

Note: all usage examples, including C++, python scripts & notebooks, should be run as part of some test suite asserting that they all return 0.

# API design

## Duplication

We should make sure the C++ API and pymarty layers are as thin as possible and track possible implementation duplications.

## Consistency

We should do a global review of the pymarty layer to ensure consistency across the different pymarty APIs:
 - structure
 - typing
 - naming, conventions
 - error handling, output

## Doc & Remaining gaps

We have a lot of doc on everything that has been done.

We should not rationalize it, and create a simpler doc for:
 - general purpose for pymarty (user-oriented)
 - remaining gaps with the C++ core (dev-oriented)

# Software Delivery

 - Should we provide a wheel?
 - Should we include third-parties? (e.g. compiler)
 - Should we provide a docker container?
 - Should we provide binaries?
 - Should we use github's release?
 - What to test in CI/CD?
 - What about local development? We should have a clear path for both
    * released pymarty
    * local dev (local build folder, etc) including python typing, venv management etc
 - Should we have a wrapper layer on top of the bare CMake build?

# Proposed Process

To faciliate review and efficient feedback, the restructuring can be done first,
and then a human review be requested to potentially amend the other aspects of the feedback.

1. Restructuring code & tests
2. Human review & rework of usage examples
3. Correct API design, improvements
4. Human review
5. Documentation
6. Improve development & release process