""" First module test for pymarty. """
from typing import Protocol


class Index(Protocol):
    """
    Index used to define tensor expressions.
    """

    @property
    def name(self) -> str:
        """ Index name. Does not contain the unique ID. """

    @name.setter
    def name(self, __s: str) -> None:
        """ Set the index name from a string. """


def minkowski_index(name: str = ...) -> Index:
    """ Generate a Minkowski index. """
