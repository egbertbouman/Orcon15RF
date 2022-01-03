import argparse
import json
import serial
import sys
import time


def read_from_serial(device):
    device.timeout = 10
    lines = device.readlines()
    line = max(set(lines), key = lines.count).decode()
    return line.split('CAAB', 1)[0] + 'CAAB'


def main(argv = None):
    parser = argparse.ArgumentParser(add_help=False, description=('Remote control Orcon MVS 15'))
    parser.add_argument('--help', '-h', action='help', default=argparse.SUPPRESS, help='Show this help message and exit')
    parser.add_argument('--clone', '-c', help='Clone an existing remote')
    parser.add_argument('--file', '-f', help='Configuration file for the remote (in case of cloning, this will be the output file')
    parser.add_argument('--port', '-p', type=str, help='Serial port to use to communicate with RF transceiver')
    parser.add_argument('--cmd', '-m', type=str, help='Name of the command to execute (1/2/3/away/auto/timer')

    args = parser.parse_args() if argv is None else parser.parse_args(argv)

    if not args.port:
        parser.print_usage()
        raise ValueError('you need to specify a port')

    if not args.file:
        parser.print_usage()
        raise ValueError('you need to specify a configuration file')

    if not args.cmd and not args.clone:
        parser.print_usage()
        raise ValueError('you need to specify a command or run the command with the --clone flag')

    print('Connecting to serial device...')
    device = serial.Serial(port=args.port, baudrate=115200, timeout=.1)
    time.sleep(2)

    if args.clone:
        packets = {}
        print('Please follow the intructions in order to clone your existing Orcon remote.')
        for cmd in ['1', '2', '3', 'away', 'auto', 'timer']:
            print(f'Press "{cmd}" once on your remote...')
            packets[cmd] = read_from_serial(device)
        with open(args.file, 'w') as fp:
            fp.write(json.dumps(packets, indent=4))
    elif args.cmd:
        with open(args.file, 'r') as fp:
            packets = json.loads(fp.read())
        if args.cmd not in packets:
            raise ValueError('unknown command, please re-run with the --clone flag')
        print(f'Executing command "{args.cmd}"...')
        device.write(packets[args.cmd].encode() + b'\n')
        device.timeout = 2
        print('Done' if [l for l in device.readlines() if b'B32AAB2A9595A5' in l] else 'Failed to execute command')


if __name__ == "__main__":
    main(sys.argv[1:])
