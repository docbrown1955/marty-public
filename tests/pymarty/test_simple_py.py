""" Basic test of python binding for MARTY. """

import pymarty


def main():
    """ Main program. """
    print(pymarty.minkowski_index())
    print(pymarty.minkowski_index("some_name").name)


if __name__ == '__main__':
    main()
