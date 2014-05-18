import xmlrpclib

p = xmlrpclib.ServerProxy("http:://localhost:8090")
p.di("calltrans","testingtransfer")

