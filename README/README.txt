Batch No.     : E-18 Batch
Title         : Implementing Core Functionalities of PDS using Hyperledger Fabric
Team Members : GOKUL E (211418104066), HEMANTH S (211418104085), HARI N (211418104074)

Hyperledger Fabric and Application Gateway is hosted as EC2 instance which could be accessed with the provided credentials below. 
After setting up fabric network and gateway by the following steps mentioned below. Application Gateway can be connected to IoT device
by the next set of steps. Then, IoT device can make different types of transaction modes like Create Asset, Read Asset, Transfer Ownership.

I. Setting up Hyperledger Fabric
Step 1: Login to AWS console using credentials below
        Username : hemanths242001@gmail.com
        Password : hemsam123#@
Step 2: After Successful login, start the EC2 instance using SSH username and pass key (PEM or PPK File)
	SSH Client can be either OpenSSH (Linux, Mac) or Windows (Putty)
Step 3: After successful ssh connection, run the following commands to instantiate fabric blockchain network. Following commands create
        a test network with two organizations and a peer in each of those organizations. And, deploy chaincode on those peers
        i  ) cd ~/go/src/github.com/igokuly2k/fabric-samples/test-network 
        ii ) ./network.sh up createChannel -ca
        iii) ./network.sh deployCC -ccn basic -ccp ../asset-transfer-basic/chaincode-go -ccl go
Step 4: After successful instantiation. Run Application Gateway by executing the following commands. Port Forwarding (8080 to 80) should 
        be done and later typescript code should be compiled and executed.
        i  ) cd ../asset-transfer-basic/application-gateway-typescript
        ii ) sudo iptables -A PREROUTING -t nat -i eth0 -p tcp --dport 80 -j REDIRECT --to-port 8080
        iii) npm run serve

II. Setting up IoT device
Step 1: Compile Hyperledger_IoT.ino file in arduino IDE and upload it to ESP8266.
Step 2: Make proper interfacing to RC522 as mentioned in that file.
Step 3  IP address of ESP8266 is shown in serial monitor.
Step 4: Set Modes using following example
        i) http://<<IP ADDRESS OF ESP8266>>/create?owner=hari&type=rice&size=20
Step 5: Start scanning RFID tags
Step 6: Stop mode using the below URL
        i) http://<<IP ADDRESS OF ESP8266>>/stop