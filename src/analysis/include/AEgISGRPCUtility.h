//#include <iostream>
//#include <string>
//#include <exception>

//#include <TXMLEngine.h>

//using namespace std;


// function to get the ROOT files path from xml/config.xml
string GetRootPathFiles() {
	TXMLEngine* xml = new TXMLEngine;
    	XMLDocPointer_t mDoc = xml->ParseFile("../xml/config.xml");
	if(mDoc == 0) {
		cout << "config xml file is bad formed or not doesn't exist" << endl;
		return "";
	}
    	XMLNodePointer_t mainnode = xml->DocGetRootElement(mDoc); // get the main node "config"
	if(strncmp(xml->GetNodeName(mainnode), "config", 6) != 0) { // strncmp(xml.GetNodeName(mainnode), "config") == 0
		cout << "There is no <config> root at the config xml file" << endl;
		return "";
	}

    	XMLNodePointer_t node = xml->GetChild(mainnode);

	while(node != 0) {
		if(strncmp(xml->GetNodeName(node), "root", 4) == 0) {			
			node = xml->GetChild(node); // get the first child <root> node			
			while (node != 0) {      				
				if(strncmp(xml->GetNodeName(node), "pathFiles", 9) == 0) {
					string rootPath = xml->GetNodeContent(node);
					cout << "Root path files : " << rootPath << endl;
					return rootPath;
				}
				node = xml->GetNext(node);
    			}
			cout << "There is no <pathFiles> child node in <root> node at the config xml file" << endl;
			return "";
		}
		
		node = xml->GetNext(node);
	}
	// Release memory before exit
    	xml->FreeDoc(mDoc);
    	delete xml;

	return "";
}

// functions to treat timeStamp
string GetTimeBySec(long timeStamp) {
	long* stampSec = (long*) &timeStamp;
	return ctime(stampSec);
}
