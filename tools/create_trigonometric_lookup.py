import math
import sys

TABLE_SIZE = 257
TABLE_NAME = "trigonometricLookupTable"
ATABLE_NAME = "trigonometricReverseLookupTable"

table = [math.sin(math.pi * i / ((TABLE_SIZE - 1) * 2)) for i in range(TABLE_SIZE)]
atable = [math.asin(i / (TABLE_SIZE - 1)) for i in range(TABLE_SIZE)]

def format_float(value):
    return f"{value:.9f}f"


def format_double(value):
    return f"{value:.17g}"

out = sys.stdout

def write_specialization(table, name, ctype):
    formatter = format_float if ctype == "float" else format_double

    out.write(
        "template<>\n"
        f"inline constexpr std::array<{ctype}, {len(table)}> "
        f"{name}<{ctype}> = {{\n"
    )
    for i, value in enumerate(table):
        if i % 4 == 0:
            out.write("    ")
        out.write(formatter(value))
        if i != len(table) - 1:
            out.write(", ")
        if i % 4 == 3 or i == len(table) - 1:
            out.write("\n")
    out.write("};\n\n")


out.write("template<typename T>\n")
out.write(f"inline constexpr std::array<T, {len(table)}> {TABLE_NAME}{{}};\n")

out.write("template<typename T>\n")
out.write(f"inline constexpr std::array<T, {len(atable)}> {ATABLE_NAME}{{}};\n")

write_specialization(table, TABLE_NAME, "float")
write_specialization(table, TABLE_NAME, "double")
write_specialization(atable, ATABLE_NAME, "float")
write_specialization(atable, ATABLE_NAME, "double")