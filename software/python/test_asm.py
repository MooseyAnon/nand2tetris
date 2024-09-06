"""Test assembler code."""

import asm

def test_get_variable():
    assert asm.get_variable("@123") == "123"
    assert asm.get_variable("@ABC") == "ABC"
    assert asm.get_variable("@ABC\n") == "ABC"
    assert asm.get_variable("@ABC_123") == "ABC_123"
    assert asm.get_variable("@ABC_123/") == "ABC_123/"
    assert asm.get_variable("@/ABC_123") == "/ABC_123"
    assert asm.get_variable("@/ABC_123/") == "/ABC_123/"
    assert asm.get_variable("@ABC_123 ") == "ABC_123"
    assert asm.get_variable("@ABC_123    ") == "ABC_123"
    assert asm.get_variable("@ABC_123//") == "ABC_123"

    try:
        asm.get_variable("123")
        assert False

    except ValueError as e:
        assert str(e) == "Invalid Variable"
        assert True


def test_get_symbol_variable():
    assert asm.get_symbol("(TEST_123)") == "TEST_123"
    assert asm.get_symbol("(TEST_123)    ") == "TEST_123"

    invalid_tests = [
        "(TEST//123)", "(TEST 123)",
        "(TEST123", "(TEST123 )",
        "(TEST123 ", "TEST_123)"
    ]
    for test in invalid_tests:
        try:    
            asm.get_symbol(test)
            assert False
        except ValueError as e:
            assert str(e) == "Invalid variable definition"
            assert True


def test_parse_instruction():
    assert asm._parse_instruction("D=M") == {"dest": "D", "comp": "M", "jump": "null"}
    assert asm._parse_instruction("D=M;JGT") == {"dest": "D", "comp": "M", "jump": "JGT"}
    assert asm._parse_instruction("M;JGT") == {"dest": "null", "comp": "M", "jump": "JGT"}
    assert asm._parse_instruction("M") == {"dest": "null", "comp": "M", "jump": "null"}

    try:
        asm._parse_instruction("")
        assert False

    except ValueError as e:
        assert str(e) == "Invalid instruction"
        assert True



def main():
    """Run tests."""
    test_get_variable()
    test_get_symbol_variable()
    test_parse_instruction()


if __name__ == "__main__":
    main()
    print("------ ASM TESTS COMPLETE ------")
