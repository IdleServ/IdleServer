// Includes
#include <string>
#include <sstream>
#include <cstdio>
#include <unistd.h>
#include <pwd.h>
#include <curl/curl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include "function.h"

// Start-Up
int main()
{

    // Must be run by root
    if(getuid() != 0)
    {
        return 0;
    }
    
	
	// Run as nobody
	struct passwd *pw;
	pw = getpwnam("nobody");
	setuid(pw->pw_uid);
	setgid(pw->pw_gid);
    
    // Run as daemon
    daemon(0, 0);

	// Server Status
    const unsigned char serverPortsC = 16;
    unsigned short serverPorts[serverPortsC] = {
        21,
        22,
        25,
        53,
        80,
        443,
        110,
        143,
        587,
        993,
        995,
        2083,
        2087,
        2096,
        3306,
        5432
    };
    enum serverPortsK {
        spHTTP = 4,
        spCPANEL = 11,
        spWHM = 12,
        spWEBMAIL = 13
    };
    struct serverStatus
    {
        float load;
        unsigned int uptime;
        bool services[serverPortsC];
    } sS;
    for(unsigned char i = 0; i < serverPortsC; i++)
    {
        sS.services[i] = false;
    }

	// Use istream to convert strings to int & float
	std::istringstream sstream(std::istringstream::in);

    // Use ostream to convert values into a string for post data
    std::ostringstream ostream(std::ostringstream::out);

	// Run Program
	while(true)
    {

		// Get Load 
		sstream.str(idleserver::stats_proc("loadavg"));
		sstream >> sS.load;
		sstream.str("");
		sstream.clear();

		// Get Uptime
		sstream.str(idleserver::stats_proc("uptime"));
		sstream >> sS.uptime;
		sstream.str("");
		sstream.clear();

        // Test Services
        for(unsigned char i = 0; i < serverPortsC; i++)
        {

            // Test Socket
            int sock = socket(AF_INET, SOCK_STREAM, 0);
            timeval sockT;
            sockT.tv_sec = 2;
            sockT.tv_usec = 0;
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (timeval *) &sockT, sizeof(timeval));
            setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (timeval *) &sockT, sizeof(timeval));
            sockaddr_in sockServer;
            sockServer.sin_family = AF_INET;
            sockServer.sin_addr.s_addr = inet_addr("127.0.0.1");
            sockServer.sin_port = htons(serverPorts[i]);
            if(connect(sock, (sockaddr *) &sockServer, sizeof(sockServer)) == 0) {
                sS.services[i] = true;
            }
            close(sock);

        }

        // Open pointers for HTTP communications
        CURL *curl;
        FILE *curlF = fopen("/dev/null", "w");
        curl = curl_easy_init();
        
        // Test HTTP Servers
        idleserver::stats_http(curl, curlF, 2083, sS.services[spCPANEL]);
        idleserver::stats_http(curl, curlF, 2087, sS.services[spWHM]);
        idleserver::stats_http(curl, curlF, 2096, sS.services[spWEBMAIL]);

        // Build post data string
        ostream.precision(2);
        ostream << "load=" << std::fixed << sS.load << "&uptime=" << sS.uptime;
        for(unsigned char i = 0; i < serverPortsC; i++) {
            ostream << "&service[" << serverPorts[i] << "]=" << std::noboolalpha << sS.services[i];
        }
        std::string postfields = ostream.str();
        ostream.str("");
        ostream.clear();

        // Ping Monitoring Server
        curl_easy_reset(curl);
        curl_easy_setopt(curl, CURLOPT_URL, "http://idleserv.net/network/ping");
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postfields.c_str());
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, curlF);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
        curl_easy_perform(curl);

        // Close Pointers
        curl_easy_cleanup(curl);
        fclose(curlF);

		// Update every 10 seconds
		sleep(10);

	}

	// End Program
	return 0;

}
