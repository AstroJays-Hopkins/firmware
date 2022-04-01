import argparse
import sexpdata
from sexpdata import loads
def generate_pin_mapping(prefix, port_letter, port_number):
    suffixes = ["PORT", "DDR", "PUE", "PIN"]
    define_list = [f'#define {prefix}_{suffix} {suffix}{port_letter}' for suffix in suffixes]
    output_portno = f'#define {prefix}_PORTNO P{port_letter}{port_number}'
    return define_list, output_portno
def process_pinmap_file(mapfile, header_file):
    with open(mapfile, 'r') as fmap:
        data = loads(fmap.read())
    with open(header_file, 'w') as hout:
        hout.write(f"/* {data[0]} */\n\n#include <avr/io.h>\n")
        for i in range(1, len(data)):
            if isinstance(data[i],str):
                hout.write(f"/* {data[i]} */\n")
            elif isinstance(data[i],sexpdata.Symbol):
                hout.write(f"#define {data[i]._val}\n")
            elif isinstance(data[i],list):
                define_list, output_portno = generate_pin_mapping(data[i][0]._val, data[i][1]._val, data[i][2])
                hout.write(f"{output_portno}\n")
                for item in define_list:
                    hout.write(f"{item}\n")
def main(args):
    process_pinmap_file(args.mapfile, args.header_file)
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--mapfile')
    parser.add_argument('--header_file')
    args = parser.parse_args()
    main(args)