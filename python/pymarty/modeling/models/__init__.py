"""Reference models built with the pure-Python modeling API.

Each module defines one class, a ``Model`` subclass, whose constructor builds
the model and finalizes it. Pass ``finalize=False`` to keep the model open, for
example to derive a close model:

    >>> from pymarty.modeling.models.qed import QED
    >>> model = QED()
    >>> model.is_finalized()
    True
"""

from pymarty.modeling.models.gthdm import GTHDM
from pymarty.modeling.models.mssm import MSSM
from pymarty.modeling.models.nmfv import NMFV
from pymarty.modeling.models.pmssm import PMSSM
from pymarty.modeling.models.qcd import QCD
from pymarty.modeling.models.qed import QED
from pymarty.modeling.models.sm import SM
from pymarty.modeling.models.thdm import THDM

__all__ = ["GTHDM", "MSSM", "NMFV", "PMSSM", "QCD", "QED", "SM", "THDM"]
