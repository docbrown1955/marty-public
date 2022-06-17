""" Basic test of python binding for MARTY. """

from . import minkowski_index


def main():
    """ Main program. """
    print(minkowski_index())
    print(minkowski_index("some_name").name)


if __name__ == '__main__':
    main()
