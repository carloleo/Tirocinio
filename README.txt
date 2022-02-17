# Key Exchange Over CAN
## Summary
Modern vehicles’ functionalities are managed and regulated via Electronic Control Units (ECU) that interact one another by using the Controller Area Network (CAN) bus.
However, the CAN bus protocol is not secure by designe: it lacks of authentication, confidentiality and integrity. Existing protocols that implement secure communication
among the various ECUs use secret key to secure exchanged messages. However, there is no phase in which such key is shared, thus making it a preliminary part of the actual communication.
The problem of session key sharing is at the heart of the implementation of secure communication. This project designs of a protocol for the exchange of session keys based on the experimental
study of the adoption of RSA in the automotive context, in the light of the constraints and requirements that this application domain imposes.

## Run our code

Instructions for the execution of our Proof Of Concept on Key Exchange:

1.  Run CMakelists.txt to generate the Makefile for the compilation. Digit: `cmake CMakelists.txt`
2.  Run the Makefile obtained to compile. From terminal digit `make` 
3.  Start the Server. Digit the `./Server` command
4.  Start the Client. Digit `./Client -i [server IP]`  (if you want to run the prototype locally, use the following IP address: `127.0.0.1`)
5.  Terminate the server by sending the SIGINT signal (`ctrl + c`) while waiting for a connection

**Contacts**

 - [Carlo Leo]
 - [Gianpiero Costantino]
 - [Ilaria Matteucci]
 - [Anna Bernasconi]
 - [Sowhat] R&D

**Thanks to**

Istituto di Informatica e Telematica ([IIT]) - Consiglio Nazionale Delle Ricerche ([CNR]) - Pisa, Italy

Dipartimento di Informatica - Università di Pisa

License
----

MIT

[CNR]: <https://www.cnr.it>
[IIT]: <https://www.iit.cnr.it>
[Sowhat]: <mailto:sowhat@iit.cnr.it>
[Carlo Leo]: <mailto:carlo_leo97@outlook.it>
[Gianpiero Costantino]: <mailto:gianpiero.costantino@iit.cnr.it>
[Ilaria Matteucci]: <mailto:ilaria.matteucci@iit.cnr.it>
[Anna Bernasconi]: <mailto:anna.bernasconi@unipi.it>
[Sowhat]: <https://sowhat.iit.cnr.it>
