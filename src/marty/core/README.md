# MARTY core

The physics core of MARTY (about 140 headers and sources, the bulk of the code).

Main groups of files:

- **Model**: `model.h/.cpp` (the `Model` class, user entry point), `modelData.h`,
  `modelBuilder.h`, `builtinOperators.h`, `mrtInterface.h` (free functions such as `Display`,
  `Show`, `ComputeFeynmanRules`).
- **Fields and particles**: `quantumField.*`, `quantumFieldTheory.*`, `scalarField.h`,
  `vectorField.h`, `fermionicField.h`, `ghostField.h`, `goldstoneField.h`, `polarization.*`,
  `flavor.*`, `ckm.*`.
- **Gauge groups**: `gauge.*`, `gauged.h`, `gaugedGroup.h`, `group.h`, `semiSimpleAlgebra.h`,
  `su_n_algebra.h`, `representation.h`, `generator.h`, `colorSpace.*`, `groupIndices.h`,
  `colorSimplification.*`, `traceIdentities.h`.
- **Lagrangian and rules**: `lagrangian.h`, `interactionTerm.*`, `feynmanRule.h`, `propagator.h`,
  `feynOptions.h`, `filters.h`, `mrtOptions.h`.
- **Diagrams**: `expander.h`, `graph.h` (Wick contractions), `wick.h`, `insertion.*`,
  `feynmanDiagram.*`, `fermionChain.h`, `fermionFlow.h`, `fermionOrder.h`, `kinematics.h`,
  `momentumConservation.h`.
- **Amplitudes**: `amplitude.*`, `amplitudeInitializer.*`, `amplitudeSimplification.*`,
  `sglsimplify.*`, `sglconverter.h`, `doubleVector.h`, `cache.h`.
- **Wilson coefficients**: `wilson.*`, `wilsonOperator.h`, `simpleWilson.*`, `operatorBasis.h`,
  `penguinpatch.h`.
- **Loops and integrals**: `dimensionalRegularization.*`, `feynmanIntegral.*`,
  `looptools_*.h`, `bernoulli.h`.
- **Library output and display**: `mtylibrary.h` (generated standalone libraries), `drawer.*`
  (`Show` / GRAFED display), `lhaBuiltIn.h` (built-in parameter blocks).
- **Support**: `mrtlog.h`, `mrtError.h`, `mrtUtils.h`, `iterable.h`, `callback.h`.

The `CMakeLists.txt` in this folder builds the object library `marty-core`.

For the pipeline these files implement, see
[../../../docs/architecture/functionalities.md](../../../docs/architecture/functionalities.md).
