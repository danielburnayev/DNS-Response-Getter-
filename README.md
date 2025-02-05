## Description/Specifications
my-dns-client.c is a DNS client that can query an existing DNS server for domain name to IP address translation and print out the results of its DNS response in a readable format. The client will make three attempts at getting a DNS response, waiting at most five seconds for each one. If no response is printed within the three attempts, the client will stop 

## Instructions
To compile my-dns-client.c to run/test it, type the following out in your cmd: <br>
gcc my-dns-client.c -o my-dns-client

TO run/test the executable that comes from compiling my-dns-client.c, type the following in your cmd: <br>
./my-dns-client \<hostname-of-your-choice\>

## Result Format
Output from \<hostname-of-your-choice\>: <br>

Preparing DNS query... <br>
Contacting DNS query... <br>
Sending DNS query... <br>
DNS response received! (Attempt 1 out of 3)  <br>
\<possibly more attempts are tried\> <br>
Processing DNS response... <br>
-------------------------------------------------------------------------------- <br>
                             DNS Response's Values: <br>                             
Header: <br>
<pre>	header.ID = \<number\> <pre><br>
/t	header.QR = \<number\> <br>
\t	header.OPCODE = \<number\> <br>
\t	header.AA = \<number\> <br>
\t	header.TC = \<number\> <br>
\t	header.RD = \<number\> <br>
\t	header.RA = \<number\> <br>
\t	header.Z = \<number\> <br>
\t	header.RCODE = \<number\> <br>
\t	header.QDCOUNT = \<number\> <br>
\t	header.ANCOUNT = \<number\> <br>
\t	header.NSCOUNT = \<number\> <br>
\t	header.ARCOUNT = \<number\> <br>
 <br>
Questions: <br>
\t	Question \<number\>: <br>
\t\t		question.QNAME = \<string\> <br>
\t\t		question.QTYPE = \<number\> <br>
\t\t		question.QCLASS = \<number\> <br>
\t \<possibly-more-questions\> <br>
 <br>
Answers: <br>
\t	Answer \<number\>: <br>
\t\t		answer.NAME = \<string\> <br>
\t\t		answer.TYPE = \<number\> <br>
\t\t		answer.CLASS = \<number\> <br>
\t\t		answer.TTL = \<number\> <br>
\t\t		answer.RDLENGTH = \<number\> <br>
\t\t		answer.RDATA = \<ip-address\> <br>
\t \<possibly-more-answers\> <br> <br>

Authoritative: <br> <br>

Additional: <br> <br>

--------------------------------------------------------------------------------  <br>
