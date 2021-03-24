import re

a = """
v=0
o=StreamingServer 3331435948 1116907222000 IN IP4 192.168.0.113
s=10
c=IN IP4 0.0.0.0
t=0 0
a=control:*
m=video 0 RTP/AVP 96
a=control:trackID=0
"""

sample = re.search("a=control:(.*)=(.*)", a)

print(sample.span())
print(sample.group())
