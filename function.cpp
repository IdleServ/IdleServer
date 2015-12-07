// Includes
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include <curl/curl.h>

// IdleServer Functions
namespace idleserver
{

	// Get stats from a proc file
	std::string stats_proc(std::string fileName)
	{
		
		// Open file
		fileName = "/proc/" + fileName;
		std::ifstream fileStream(fileName.c_str(), std::ifstream::in);
		
		// Fail?
		if(!fileStream.good())
			return "";

		// Get contents
		std::string fileContent, fileLine;
		while(getline(fileStream, fileLine))
			fileContent += fileLine + "\n";

		// Close File
		fileStream.close();

		// Get stat
		size_t fileContent_find = fileContent.find_first_of(" ");
		fileContent = fileContent.substr(0, fileContent_find);

		// Done
		return fileContent;

	}
    
    // Check HTTP Servers
    void stats_http(CURL *curl, FILE *curlF, const unsigned short &port, bool &status){
    
        // Check socket first
        if(!status)
            return;
    
        // Make URL
        std::ostringstream ostream;
        ostream << ((port == 80) ? "http" : "https") << "://127.0.0.1";
        if(port != 80)
            ostream << ":" << port;
        std::string url = ostream.str();

        // Do HEAD request
        curl_easy_reset(curl);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, curlF);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
        if(curl_easy_perform(curl) == CURLE_OK)
            status = true;
        else
            status = false;
    
    }

}
