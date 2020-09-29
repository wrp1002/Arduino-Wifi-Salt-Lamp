
bool CheckArgs() {
	if (server.args() != 1)
		return false;
	if (server.argName(0) != "brightness")
		return false;
	//if (server.arg(0) != "1" && server.arg(0) != "2")
	//  return false;
	
	return true;
}

void HandleRoot() {
	Serial.println("Sending Homepage...");
	
	String responseHTML = "\
		<html>\
			<head>\
			 <style>\
				 input { outline: none; }\
				 html { font-family: Helvetica;\
					 display: inline-block;\
					 margin: 0px auto;\
					 text-align: center;\
					 font-size: 50px;\
					 background-color: #202020;\
					 color: white; \
				 }\
				 a.button { \
					 background-color: #195B6A; \
					 display: block;\
					 border: none; \
					 color: white; \
					 padding: 16px 40px;\
					 text-decoration: none; \
					 font-size: 75px;\
					 margin: 10px; \
					 cursor: pointer;\
				 }\
				 .mainContainer {\
					 text-align:center;\
					 width: 90%;\
					 padding: 10px 10px;\
					 margin:0 auto;\
				 }\
				 input[type=range] {\
					 -webkit-appearance: none;\
					 width: 95%;\
					 height: 50px;\
					 background-color:#202020\
				 }\
				 input[type=range]::-webkit-slider-runnable-track {\
					 width: 100%;\
					 height: 100px;\
					 cursor: pointer;\
					 box-shadow: 2px 2px 2px rgba(0,0,0,.5);\
					 border-radius: 50px;\
					 border: 0px solid #000101;\
				 }\
				 .brightness::-webkit-slider-runnable-track {\
					 background: linear-gradient(to right,\
						 hsl(0, 0%, 0%) 0%,\
						 hsl(0, 100%, 100%) 100%\
					 );\
				 }\
				 input[type=range]::-webkit-slider-thumb {\
					 height: 100px;\
					 width: 100px;\
					 border-radius: 50%;\
					 cursor: pointer;\
					 -webkit-appearance: none;\
					 background: white;\
					 background-color: rgba(200, 200, 200, .8);\
					 box-shadow: 2px 2px 2px rgba(0,0,0,.5);\
				 }\
			 </style>\
		\
			 <script>\
				 function SendRequest(url) {\
					 var http = new XMLHttpRequest();\
					 http.open('GET', url, true);\
					 http.send();\
				 }\
				 function Update(id) {\
					 value = document.getElementById(id).value;\
					 SendRequest('/setBrightness?brightness=' + value);\
				 }\
				 function On() {\
					 document.getElementById('offButton').style.outline = 'none';\
					 document.getElementById('onButton').style.outline = '5px solid white';\
				 }\
				 function Off() {\
					 document.getElementById('onButton').style.outline = 'none';\
					 document.getElementById('offButton').style.outline = '5px solid white';\
				 }\
			 </script>\
		\
		 </head>\
		 <body onload='if (" + String(powered) + ") On(); else Off();'>\
			 <div class='mainContainer' id='center'>\
				 <h3>ESP8266 Salt Lamp</h3>\
				 <hr>\
				 <br>\
				 <a id='offButton' class='button' onclick='SendRequest(\"/off\"); Off();' style='display:initial; background-color:red'>OFF</a>\
				 <a id='onButton' class='button' onclick='SendRequest(\"/on\"); On();' style='display:initial; background-color:green'>ON</a>\
				 <br><br><hr>\
				 <h3>Brightness</h3>\
				 <input id='light' class='brightness' type='range' min='1' max='100' value='" + String(map(targetBrightness, 0, 255, 0, 100)) + "' onchange='Update(\"light\");' /><br>\
			 <div>\
		 </body>\
		</html>\
	";
	
	//oninput for continuous updates
	
	server.send(200, "text/html", responseHTML);
}

void HandleOn() {
	powered = true;
	server.send(200);
}

void HandleOff() {
	powered = false;
	server.send(200);
}

void HandleStatus() {
	server.send(200, "text/html", String(powered));
}

void HandleGetBrightness() {
	server.send(200, "text/html", String(map(targetBrightness, 0, 255, 0, 100)));
}

void HandleSetBrightness() {
	Serial.println("New set brightness request:");
	for (int i = 0; i < server.args(); i++) {
		Serial.print(server.argName(i));
		Serial.print(": ");
		Serial.println(server.arg(i));
	}
	
	if (server.args() > 0 && CheckArgs()) {
		targetBrightness = map(server.arg(0).toInt(), 0, 100, 0, 255);
		Serial.print("New brightness:");
		Serial.println(targetBrightness);
	}
	
	server.send(200);
}



void HandleNotFound() {
	server.send(404, "text/html", "<h1>Not Found</h1>");
}
