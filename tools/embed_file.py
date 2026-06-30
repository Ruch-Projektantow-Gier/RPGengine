import argparse

if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        prog='embed_file.py',
        description='Embeds binary files into C++ sources and headers.'
    )

    parser.add_argument('filename', help='File to embed.')
    parser.add_argument('codename', help='How to name created array, preceding namespaces allowed eg. my_namespace::my_array.')
    parser.add_argument('--per-line', type=int, default=20, help='How many bytes per line.')
    parser.add_argument('-o', '--output', default=None, help='How to name the output file.')
    parser.add_argument('--header-only', action='store_true', help='Create header only.')

    args = parser.parse_args()
    if args.output is None:
        args.output = args.filename

    with open(args.filename, "rb") as f:
        byte_array = f.read()

    names = args.codename.split('::')
    namespace = '::'.join(names[:-1])
    if namespace == '':
        namespace = None
    array_name = names[-1]
    namespace_tabs = '\t' if namespace is not None else ''

    with open(f"{args.output}.hpp", "w") as f:
        f.write('#pragma once\n')
        f.write('#include <array>\n')
        if namespace is not None:
            f.write(f'namespace {namespace} {{\n')
        f.write(f'{namespace_tabs}inline constexpr std::array<unsigned char, {len(byte_array)}> {array_name} = {{\n')
        for i in range(len(byte_array)):
            if i % args.per_line == 0:
                f.write(f'{namespace_tabs}\t')
            f.write(f'{byte_array[i]},')
            f.write('\n' if i % args.per_line == args.per_line - 1 or i+1 == len(byte_array) else ' ')
        f.write(f'{namespace_tabs}}};')
        if namespace is not None:
            f.write('\n}')