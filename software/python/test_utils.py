"""Test utilities."""

import utils

def test_is_number():
    assert utils.is_number("100") == True
    assert utils.is_number("10123") == True
    assert utils.is_number("39") == True
    assert utils.is_number("-39") == True
    assert utils.is_number("123A") == False
    assert utils.is_number("-123A") == False
    assert utils.is_number(" 123") == False
    assert utils.is_number("Abc") == False
    assert utils.is_number("") == False


def test_atoi():
    for i in range(-200, 300, 7):
        assert utils.atoi(str(i)) == i

    try:
        utils.atoi("123a")
        assert False

    except ValueError as e:
        assert str(e) == "All characters in string must be decimal"
        assert True


def test_dec_to_bin():
    for num in range(0, 12800, 500):
        # 016b == pad any values not used under 16 with 0s
        assert utils.dec_to_bin(num) == f"{num:016b}"

    try:
        utils.dec_to_bin("123a")
        assert False

    except ValueError as e:
        assert str(e) == "Input must be a decimal number"
        assert True


def test_bin_to_dec():
    assert utils.bin_to_dec("101") == int("101", 2)

    for i in range(0, 12800, 500):
        binary = utils.dec_to_bin(i)
        assert utils.bin_to_dec(binary) == i


def test_first_char():
    assert utils.first("ABC") == 0
    assert utils.first(" ABC") == 1
    assert utils.first("      ABC") == 6
    assert utils.first("") == -1
    assert utils.first("       ") == -1
    assert utils.first("\n") == -1
    assert utils.first("//") == -1
    assert utils.first(" /") == 1


def test_clean():
    assert utils.clean("ABC_123") == "ABC_123"
    assert utils.clean("     ABC_123") == "ABC_123"
    assert utils.clean("  ABC_123  ") == "ABC_123"
    assert utils.clean("ABC_123\n") == "ABC_123"
    assert utils.clean("ABC_123// comment") == "ABC_123"
    assert utils.clean("ABC 123") == "ABC"
    assert utils.clean("  @ABC") == "@ABC"
    assert utils.clean("  (ABC)   ") == "(ABC)"
    assert utils.clean("") == ""
    assert utils.clean("\n") == ""
    assert utils.clean("        ") == ""
    assert utils.clean("        \n") == ""
    assert utils.clean("// this is a comment \n") == ""


def main():
    """Run tests."""
    test_is_number()
    test_atoi()
    test_dec_to_bin()
    test_bin_to_dec()
    test_first_char()
    test_clean()


if __name__ == "__main__":
    main()
    print("------ UTIL TESTS COMPLETE ------")
