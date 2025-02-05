## Description/Specifications
my-dns-client.c is a DNS client that can query an existing DNS server for domain name to IP address translation and print out the results of its DNS response in a readable format. The client will make three attempts at getting a DNS response, waiting at most five seconds for each one. If no response is printed within the three attempts, the client will stop 

## Instructions
To compile my-dns-client.c to run/test it, type the following out in your cmd: <br>
gcc my-dns-client.c -o my-dns-client

TO run/test the executable that comes from compiling my-dns-client.c, type the following in your cmd: <br>
./my-dns-client /<hostname-of-your-choice/>

## Result Format
Output from /<hostname-of-your-choice/>: <br>

Preparing DNS query... <br>
Contacting DNS query... <br>
Sending DNS query... <br>
DNS response received! (Attempt 1 out of 3)  <br>
/<possibly more attempts are tried/> <br>
Processing DNS response... <br>
-------------------------------------------------------------------------------- <br>
                             DNS Response's Values: <br>                             
Header: <br>
	header.ID = /<number/> <br>
	header.QR = /<number/> <br>
	header.OPCODE = /<number/> <br>
	header.AA = /<number/> <br>
	header.TC = /<number/> <br>
	header.RD = /<number/> <br>
	header.RA = /<number/> <br>
	header.Z = /<number/> <br>
	header.RCODE = /<number/> <br>
	header.QDCOUNT = /<number/> <br>
	header.ANCOUNT = /<number/> <br>
	header.NSCOUNT = /<number/> <br>
	header.ARCOUNT = /<number/> <br>

Questions:
	Question /<number/>:
		question.QNAME = /<string/>
		question.QTYPE = /<number/>
		question.QCLASS = /<number/>
  /<possibly-more-questions/>

Answers:
	Answer /<number/>:
		answer.NAME = /<string/>
		answer.TYPE = /<number/>
		answer.CLASS = /<number/>
		answer.TTL = /<number/>
		answer.RDLENGTH = /<number/>
		answer.RDATA = /<ip-address/>
  /<possibly-more-answers/>

Authoritative:

Additional:

--------------------------------------------------------------------------------

