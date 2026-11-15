# pMSSM (split implementation)

The pMSSM model is split into several translation units so that each part of the Lagrangian can
be compiled separately:

- `pmssm_lem.h`: the model class declaration (in the parent folder, `../pmssm_lem.h`).
- `pmssm_lem.content.cpp`: the particle content (fields and their representations).
- `pmssm_lem.kinetic.cpp`: kinetic terms.
- `pmssm_lem.mass.cpp`: mass terms and their diagonalisation.
- `pmssm_lem.interaction.cpp`: interaction terms.
- `pmssm_data.h`: data definitions used by the model.

Use this folder when you need to change only one part of the pMSSM definition.
