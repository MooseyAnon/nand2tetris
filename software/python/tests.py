"""Base test runner."""

from test_asm import main as asm_main
from test_utils import main as utils_main


def main():
    """Run tests."""
    print("!! RUNNING UTILS TESTS !!")
    utils_main()
    print("!! RUNNING ASM TESTS !!")
    asm_main()

if __name__ == "__main__":
    main()
    print("------ TESTS COMPLETE ------")
