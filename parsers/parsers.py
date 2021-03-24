#!/bin/env python

import re
import sys

DEBUG = 0

response_path = "parsers/exchange/response"
temp_path = "parsers/exchange/temp"

if len(sys.argv) > 1:
    if sys.argv[1] == "-t":
        response_path = "exchange/response"
        temp_path = "exchange/temp"

# Request types

METHOD_DESCRIPTOR = {
    "REDIRECT": "11",
    "SET_PARAMETER": "10",
    "GET_PARAMETER": "9",
    "ANNOUNCE": "8",
    "RECORD": "7",
    "PAUSE": "6",
    "PLAY": "5",
    "TEARDOWN": "4",
    "SETUP": "3",
    "DESCRIBE": "2",
    "OPTIONS": "1"
}

class Parse(object):

    def __init__(self):
        self.parasable = None
        self.request_type = None

    def read_parsable(self):
        with open(temp_path, "r") as f:
            self.parasable = f.read()

        self.request_type = self.parasable[0]

    # Parse text by finding a keyword
    def parse_line(self, keyword):
        sample = re.search(keyword, self.parasable)
        return sample

    @staticmethod
    def get_keyword_value(line):
        separator = ": "
        separator_offset = line.find(separator)
        return line[separator_offset+2:]

    def write_parsed(self, response):
        with open(response_path, "w") as f:
            f.write(response)

    def append_parsed_meta(self, response):
        with open(response_path, "a") as f:
            f.write(response)

    @staticmethod
    def clear():
        with open(temp_path, "w") as f:
            f.write("")

        with open(response_path, "w") as f:
            f.write("")

# initalize
try:
    operation_obj = Parse()
    operation_obj.read_parsable()
    code = operation_obj.request_type
    data = operation_obj.parasable

except:
    raise TypeError("Not working")

else:
    # Operation cases
    if code == METHOD_DESCRIPTOR["OPTIONS"]:
        line = operation_obj.parse_line("Public: (.*)").group()
        output = operation_obj.get_keyword_value(line)
        operation_obj.write_parsed(output+'\n')

        for key, value in METHOD_DESCRIPTOR.items():
            if key in output:
                operation_obj.append_parsed_meta(str(value))
            else:
                operation_obj.append_parsed_meta("0")

    elif code == METHOD_DESCRIPTOR["DESCRIBE"]:
        line = operation_obj.parse_line("Content-Type: (.*)").group()
        content_type = operation_obj.get_keyword_value(line)

        if "application/sdp" in content_type:
            keyword = "a=control:(.*)=(.*)"
            streams = re.findall(keyword, operation_obj.parasable)
            # print(streams)

            # Stream count
            operation_obj.write_parsed("{0}".format(len(streams))+"\n")

            for stream in streams:
                s = "=".join(stream)+"\n"
                operation_obj.append_parsed_meta(s)

            import time
            time.sleep(120)
        else:
            print("UNSUPPORTED FORMAT")

    elif code == METHOD_DESCRIPTOR["SETUP"]:
        session = operation_obj.parse_line("Session: (.*)").group()
        server_port = operation_obj.parse_line("server_port=(.*)-(.*)").group(1)

        if session and int(server_port):
            session = operation_obj.get_keyword_value(session)
            operation_obj.write_parsed("{0}\n{1}\n".format(session, server_port))
