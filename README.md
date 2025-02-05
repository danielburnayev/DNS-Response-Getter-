## Description/Specifications
my-dns-client.c is a DNS client that can query an existing DNS server for domain name to IP address translation and print out the results of its DNS response in a readable format. The client will make three attempts at getting a DNS response, waiting at most five seconds for each one. If no response is printed within the three attempts, the client will stop 

## Instructions
To compile my-dns-client.c to run/test it, type the following out in your cmd:
gcc my-dns-client.c -o my-dns-client

TO run/test the executable that comes from compiling my-dns-client.c, type the following in your cmd:
./my-dns-client </hostname-of-your-choice>/

## Result Format
Output from </hostname-of-your-choice>/:

Preparing DNS query...
Contacting DNS query...
Sending DNS query...
DNS response received! (Attempt 1 out of 3) 
</possible more attempts are tried>/
Processing DNS response...
--------------------------------------------------------------------------------
                             DNS Response's Values:                             
Header:
	header.ID = </number>/
	header.QR = </number>/
	header.OPCODE = </number>/
	header.AA = </number>/
	header.TC = </number>/
	header.RD = </number>/
	header.RA = </number>/
	header.Z = </number>/
	header.RCODE = </number>/
	header.QDCOUNT = </number>/
	header.ANCOUNT = </number>/
	header.NSCOUNT = </number>/
	header.ARCOUNT = </number>/

Questions:
	Question </number>/:
		question.QNAME = </string>/
		question.QTYPE = </number>/
		question.QCLASS = </number>/
  </possibly more questions>/

Answers:
	Answer </number>/:
		answer.NAME = </string>/
		answer.TYPE = </number>/
		answer.CLASS = </number>/
		answer.TTL = </number>/
		answer.RDLENGTH = </number>/
		answer.RDATA = </ip address>/
  </possibly more answers>/

Authoritative:

Additional:

--------------------------------------------------------------------------------

